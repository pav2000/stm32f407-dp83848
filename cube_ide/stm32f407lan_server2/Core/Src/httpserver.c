/*
 * htppserver.c
 *
 *  Created on: Jul 13, 2023
 *      Author: pavel
 */

// Конвертация GIF https://www.xconvert.com/compress-gif
/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/apps/fs.h"
#include "string.h"
#include <stdio.h>
#include "httpserver.h"
#include "cmsis_os.h"
static void http_server(struct netconn *conn)
{
	struct netbuf *inbuf;
	err_t recv_err;
	char* buf,*name;
	u16_t buflen;
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // Соединение установлено

	recv_err = netconn_recv(conn, &inbuf); // Чтение данных из порта, блокировка, если еще ничего нет

//	printf("new connection \n");
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // Начало соединения
	if (recv_err == ERR_OK)
	{
		if (netconn_err(conn) == ERR_OK)
		{
			netbuf_data(inbuf, (void**)&buf, &buflen); // Получить указатель данных и длину данных внутри netbuf
			name=get_name_file(buf);  // Вытащить имя запрашиваемого файла
			if (strcmp(name,"")==0) http_file(conn, "index.html"); // Если пустая строка то запрос индекса
			else http_file(conn, name);
		}

	}
	//osDelay(20);
	netconn_close(conn); // Закройте соединение (сервер закрывается в HTTP)
	netbuf_delete(inbuf);  // Удаляем буфер (netconn_recv его выделяет, поэтому мы должны обязательно освободить буфер)
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // Конец соединение
}


static void http_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err, accept_err;

  /* Create a new TCP connection handle */
  conn = netconn_new(NETCONN_TCP);

  if (conn!= NULL)
  {
    /* Bind to port 80 (HTTP) with default IP address */
    err = netconn_bind(conn, IP_ADDR_ANY, 80);

    if (err == ERR_OK)
    {
      /* Put the connection into LISTEN state */
      netconn_listen(conn);

      while(1)
      {
        /* accept any incoming connection */
        accept_err = netconn_accept(conn, &newconn);
        if(accept_err == ERR_OK)
        {
          /* serve connection */
          http_server(newconn);
          /* delete connection */
          netconn_delete(newconn);
        }
      }
    }
  }
}

// задача мигания светодиодом 2
static void blink_thread(void *arg)
{
  while(1)
	  {
	  HAL_GPIO_WritePin(LED3_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	  osDelay(50);
	  HAL_GPIO_WritePin(LED3_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	  osDelay(1000-50);
	  }
}

//Функция http_server_init () просто создаст новый http_thread.
// Размер стека установлен по умолчанию (2*1024 байт), а приоритет - это нормальный.
// 1024 не хватает для стека
void http_server_init()
{
  sys_thread_new("http_thread", http_thread, NULL,2*DEFAULT_THREAD_STACKSIZE, osPriorityNormal);
  sys_thread_new("blink", blink_thread, NULL, 128, osPriorityBelowNormal);
}


// Кинуть файл в сокет (файловая система уже смотнтирована)
FATFS Fs1;   /* Work area (filesystem object) for logical drive */
FIL _fil;        /* File object */
uint8_t line[1024]; /* Line buffer */
FRESULT http_file(struct netconn *conn, char *name){
	FRESULT _fresult;     /* FatFs return code */


	  _fresult = f_mount(&Fs1,"", 1);  // Монтировать карту
	  if (_fresult != FR_OK) { printf("f_mount err: %d \n",_fresult);  return _fresult;	}

	_fresult = f_open(&_fil, (char const *)name, FA_READ); 	// Открыть файл для чтения
	if (_fresult != FR_OK){ printf("file %s f_open err: %d \n", name, _fresult); return _fresult;	}
	printf("load: %s ",name);

	uint br=0,size=0;

	/* Чтение и передача файла */
	for (;;) {
		_fresult = f_read(&_fil, line, sizeof line, &br); /* Read a chunk of data from the source file */
		if (_fresult != FR_OK) {printf("f_read err: %d \n",_fresult); return _fresult;} else printf(".");
		size=size+br;
		if (br == 0) {printf("\n"); break;}  /* Ошибка или конец файла*/
		netconn_write(conn, (const unsigned char*)(line), (size_t)br, NETCONN_NOCOPY); // послать буфер в сокет
		}
//	printf("Real size:%d, read size:%d \n",size,f_size(&_fil));
	_fresult = f_close(&_fil); // закрыть файл
	if (_fresult != FR_OK){ printf("f_close err: %d \n",_fresult);return _fresult;}

	_fresult =  f_mount(NULL,"", 1); // Отмонтировать диск
	if (_fresult != FR_OK){ printf("f_unmount err: %d \n",_fresult);return _fresult;}

return FR_OK;
}

// Извлечь имя файла из запроса (ищет заголовок и далее идет имя до кода 0x0a)
// возвращает или имя или null
#define HEAD1 "GET /"
char* get_name_file(char *buf){
char *ptr;
ptr = strstr(buf,HEAD1)+strlen(HEAD1);

for(uint16_t i=0;i<strlen(ptr);i++) // Копирование имени
      if ((ptr[i]==0x20)||(ptr[i]=='?')) {ptr[i]=0; break;}
return ptr;
}

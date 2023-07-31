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
	char* buf;
	u16_t buflen;
	struct fs_file file;
	uint16_t len;
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // Соединение установлено

	recv_err = netconn_recv(conn, &inbuf); // Чтение данных из порта, блокировка, если еще ничего нет

	printf("new connection \n");
	if (recv_err == ERR_OK)
	{
		if (netconn_err(conn) == ERR_OK)
		{
			netbuf_data(inbuf, (void**)&buf, &buflen); // Получить указатель данных и длину данных внутри netbuf

			if ((strncmp((char const *)buf,"GET /index.html",15)==0)||(strncmp((char const *)buf,"GET / HTTP/1.1\r\n",16)==0)) // Проверьте, есть ли запрос на получение index.html или GET / HTTP/1.1\r\n
			{
				http_file(conn, "index.html");
			}
			else
			{
			if (strncmp((char const *)buf,"GET /img/lwip.gif",14)==0) // Получение картинки
						{
				http_file(conn, "img/lwip.gif");
						//	fs_open(&file, "/img/lwip.gif");
					//		netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
			//	printf("GET /img/lwip.gif \n");
						//	fs_close(&file);
						}
				else{
				/* Load Error page */
			//	fs_open(&file, "/404.html");
			//	netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
				http_file(conn, "404.html");
				//	printf("/404.html \n");
			//	fs_close(&file);
				}
			}
		}
	}

	netconn_close(conn); // Закройте соединение (сервер закрывается в HTTP)

	netbuf_delete(inbuf);  // Удаляем буфер (netconn_recv его выделяет, поэтому мы должны обязательно освободить буфер)
//	HAL_GPIO_WritePin(LED3_GPIO_Port, LED1_Pin, GPIO_PIN_SET); // Конец соединение
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

//Функция http_server_init () просто создаст новый http_thread.
// Размер стека установлен по умолчанию (1024 байт), а приоритет - это нормальный.
void http_server_init()
{
  sys_thread_new("http_thread", http_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);
}

// Кинуть файл в сокет
volatile static FATFS FatFs;   /* Work area (filesystem object) for logical drive */
volatile static FIL _fil;        /* File object */
uint8_t line[1024]; /* Line buffer */
FRESULT _fresult;     /* FatFs return code */

FRESULT http_file(struct netconn *conn, char *name){
	_fresult = f_mount(&FatFs,"", 1);  // Монтировать карту
	if (_fresult != FR_OK) {printf("f_mount err: %d \n",_fresult);return _fresult;} else printf("f_mount Ok \n");

	_fresult = f_open(&_fil, (char const *)name, FA_READ); 	// Открыть файл для чтения в корне
	if (_fresult != FR_OK) { printf("f_open err: %d \n",_fresult); return _fresult;} else printf("f_open Ok \n");
	printf("name file: %s\n",name);

	int br;
	/* Copy source to destination */
	for (;;) {
		_fresult = f_read(&_fil, line, sizeof line, &br); /* Read a chunk of data from the source file */
		if (_fresult != FR_OK) {printf("f_read err: %d \n",_fresult); return _fresult;} else printf(".");
		if (br == 0) break; /* error or eof */
		netconn_write(conn, (const unsigned char*)(line), (size_t)br, NETCONN_NOCOPY);
	//	printf("netconn_write %d butes \n",br);
		}

	/* Close the file */
	f_close(&_fil);

	_fresult =  f_mount(NULL,"", 1);
	if (_fresult != FR_OK){ printf("f_unmount err: %d \n",_fresult);return _fresult;} else printf("f_unmount Ok \n");
	 //   free(FatFs);

}



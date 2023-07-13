/*
 * htppserver.c
 *
 *  Created on: Jul 13, 2023
 *      Author: pavel
 */

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
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); // Соединение установлено

	recv_err = netconn_recv(conn, &inbuf); // Чтение данных из порта, блокировка, если еще ничего нет

	if (recv_err == ERR_OK)
	{
		if (netconn_err(conn) == ERR_OK)
		{
			netbuf_data(inbuf, (void**)&buf, &buflen); // Получить указатель данных и длину данных внутри netbuf

			if ((strncmp((char const *)buf,"GET /index.html",15)==0)||(strncmp((char const *)buf,"GET / HTTP/1.1\r\n",16)==0)) // Проверьте, есть ли запрос на получение index.html или GET / HTTP/1.1\r\n
			{
				fs_open(&file, "/index.html");
				netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
				fs_close(&file);
			}
			else
			{
			if (strncmp((char const *)buf,"GET /img/lwip1.jpg",14)==0) // Получение картинки
						{
							fs_open(&file, "/img/lwip1.jpg");
							netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
							fs_close(&file);
						}
				else{
				/* Load Error page */
				fs_open(&file, "/404.html");
				netconn_write(conn, (const unsigned char*)(file.data), (size_t)file.len, NETCONN_NOCOPY);
				fs_close(&file);
				}
			}
		}
	}

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

//Функция http_server_init () просто создаст новый http_thread.
// Размер стека установлен по умолчанию (1024 байт), а приоритет - это нормальный.
void http_server_init()
{
  sys_thread_new("http_thread", http_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);
}





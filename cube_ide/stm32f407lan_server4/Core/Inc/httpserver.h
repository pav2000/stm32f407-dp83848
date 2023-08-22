/*
 * htppserver.h
 *
 *  Created on: Jul 13, 2023
 *      Author: pavel
 */

#ifndef __HTTPSERVER_NETCONN_H__
#define __HTTPSERVER_NETCONN_H__

#include "lwip/api.h"
#include "fatfs.h"
void http_server_init(void);
FRESULT http_file(struct netconn *conn, char *name);
char* get_name_file(char *buf);
#endif /* __HTTPSERVER_NETCONN_H__ */

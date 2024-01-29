#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/ssr.h"
#include "include/base64.h"
#include "include/parse-ssr-v2ray.h"

typedef struct {
	char* name;
	int name_len;
	char* value;
	int nfree;
} ss_ssr_server;

/********************************************************************/
/* SS                                                               */
/********************************************************************/

typedef enum {
	SS_SERVER_PARAM_METHOD = 0,
	SS_SERVER_PARAM_PASSWORD,
	SS_SERVER_PARAM_HOST,
	SS_SERVER_PARAM_PORT,
	SS_SERVER_PARAM_MAX
} SS_SERVER_PARAM;

static ss_ssr_server ssserver[SS_SERVER_PARAM_MAX] = {
	{ "method"     ,  6 , },
	{ "password"   ,  8 , },
	{ "host"       ,  4 , },
	{ "port"       ,  4 , },
};

static int show_ss_server(ss_ssr_server serv[], int mode)
{
	if (mode == PARSE_SHOW_LIST) {
		int i;
		for (i = 0; i < SS_SERVER_PARAM_MAX; i++) {
			if (ssserver[i].value)
				printf("%-13s: %s\n", ssserver[i].name, ssserver[i].value);
		}
	} else if (mode == PARSE_SHOW_SHELL) {
		int i, printf_count = 0;
		for (i = 0; i < SS_SERVER_PARAM_MAX; i++) {
			if (ssserver[i].value) {
				if (printf_count == 0) {
					printf("ss_%s=\"%s\"", ssserver[i].name, ssserver[i].value);
				} else {
					printf(";ss_%s=\"%s\"", ssserver[i].name, ssserver[i].value);
				}
				printf_count++;
			}
		}
		if (printf_count) {
			printf("\n");
		}		
	}	

	return 0;
}

int parse_ss_server(char* server, ss_ssr_server serv[])
{
	if (server == NULL) {
		return 1;
	}

	int i = 0;
	char* temp = NULL;
	while (temp = strsep(&server, ":@")) {
		serv[i++].value = temp;
	}

	if (i != (SS_SERVER_PARAM_PORT + 1)) {
		return 1;
	}

	for (i = 0; i <= SS_SERVER_PARAM_PORT; i++) {
		if (serv[i].value == NULL || strlen(serv[i].value) == 0) {
			return 1;
		}
	}

	return 0;
}

int parse_ss_param(char* param, ss_ssr_server serv[])
{
	if (param == NULL) {
		return 1;
	}

// 	char* temp = NULL;
// 	while (temp = strsep(&param, "&")) {
// 		int i, length = strlen(temp);
// 		for (i = 0; i < sizeof(ssrserver) / sizeof(ssrserver[0]); i++) {
// 			if ((length >= (ssrserver[i].name_len + 1)) && (temp[ssrserver[i].name_len] == '=') && (strncmp(temp, ssrserver[i].name, ssrserver[i].name_len) == 0)) {
// 				temp = temp + ssrserver[i].name_len + 1;
// 				length = strlen(temp);
// 				if (length > 0) {
// 					temp = Base64Decode(temp, length, 0);
// 					if (temp == NULL) {
// 						return 1;
// 					}
// 					serv[i].value = temp;
// 					serv[i].nfree = 1;
// 				}
// 				break;
// 			}
// 		}
// 	}

	return 0;
}

int parse_ss_server_param(const char* link, ss_ssr_server serv[])
{
	if (link == NULL) {
		return 1;
	}

	int i, len;
	char* param = strchr(link, '#');
	char* server = Base64Decode(link, (param == NULL) ? strlen(link) : (param - link), 0);
	if (server == NULL || (len = strlen(server)) == 0) {
		return 1;
	}

	// for (i = 1; i < len; i++) {
	// 	if (server[i] == '?' && server[i - 1] == '/') {
	// 		server[i - 1] = '\0';
	// 		param = &server[i + 1];
	// 		break;
	// 	}
	// }

	if (parse_ss_server(server, serv) != 0 || parse_ss_param(param, serv) != 0) {
		free(server);
		return 1;
	}

	if (server == serv[0].value) {
		serv[0].nfree = 1;
	} else {
		free(server);
	}

	return 0;
}

int parse_ss(const char* link, int mode)
{
	if (link == NULL) {
		return 1;
	}

	int i, ret;
	for (i = 0; i < SS_SERVER_PARAM_MAX; i++) {
		ssserver[i].value = NULL;
		ssserver[i].nfree = 0;
	}

	ret = parse_ss_server_param(link, ssserver);

	show_ss_server(ssserver, mode);

	for (i = 0; i < SS_SERVER_PARAM_MAX; i++) {
		if (ssserver[i].value != NULL && ssserver[i].nfree == 1) {
			free(ssserver[i].value);
		}
	}

	return ret;
}

/********************************************************************/
/* SSR                                                              */
/********************************************************************/

typedef enum {
	SSR_SERVER_PARAM_HOST = 0,
	SSR_SERVER_PARAM_PORT,
	SSR_SERVER_PARAM_PROTOCOL,
	SSR_SERVER_PARAM_METHOD,
	SSR_SERVER_PARAM_OBFS,
	SSR_SERVER_PARAM_PASSWORD,
	SSR_SERVER_PARAM_PROTOCOL_PARAM,
	SSR_SERVER_PARAM_OBFS_PARAM,
	SSR_SERVER_PARAM_REMARKS,
	SSR_SERVER_PARAM_GROUP,
	SSR_SERVER_PARAM_MAX
} SSR_SERVER_PARAM;

static ss_ssr_server ssrserver[SSR_SERVER_PARAM_MAX] = {
	{ "host"       ,  4 , },
	{ "port"       ,  4 , },
	{ "protocol"   ,  8 , },
	{ "method"     ,  6 , },
	{ "obfs"       ,  4 , },
	{ "password"   ,  8 , },
	{ "protoparam" , 10 , },
	{ "obfsparam"  ,  9 , },
	{ "remarks"    ,  7 , },
	{ "group"      ,  5 , },
};

static int show_ssr_server(ss_ssr_server serv[], int mode)
{
	if (mode == PARSE_SHOW_LIST) {
		int i, printf_count = 0;
		for (i = 0; i < SSR_SERVER_PARAM_MAX; i++) {
			if (ssrserver[i].value)
				printf("%-13s: %s\n", ssrserver[i].name, ssrserver[i].value);
		}
	} else if (mode == PARSE_SHOW_SHELL) {
		int i, printf_count = 0;
		for (i = 0; i < SSR_SERVER_PARAM_MAX; i++) {
			if (ssrserver[i].value) {
				if (printf_count == 0) {
					printf("ssr_%s=\"%s\"", ssrserver[i].name, ssrserver[i].value);
				} else {
					printf(";ssr_%s=\"%s\"", ssrserver[i].name, ssrserver[i].value);
				}
				printf_count++;
			}
		}
		if (printf_count) {
			printf("\n");
		}
	}
	return 0;
}

int parse_ssr_server(char* server, ss_ssr_server serv[])
{
	if (server == NULL) {
		return 1;
	}

	int i = 0;
	char* temp = NULL;
	while (temp = strsep(&server, ":")) {
		serv[i++].value = temp;
	}

	if (i != (SSR_SERVER_PARAM_PASSWORD + 1)) {
		return 1;
	}

	for (i = 0; i <= SSR_SERVER_PARAM_PASSWORD; i++) {
		if (serv[i].value == NULL || strlen(serv[i].value) == 0) {
			return 1;
		}
	}

	char* password = Base64Decode(serv[SSR_SERVER_PARAM_PASSWORD].value, strlen(serv[SSR_SERVER_PARAM_PASSWORD].value), 0);
	if (password == NULL || strlen(password) == 0) {
		return 1;
	}

	serv[SSR_SERVER_PARAM_PASSWORD].value = password;
	serv[SSR_SERVER_PARAM_PASSWORD].nfree = 1;
	return 0;
}

int parse_ssr_param(char* param, ss_ssr_server serv[])
{
	if (param == NULL) {
		return 0;
	}

	char* temp = NULL;
	while (temp = strsep(&param, "&")) {
		int i, length = strlen(temp);
		for (i = 0; i < sizeof(ssrserver) / sizeof(ssrserver[0]); i++) {
			if ((length >= (ssrserver[i].name_len + 1)) && (temp[ssrserver[i].name_len] == '=') && (strncmp(temp, ssrserver[i].name, ssrserver[i].name_len) == 0)) {
				temp = temp + ssrserver[i].name_len + 1;
				length = strlen(temp);
				if (length > 0) {
					temp = Base64Decode(temp, length, 0);
					if (temp == NULL) {
						return 1;
					}
					serv[i].value = temp;
					serv[i].nfree = 1;
				}
				break;
			}
		}
	}

	return 0;
}

int parse_ssr_server_param(const char* link, ss_ssr_server serv[])
{
	if (link == NULL) {
		return 1;
	}

	int i, len;
	char* server = Base64Decode(link, strlen(link), 0), *param = NULL;
	if (server == NULL || (len = strlen(server)) == 0) {
		return 1;
	}

	for (i = 1; i < len; i++) {
		if (server[i] == '?' && server[i - 1] == '/') {
			server[i - 1] = '\0';
			param = &server[i + 1];
			break;
		}
	}

	if (parse_ssr_server(server, serv) != 0 || parse_ssr_param(param, serv) != 0) {
		free(server);
		return 1;
	}

	if (server == serv[0].value) {
		serv[0].nfree = 1;
	} else {
		free(server);
	}

	return 0;
}

int parse_ssr(const char* link, int mode)
{
	if (link == NULL) {
		return 1;
	}

	int i, ret;
	for (i = 0; i < SSR_SERVER_PARAM_MAX; i++) {
		ssrserver[i].value = NULL;
		ssrserver[i].nfree = 0;
	}

	ret = parse_ssr_server_param(link, ssrserver);

	show_ssr_server(ssrserver, mode);

	for (i = 0; i < SSR_SERVER_PARAM_MAX; i++) {
		if (ssrserver[i].value != NULL && ssrserver[i].nfree == 1) {
			free(ssrserver[i].value);
		}
	}

	return ret;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "include/v2ray.h"
#include "include/base64.h"
#include "include/parse-ssr-v2ray.h"

typedef struct {
	char* name;
	int name_len;
	char* value;
	int nfree;
} v2ray_server;

/********************************************************************/
/* V2RAY                                                            */
/********************************************************************/

typedef enum {
	V2RAY_SERVER_PARAM_ADD = 0,
	V2RAY_SERVER_PARAM_AID,
	V2RAY_SERVER_PARAM_HOST,
	V2RAY_SERVER_PARAM_ID,
	V2RAY_SERVER_PARAM_NET,
	V2RAY_SERVER_PARAM_PATH,
	V2RAY_SERVER_PARAM_PORT,
	V2RAY_SERVER_PARAM_PS,
	V2RAY_SERVER_PARAM_TLS,
	V2RAY_SERVER_PARAM_TYPE,
	V2RAY_SERVER_PARAM_V,
	V2RAY_SERVER_PARAM_MAX
} V2RAY_SERVER_PARAM;

static v2ray_server v2rayserver[V2RAY_SERVER_PARAM_MAX] = {
	{ "add"   ,  3 , },
	{ "aid"   ,  3 , },
	{ "host"  ,  4 , },
	{ "id"    ,  2 , },
	{ "net"   ,  3 , },
	{ "path"  ,  4 , },
	{ "port"  ,  4 , },
	{ "ps"    ,  2 , },
	{ "tls"   ,  3 , },
	{ "type"  ,  4 , },
	{ "v"     ,  1 , },
};

static int show_v2ray_server(v2ray_server serv[], int mode)
{
	if (mode == PARSE_SHOW_LIST) {
		int i;
		for (i = 0; i < V2RAY_SERVER_PARAM_MAX; i++) {
			if (v2rayserver[i].value)
				printf("%-13s: %s\n", v2rayserver[i].name, v2rayserver[i].value);
		}
	} else if (mode == PARSE_SHOW_SHELL) {
		int i, printf_count = 0;
		for (i = 0; i < V2RAY_SERVER_PARAM_MAX; i++) {
			if (v2rayserver[i].value) {
				if (printf_count == 0) {
					printf("v2ray_%s=\"%s\"", v2rayserver[i].name, v2rayserver[i].value);
				} else {
					printf(";v2ray_%s=\"%s\"", v2rayserver[i].name, v2rayserver[i].value);
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

int parse_v2ray_server(char* server, v2ray_server serv[])
{
	if (server == NULL) {
		return 1;
	}

	json_object* json_server = NULL;
	if ((json_server = json_tokener_parse(server)) == NULL) {
		return 1;
	}

	json_object_object_foreach(json_server, json_key, json_value) {
		int i;
		for (i = 0; i < sizeof(v2rayserver) / sizeof(v2rayserver[0]); i++) {
			if (strcmp(json_key, v2rayserver[i].name) == 0) {
				const char* value = json_object_get_string(json_value);
				if (value == NULL || strlen(value) == 0) {
					continue;
				}
				if ((v2rayserver[i].value = strdup(value)) == NULL) {
					goto err;
				}
				v2rayserver[i].nfree = 1;
				break;
			}
		}
	}

	json_object_put(json_server);
	return 0;
err:
	json_object_put(json_server);
	return 1;
}

int parse_v2ray_param(char* param, v2ray_server serv[])
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

int parse_v2ray_server_param(const char* link, v2ray_server serv[])
{
	if (link == NULL) {
		return 1;
	}

	int i, len;
	char* server = Base64Decode(link, strlen(link), 0), *param = NULL;
	if (server == NULL || (len = strlen(server)) == 0) {
		return 1;
	}

	if (parse_v2ray_server(server, serv) != 0 || parse_v2ray_param(param, serv) != 0) {
		free(server);
		return 1;
	}

	free(server);
	return 0;
}

int parse_v2ray(const char* link, int mode)
{
	if (link == NULL) {
		return 1;
	}

	int i, ret;
	for (i = 0; i < V2RAY_SERVER_PARAM_MAX; i++) {
		v2rayserver[i].value = NULL;
		v2rayserver[i].nfree = 0;
	}

	ret = parse_v2ray_server_param(link, v2rayserver);

	show_v2ray_server(v2rayserver, mode);

	for (i = 0; i < V2RAY_SERVER_PARAM_MAX; i++) {
		if (v2rayserver[i].value != NULL && v2rayserver[i].nfree == 1) {
			free(v2rayserver[i].value);
		}
	}

	return ret;
}

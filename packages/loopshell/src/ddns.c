#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

#include <curl/curl.h>
#include <curl/easy.h>

const char IPADDR_API_V4[] = "http://checkip.synology.com";
const char IPADDR_API_V6[] = "http://checkipv6.synology.com";
const char* IPADDR_API = IPADDR_API_V4;

static void ddns_help()
{
	printf("ddns [OPTIONS...]\n"
		   "  -h            Show this help\n"
		   "  -v            4 | 6\n");
}

static int parse_argv(int argc, char **argv)
{
	static const struct option options[] = {
		{ "help",      no_argument,       NULL,  'h'    },
		{ "ver",       required_argument, NULL,  'v'    },
		{}
	};

	int c, ver;
	while ((c = getopt_long(argc, argv, "v:h", options, NULL)) >= 0) {
		ver = 0;
		switch (c) {
		case 'h':
			ddns_help();
			return 0;
		case 'v':
			ver = atoi(optarg);
			if (ver == 4) {
				IPADDR_API = IPADDR_API_V4;
			} else if (ver == 6) {
				IPADDR_API = IPADDR_API_V6;
			} else {
				printf("-v 4|6\n");
				return 0;
			}
			break;
		case '?':
			return 0;
		default:
			;
		}
	}

	// if (optind == argc) {
	// 	printf("no string ?\n");
	// 	return 0;
	// }

	return 1;
}

static size_t parse_synology_ip( void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t length = 0;
	if ((length = size * nmemb) > 0) {
		char *ipmark = "Current IP Address: ";
		char buf[256] = {0}, *start = NULL, *end = NULL;	

		if ((start = strstr(ptr, ipmark)) != NULL) {
			start += strlen(ipmark);
			for (end = start; end < (char*)ptr + length; end++) {
				if (*end == '<') {
					break;
				}
			}
			memcpy(buf, start, end - start);
			printf("%s\n", buf);
		}
	}
    
    return length;
}

int main(int argc, char* argv[])
{
	if (parse_argv(argc, argv) == 0) {
		return 0;
	}

	CURL *curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, IPADDR_API);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_synology_ip);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	return 0;
}

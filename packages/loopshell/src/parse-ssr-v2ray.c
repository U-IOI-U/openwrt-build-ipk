#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#define _GNU_SOURCE
#include <errno.h>
#include "include/ssr.h"
#include "include/v2ray.h"

static int ignore_error = 0;
static int parse_show_mode = 0;

int parse_link(const char* link, int mode)
{
	if (link == NULL) {
		return 1;
	}

	int link_length = strlen(link);
	if ((link_length >= 5) && (strncmp(link, "ss://", 5) == 0 || strncmp(link, "SS://", 5) == 0)) {
		return parse_ss(link + 5, mode);
	}

	if ((link_length >= 6) && (strncmp(link, "ssr://", 6) == 0 || strncmp(link, "SSR://", 6) == 0)) {
		return parse_ssr(link + 6, mode);
	}

	if ((link_length >= 8) && (strncmp(link, "vmess://", 8) == 0 || strncmp(link, "VMESS://", 8) == 0)) {
		return parse_v2ray(link + 8, mode);
	}

	return 1;
}

static void parse_ssr_v2ray_help()
{
	printf("parse [OPTIONS...] string\n"
		   "  -h            Show this help\n"
		   "  -i            ignore error\n"
		   "  -m            mode\n"
		   "      0         show (default)\n"
		   "      1         shell eval\n");
}

static int parse_argv(int argc, char *argv[])
{
	static const struct option options[] = {
		{ "help",      no_argument,       NULL, 'h'    },
		{}
	};

	int c;
	while ((c = getopt_long(argc, argv, "im:h", options, NULL)) >= 0) {
		switch (c) {
		case 'h':
			parse_ssr_v2ray_help();
			return 0;
		case 'i':
			ignore_error = 1;
			break;
		case 'm':
			parse_show_mode = atoi(optarg);
			break;
		case '?':
			return 0;
		default:
			;
		}
	}

	if (optind == argc) {
		printf("no string ?\n");
		return 0;
	}

	return 1;
}

int parse_ssr_v2ray_main(int argc, char **argv)
{
	if (parse_argv(argc, argv) == 0) {
		return 0;
	}	

	if (ignore_error) {
		int i;
		for (i = optind; i < argc; i++) {
			parse_link(argv[i], parse_show_mode);
		}
	} else {
		int i, ret;
		for (i = optind; i < argc; i++) {
			if ((ret = parse_link(argv[i], parse_show_mode)) != 0) {
				return ret;
			}
		}		
	}

	return 0;
}

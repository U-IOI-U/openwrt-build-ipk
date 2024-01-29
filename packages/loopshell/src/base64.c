/********************************************
编译：
gcc -g -o base64 base64.c -lssl

测试：
./base64 -e 123qwe
base64_encode("123qwe"): MTIzcXdl
 
./base64 -d MTIzcXdl
base64_decode("MTIzcXdl"): 123qwe
*********************************************/
//base64.c:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "include/base64.h"

typedef enum {
	BASE64_NONE = 0,
	BASE64_ENCODE,
	BASE64_DECODE
} base64_mode;

static base64_mode base64_command = BASE64_NONE;
static int new_line = 0;
static int url_safe = 0;

static char *base64_encode(const unsigned char *in, int len, int with_new_line)
{
	BIO *bmem = NULL, *b64 = NULL;
	BUF_MEM *b_ptr = NULL;

	b64 = BIO_new(BIO_f_base64());
	if (!with_new_line) {
		BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	}

	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, in, len);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &b_ptr);

	char *buff = (char *)calloc(1, b_ptr->length + 1);
	if (buff == NULL) {
		BIO_free_all(b64);
		printf("alloc memory fail\n");
		return NULL;
	}

	memcpy(buff, b_ptr->data, b_ptr->length);
	buff[b_ptr->length] = '\0';
	BIO_free_all(b64);

	return buff;
}

static char *base64_decode(unsigned char *in, int len, int with_new_line)
{
	BIO *b64 = NULL, *bmem = NULL;

	char *buffer = (char *)calloc(1, len);
	if (buffer == NULL) {
		printf("alloc memory fail\n");
		return NULL;
	}

	b64 = BIO_new(BIO_f_base64());
	if (!with_new_line) {
		BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	}

	bmem = BIO_new_mem_buf(in, len);
	bmem = BIO_push(b64, bmem);
	BIO_read(bmem, buffer, len);
	BIO_free_all(bmem);

	return buffer;
}

char* Base64Encode(const unsigned char *in, int len, int with_new_line, int url_safe)
{
	if (in == NULL || len <= 0) {
		return NULL;
	}

	int olen = 0, i = 0;
	char* base64 = NULL;
	if ((base64 = base64_encode(in, len, with_new_line)) == NULL || (olen = strlen(base64)) == 0) {
		if (base64) {
			free(base64);
		}
		return NULL;
	}

	if (base64[olen - 1] == '\n') {
		base64[olen - 1] = '\0';
	}
	if (url_safe) {
		for (i = olen - 1; i >= 0; i--) {
			if (base64[i] == '+') {
				base64[i] = '-';
			} else if (base64[i] == '/') {
				base64[i] = '_';
			} else if (base64[i] == '=') {
				base64[i] = '\0';
			}
		}
	}
	return base64;
}

char* Base64Decode(const unsigned char *in, int len, int with_new_line)
{
	if (in == NULL || len <= 0) {
		return NULL;
	}

	int i;
	int in_len = (in[len - 1] == '\n') ? (len - 1) : len;
	int buf_len = ((in_len % 4) == 0) ? (in_len + 1) : (((int)(in_len / 4) + 1) * 4 + 1);
	char* base64 = NULL, *buf = (char*)calloc(1, buf_len + 1);
	if (buf == NULL) {
		printf("calloc memory fail\n");
		return NULL;
	}

	for (i = 0; i < in_len; i++) {
		if (in[i] == '-') {
			buf[i] = '+';
		} else if (in[i] == '_') {
			buf[i] = '/';
		} else {
			buf[i] = in[i];
		}
	}

	for (i = in_len; i < (buf_len - 1); i++) {
		buf[i] = '=';
	}

	buf[buf_len - 1] = '\n';

	base64 = base64_decode(buf, buf_len, with_new_line);
	free(buf);

	if (base64 != NULL && strlen(base64) == 0) {
		free(base64);
		return NULL;
	}

	return base64;
}

static void base64_help()
{
	printf("base64 [OPTIONS...] string\n"
		   "  -h            Show this help\n"
		   "  -e            Encode\n"
		   "  -d            Decode\n"
		   "  -n            New Line\n"
		   "  -s            Url Safe\n");
}

static int parse_argv(int argc, char *argv[])
{
	static const struct option options[] = {
		{ "help",      no_argument,       NULL, 'h'    },
		{}
	};

	int c;
	while ((c = getopt_long(argc, argv, "ednsh", options, NULL)) >= 0) {
		switch (c) {
		case 'h':
			base64_help();
			return 0;
		case 'e':
			base64_command = BASE64_ENCODE;
			break;
		case 'd':
			base64_command = BASE64_DECODE;
			break;
		case 'n':
			new_line = 1;
			break;
		case 's':
			url_safe = 1;
			break;
		case '?':
			return 0;
		default:
			;
		}
	}

	if (base64_command == BASE64_NONE) {
		printf("decode or encode ?\n");
		return 0;
	}

	if (optind == argc) {
		printf("no string ?\n");
		return 0;
	}

	return 1;
}

int base64_main(int argc, char **argv)
{
	if (parse_argv(argc, argv) == 0) {
		return 0;
	}

	int index;
	if (base64_command == BASE64_ENCODE) {
		char *str = NULL, *out = NULL;
		for (index = optind; index < argc; index++) {
			str = argv[index];
			out = Base64Encode(str, strlen(str), new_line, url_safe);
			if (out == NULL) {
				return 1;
			}
			printf("%s\n", out);
			free(out);
			out = NULL;
		}		
	} else if (base64_command == BASE64_DECODE) {
		char *str = NULL, *out = NULL;
		for (index = optind; index < argc; index++) {
			str = argv[index];
			out = Base64Decode(str, strlen(str), new_line);
			if (out == NULL) {
				return 1;
			}
			printf("%s\n", out);
			free(out);
			out = NULL;
		}
	}

	return 0;
}

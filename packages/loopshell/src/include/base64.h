#ifndef _TOOLS_BASE64_H_
#define _TOOLS_BASE64_H_

#include <stdio.h>
#include <stdlib.h>

char* Base64Encode(const unsigned char *in, int len, int with_new_line, int url_safe);
char* Base64Decode(const unsigned char *in, int len, int with_new_line);

#endif /* _TOOLS_BASE64_H_ */

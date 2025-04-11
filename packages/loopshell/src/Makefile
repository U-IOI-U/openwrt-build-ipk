prefix ?= 

ifneq ($(DESTDIR),)
	prefix = $(DESTDIR)
endif

all: loopshell ddns

%.o:%.c
	$(CC) $(CFLAGS) -I. -I./include -c $< -o $@

loopshell: loopshell.o parse-ssr-v2ray.o ssr.o v2ray.o base64.o
	$(CC) $(LDFLAGS) -lcrypto -ljson-c $^ -o $@
	$(STRIP) $@

ddns: ddns.o
	$(CC) $(LDFLAGS) -lcurl $^ -o $@
	$(STRIP) $@

install:
	install -d -m0755 $(prefix)/usr/bin
	install -m0755 loopshell $(prefix)/usr/bin/
	install -m0755 ddns $(prefix)/usr/bin/

clean :
	rm *.o loopshell ddns


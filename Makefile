CC=clang

PREFIX ?= /usr/local
INSTALL_PATH ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/man

.PHONY: all install uninstall

logo2bmp: logo2bmp.c
	$(CC) logo2bmp.c -o logo2bmp

bmp2logo: bmp2logo.c
	$(CC) bmp2logo.c -o bmp2logo

logo2kernel: logo2kernel.c
	$(CC) logo2kernel.c -o logo2kernel

install: logo2bmp bmp2logo logo2kernel bmp2logo.1 logo2bmp.1 logo2kernel.1
	@install -m 755 logo2bmp $(INSTALL_PATH)
	@install -m 755 bmp2logo $(INSTALL_PATH)
	@install -m 755 logo2kernel $(INSTALL_PATH)
	@mkdir -p $(DESTDIR)$(MANDIR)/man1
	@install -m 644 bmp2logo.1 $(DESTDIR)$(MANDIR)/man1/
	@install -m 644 logo2bmp.1 $(DESTDIR)$(MANDIR)/man1/
	@install -m 644 logo2kernel.1 $(DESTDIR)$(MANDIR)/man1/

uninstall:
	@rm -f $(INSTALL_PATH)/logo2bmp
	@rm -f $(INSTALL_PATH)/bmp2logo
	@rm -f $(INSTALL_PATH)/logo2kernel
	@rm -f $(DESTDIR)$(MANDIR)/man1/bmp2logo.1
	@rm -f $(DESTDIR)$(MANDIR)/man1/logo2bmp.1
	@rm -f $(DESTDIR)$(MANDIR)/man1/logo2kernel.1

clean:
	rm -f logo2bmp bmp2logo logo2kernel

CC=clang
INSTALL_PATH=/usr/local/bin

logo2bmp: logo2bmp.c
	$(CC) logo2bmp.c -o logo2bmp

bmp2logo: bmp2logo.c
	$(CC) bmp2logo.c -o bmp2logo

install: logo2bmp bmp2logo
	install -m 755 logo2bmp $(INSTALL_PATH)
	install -m 755 bmp2logo $(INSTALL_PATH)

clean:
	rm -f logo2bmp bmp2logo

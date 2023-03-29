/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2023 The FreeBSD Foundation
 *
 * This software was developed by Simon Peter
 * based on research by Jesper Schmitz Mouridsen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc,char** argv) {
int width,height;
int f = open("/tmp/t2.bmp",O_RDONLY);
unsigned int filesize,byte;
byte=0;
unsigned int bitmapoffset;
unsigned char * vt_logo_image;
vt_logo_image = (unsigned char*)malloc(257*219*8+3*219);
unsigned char* vt_logo_raw = (unsigned char*)malloc(257*219*8);
read(f,vt_logo_image,54);

lseek(f,0x2,SEEK_SET);
read(f,&filesize,4);
lseek(f,0x12,SEEK_SET);
read(f,&width,4);
lseek(f,0x16,SEEK_SET);
read(f,&height,4);
lseek(f,0x0a,SEEK_SET);
read(f,&bitmapoffset,4);
int bytes,padding, bpl, xi, yi;
int i,offset;
offset=0;
i=0;
int size=0;
bpl = (width + 7) / 8;
unsigned int  usedbytes[7300];
lseek(f,bitmapoffset,SEEK_SET);
read(f,vt_logo_image,filesize-bitmapoffset);
close(f);
bytes = filesize-bitmapoffset;
int out = open("/tmp/t2.raw",O_WRONLY|O_CREAT,0666);
bpl = (width + 7) / 8;
for(yi=0;yi<height;yi++) {
for(xi=0;xi<bpl;xi++) {
i+=write(out,&vt_logo_image[offset],1);
offset++;

}
offset+=(xi % 4 ==0) ? 0 : 4-xi%4;
}
close(out);
int in = open("/tmp/t2.raw",O_RDONLY);
read(in,vt_logo_raw,height*width*8);
for(yi=height-1;yi>=0;yi--) {
for(xi=0;xi<bpl;xi++) {
if(size==0)
	printf("\t");
printf("0x%.2x,",vt_logo_raw[yi*bpl+xi]);
size++;
if(size % 12==0){
	printf("\n\t");
}else if(size<height*bpl) {
printf(" ");
}
}
}
close(in);
}

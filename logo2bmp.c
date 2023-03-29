/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2023 The FreeBSD Foundation
 *
 * This software was developed by Simon Peter
 * and Jesper Schmitz Mouridsen.
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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
unsigned int vt_logo_width;
unsigned int vt_logo_height;
unsigned int vt_logo_depth=1;
unsigned char bmp_header[] =	{
0x42,0x4D,0x52,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00
	};
unsigned char zero[] = {0x0,0x0,0x0,0x0};
unsigned char* logo_data;
char buffer[1024];
int logo_size;
int main(int argc,char** argv) {
FILE *input_file;
char* file_path = (char*)malloc(4096);
int opt = 0;
while((opt = getopt(argc, argv, "s:f:")) != -1)
  {
    switch(opt)
      {
      case 's': {
	printf("size: %s\n", optarg);
	char *size = strtok(optarg,"x");
	sscanf(size,"%ud",&vt_logo_width);
	sscanf(strtok(NULL,"x"),"%ud",&vt_logo_height);
	printf("width: %d \n", vt_logo_width);
	printf("height: %d \n", vt_logo_height);
	break;
      }
      case 'f':
	strcpy(file_path,optarg);
	break;
      case ':':
	printf("option needs a value\n");
	break;
      case '?':
	printf("unknown option: %c\n", optopt);
	break;
      }
  }
if(!vt_logo_width || !vt_logo_height) {
printf("specify size e.g -s 257x219\n");
exit(1);
}
if(!*file_path) {
printf("specify input file (src format,not binary, e.g -f /usr/src/sys/dev/vt/logo/logo_freebsd.c\n"); 
exit(1);

}
// Open the input file
	input_file = fopen(file_path, "r");
	if (!input_file) {
		fprintf(stderr, "Failed to open input file\n");
		return 1;
	}

	// Search for the start of the logo data
	while (fgets(buffer, sizeof(buffer), input_file)) {
		if (strstr(buffer, "unsigned char vt_logo_image[] = {")) {
			break;
		}
	}
	if (feof(input_file)) {
		fprintf(stderr, "Failed to find logo data in file\n");
		fclose(input_file);
		return 1;
	}

	// Convert the logo data to binary data
	logo_data = malloc(1024);
	logo_size = 0;
	while (fgets(buffer, sizeof(buffer), input_file)) {
		char *token = strtok(buffer, ",\n");
		while (token) {
			uint8_t byte = (uint8_t)strtol(token, NULL, 0);
			logo_data[logo_size++] = byte;
			if (logo_size >= 1024) {
				uint8_t *new_data = realloc(logo_data,
				    logo_size + 1024);
				if (!new_data) {
					fprintf(stderr,
					    "Failed to allocate memory for logo data\n");
					fclose(input_file);
					free(logo_data);
					return 1;
				}
				logo_data = new_data;
			}
			token = strtok(NULL, ",\n");
		}
	}

	// Close the input file
	fclose(input_file);


int width,height;
width =vt_logo_width;
height= vt_logo_height;
int f = open("/tmp/t2.bmp",O_WRONLY|O_CREAT,0666);
write(f,&bmp_header,sizeof(bmp_header));
int bytes,padding, bpl, xi, yi;
unsigned int size;
size=0;
bpl = (width + 7) / 8;
bytes = width * height /8;
for(yi=height-1;yi>=0;yi--) {
for(xi=0;xi<bpl;xi++) {
	size++;
	write(f,&logo_data[yi*bpl+xi],1);
	}
	int padding = (xi % 4 == 0) ? 0 : 4-xi % 4;
	size+=padding;
	write(f,zero,padding);
}
size+=sizeof(bmp_header);
lseek(f,2,SEEK_SET);
write(f,&size,4);
lseek(f,0x12,SEEK_SET);
write(f,&width,4);
lseek(f,0x16,SEEK_SET);
write(f,&height,4);
printf("Wrote bmp to /tmp/t2.bmp\n");
close(f);
}

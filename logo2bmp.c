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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "logo_freebsd.c"
#define OUTPUT_FILE "logo.bin"
#define BMP_FILE "logo.bmp"

int
main(void)
{
	FILE *input_file, *output_file;
	char buffer[1024];
	int logo_size;
	uint8_t *logo_data;

	char cmd[256];
	char buf[256];
	int ret;

	// Check if the "convert" command is available
	ret = system("command -v convert >/dev/null");
	if (ret != 0) {
		fprintf(stderr,
		    "Error: The 'convert' command is not available.\n");
		return 1;
	}

	// Open the input file
	input_file = fopen(INPUT_FILE, "r");
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

	// Write the binary data to a file
	output_file = fopen(OUTPUT_FILE, "wb");
	if (!output_file) {
		fprintf(stderr, "Failed to open output file\n");
		free(logo_data);
		return 1;
	}
	fwrite(logo_data, logo_size, 1, output_file);
	fclose(output_file);

	// Clean up
	free(logo_data);

	printf("Logo saved to %s\n", OUTPUT_FILE);

	// Convert the binary data to a bmp file
	snprintf(cmd, sizeof(cmd), "convert -size 257x219 -depth 1 gray:%s %s",
	    OUTPUT_FILE, BMP_FILE);
	ret = system(cmd);
	if (ret != 0) {
		fprintf(stderr,
		    "Error: Failed to execute the 'convert' command.\n");
		return 1;
	}

	printf("Logo saved to %s\n", BMP_FILE);

	return 0;
}
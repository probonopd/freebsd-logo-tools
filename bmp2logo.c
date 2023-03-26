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

#define INPUT_FILE "logo.bin"
#define OUTPUT_FILE "logo_modified.c"
#define BMP_FILE "logo.bmp"
#define ORIGINAL_FILE "logo_freebsd.c"

int
main(void)
{
	FILE *input_file, *output_file;
	uint8_t *logo_data;
	size_t logo_size;

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

	// Convert the BMP image to a raw image with a bit depth
	// of 1
	snprintf(cmd, sizeof(cmd), "convert %s -depth 1 gray:%s", BMP_FILE,
	    INPUT_FILE);
	ret = system(cmd);
	if (ret != 0) {
		fprintf(stderr,
		    "Error: Failed to execute the 'convert' command.\n");
		return 1;
	}

	// Open the input file
	input_file = fopen(INPUT_FILE, "rb");
	if (!input_file) {
		fprintf(stderr, "Failed to open input file\n");
		return 1;
	}

	// Get the size of the input file
	fseek(input_file, 0, SEEK_END);
	logo_size = ftell(input_file);
	fseek(input_file, 0, SEEK_SET);

	// Allocate memory for the logo data
	logo_data = malloc(logo_size);
	if (!logo_data) {
		fprintf(stderr, "Failed to allocate memory for logo data\n");
		fclose(input_file);
		return 1;
	}

	// Read the binary data from the input file
	fread(logo_data, logo_size, 1, input_file);

	// Close the input file
	fclose(input_file);

	// Open the output file
	output_file = fopen(OUTPUT_FILE, "w");
	if (!output_file) {
		fprintf(stderr, "Failed to open output file\n");
		free(logo_data);
		return 1;
	}

	// Write the content of the ORIGINAL_FILE
	// to the output file, up to the line
	// "unsigned char vt_logo_image[] = {"
	FILE *original_file = fopen(ORIGINAL_FILE, "r");
	if (!original_file) {
		fprintf(stderr, "Failed to open original file\n");
		free(logo_data);
		fclose(output_file);
		return 1;
	}
	while (fgets(buf, sizeof(buf), original_file)) {
		if (strstr(buf, "unsigned char vt_logo_image[] = {")) {
			break;
		}
		fprintf(output_file, "%s", buf);
	}
	fclose(original_file);

	// Write the logo data in the original format to the output file
	fprintf(output_file, "unsigned char vt_logo_image[] = {\n\t");
	for (size_t i = 0; i < logo_size; i++) {
		fprintf(output_file, "0x%02x", logo_data[i]);
		if (i < logo_size - 1) {
			fprintf(output_file, ",");
		}
		if ((i + 1) % 12 == 0) {
			fprintf(output_file, "\n\t");
		} else {
			fprintf(output_file, " ");
		}
	}
	// (to remove the last comma)
	fseek(output_file, -1, SEEK_END);
	// Remove the last character (blank)
	ftruncate(fileno(output_file), ftell(output_file));
	fprintf(output_file, ",\n};\n");

	// Close the output file
	fclose(output_file);

	// Clean up
	free(logo_data);

	printf("Logo array saved to %s\n", OUTPUT_FILE);
	return 0;
}
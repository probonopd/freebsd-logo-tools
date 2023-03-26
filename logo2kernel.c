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

int
main()
{
	const char *filename = "/boot/kernel/kernel";
	const char pattern[] =
	    "\x01\x01\x00\x00\xDB\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00";
	const size_t pattern_size = sizeof(pattern) - 1;

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open file: %s\n", filename);
		exit(1);
	}

	// Get file size
	fseek(fp, 0L, SEEK_END);
	long file_size = ftell(fp);
	rewind(fp);

	// Allocate buffer to read file
	uint8_t *file_buf = malloc(file_size);
	if (file_buf == NULL) {
		fprintf(stderr, "Failed to allocate memory for file\n");
		exit(1);
	}

	// Read file into buffer
	size_t bytes_read = fread(file_buf, 1, file_size, fp);
	if (bytes_read != file_size) {
		fprintf(stderr, "Failed to read file into buffer\n");
		exit(1);
	}

	fclose(fp);

	// Search for pattern in file
	long offset = -1;
	for (long i = 0; i <= file_size - pattern_size; i++) {
		if (memcmp(&file_buf[i], pattern, pattern_size) == 0) {
			offset = i;
			break;
		}
	}

	if (offset >= 0) {
		printf("Pattern found at offset: 0x%lx\n", offset);
	} else {
		printf("Pattern not found in file\n");
	}

	// Add the length of the pattern to the offset
	offset += pattern_size;

	printf("Offset at which the logo starts: 0x%lx\n", offset);

	printf("Inserting logo from logo.bin into kernel at offset 0x%lx...\n",
	    offset);

	// Starting at the offset, replace the next bytes with the logo from
	// logo.bin but leave the bytes after the logo alone
	const char *logo_filename = "logo.bin";
	FILE *logo_fp = fopen(logo_filename, "rb");
	if (logo_fp == NULL) {
		fprintf(stderr, "Failed to open file: %s\n", logo_filename);
		exit(1);
	}

	// Get file size
	fseek(logo_fp, 0L, SEEK_END);
	long logo_file_size = ftell(logo_fp);
	rewind(logo_fp);

	// Allocate buffer to read file
	uint8_t *logo_file_buf = malloc(logo_file_size);
	if (logo_file_buf == NULL) {
		fprintf(stderr, "Failed to allocate memory for file\n");
		exit(1);
	}

	// Read file into buffer
	size_t logo_bytes_read = fread(logo_file_buf, 1, logo_file_size,
	    logo_fp);
	if (logo_bytes_read != logo_file_size) {
		fprintf(stderr, "Failed to read file into buffer\n");
		exit(1);
	}

	fclose(logo_fp);

	// Copy the logo into the kernel file
	for (long i = offset; i < offset + logo_file_size; i++) {
		file_buf[i] = logo_file_buf[i - offset];
	}

	// Write the modified kernel to a new file
	const char *new_filename = "kernel.modified";
	FILE *new_fp = fopen(new_filename, "wb");
	if (new_fp == NULL) {
		fprintf(stderr, "Failed to open file: %s\n", new_filename);
		exit(1);
	}

	// Write the modified kernel to the new file
	size_t bytes_written = fwrite(file_buf, 1, file_size, new_fp);
	if (bytes_written != file_size) {
		fprintf(stderr, "Failed to write file\n");
		exit(1);
	}

	// Compare the original kernel with the modified kernel file size
	if (bytes_written != bytes_read) {
		fprintf(stderr,
		    "The original kernel and the modified kernel file size are different\n");
		exit(1);
	}

	fprintf(stderr, "Modified kernel written to: %s\n", new_filename);

	fclose(new_fp);

	free(file_buf);

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "htable.h"
#include "hencode.h"
#define SIZE 4096

int main(int argc, char *argv[]) {
	CharCode *codes;
	int fdin, fdout, uniq_chars, offset, i, j, k;
	int bit_counter = 8;
	unsigned char rbuff[SIZE] = { 0 };
	uint8_t wbuff[SIZE] = { 0 };
	uint32_t mask;
	ssize_t num;
	if (-1 == (fdin = open(argv[1], O_RDONLY))) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}
	if (-1 == (fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
		S_IRUSR | S_IWUSR))) {
		fdout = STDOUT_FILENO;
	}
	codes = get_codes(fdin, &uniq_chars);
	write(fdout, &uniq_chars, sizeof(uint32_t));
	for (i = 0; i < uniq_chars; i++) {
		write(fdout, &codes[i].c, sizeof(uint8_t));
		write(fdout, &codes[i].count, sizeof(uint32_t));
	}
	lseek(fdin, 0, SEEK_SET);
	while ((num = read(fdin, rbuff, SIZE)) > 0) {
		k = 0;
		for (i = 0; i < num; i++) {
			for (j = 0; j < uniq_chars; j++) {
				if (codes[j].c == rbuff[i]) {
					mask = codes[j].code;
					offset = bit_counter - codes[j].digits;
					while (1) {
						if (offset >= 0) {
							wbuff[k] |=
								mask << offset;
							break;
						}
						else {
							wbuff[k++] |=
								mask >>-offset;
							mask &= all_ones(
								-offset);
							offset += 8;
						}
					}
					bit_counter = offset;
					break;
				}
			}
		}
		write(fdout, wbuff, k);
		wbuff[0] = wbuff[k];
		clear_arr(wbuff, SIZE);
	}
	if (uniq_chars > 1)
		write(fdout, wbuff, 1);
	free(codes);
	close(fdin);
	return 0;
}

void clear_arr(uint8_t *arr, int size) {
	int i;
	for (i = 1; i < size; i++) {
		arr[i] = 0;
	}
}

int all_ones(int n) {
	int i;
	int total = 1;
	for (i = 0; i <= n; i++) {
		total *= 2;
	}
	return total - 1;
}

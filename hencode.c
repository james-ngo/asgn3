#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "htable.h"
#define SIZE 4096

int main(int argc, char *argv[]) {
	CharCode *codes;
	int fdin, fdout, uniq_chars, i, j;
	char buff[SIZE];
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
	while ((num = read(fdin, buff, num)) > 0) {
		for (i = 0; i < num; i++) {
			for (j = 0; i < uniq_chars; j++) {
				if (codes[j].c == buff[i]) {
					buff[i] = strtol(codes[i].code,
						NULL, 2);
					break;
				}
			}
		}
		write(fdout, buff, SIZE);
	}
	free(codes);
	close(fdin);
	return 0;
}

char *str_slice(char *src, int start, int end) {
	int i;
	char *new = (char*)malloc(end - start + 2);
	for (i = start; i <= end; i++) {
		new[i] = src[i];
	}
	new[i] = '\0';
	return new;
}

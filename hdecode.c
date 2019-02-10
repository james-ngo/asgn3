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
	int fdin, fdout, uniq_chars, max_idx, i, j, k;
	uint8_t rbuff[SIZE];
	uint8_t wbuff[SIZE];
	uint32_t int32_buff[1];
	ssize_t num;
	Node *node;
	Node *node_arr;
	LinkedList list = { 0 };
	if (-1 == (fdin = open(argv[1], O_RDONLY))) {
		if (!strcmp(argv[1], "-")) {
			fdin = STDIN_FILENO;
		}
		else {
			perror(argv[1]);
			exit(EXIT_FAILURE);
		}
	}
	if (-1 == (fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
		S_IRUSR | S_IWUSR))) {
		fdout = STDOUT_FILENO;
	}
	if (read(fdin, rbuff, 4) <= 0) {
		perror("bummer\n");
		exit(EXIT_FAILURE);
	}
	uniq_chars = rbuff[0];
	node_arr = (Node*)malloc(sizeof(Node) * uniq_chars);
	for (i = 0; i < uniq_chars; i++) {
		read(fdin, rbuff, 1);
		node_arr[i].c = rbuff[0];
		read(fdin, int32_buff, 4);
		node_arr[i].freq = int32_buff[0];
        }
	max_idx = uniq_chars;
	for (i = 0; i < uniq_chars; i++) {
		node = (Node*)malloc(sizeof(Node));
		node->c = node_arr[min(node_arr, max_idx)].c;
		node->freq = node_arr[min(node_arr, max_idx)].freq;
		append(&list, node);
		remove_at(node_arr, min(node_arr, max_idx), max_idx);
		max_idx--;
	}
	to_tree(&list);
	node = list.head;
	while ((num = read(fdin, rbuff, SIZE)) > 0) {
		k = 0;
		for (i = 0; i < num; i++) {
			j = 7;
			while (j >= 0) {
				if ((rbuff[i] >> j--) & 1) {
					node = node->right;
				}
				else {
					node = node->left;
				}
				if (node->c) {
					wbuff[k++] = node->c;
					node = list.head;
				}
			}
		}
		write(fdout, wbuff, k);
	}
	return 0;
}

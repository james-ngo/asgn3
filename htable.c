#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include "htable.h"
#define SIZE 4096

CharCode *get_codes(int fdin, int *uniq_chars) {
	int histogram[256] = { 0 };
	int code, i, j;
	ssize_t num;
	int max_idx;
	Node *node_arr;
	Node *node;
	LinkedList list = { 0 };
	CharCode *codes;
	unsigned char buff[SIZE];
	while ((num = read(fdin, buff, SIZE)) > 0) {
		for (i = 0; i < num; i++) {
			if (!histogram[(int)buff[i]]) {
				(*uniq_chars)++;
			}
			histogram[(int)buff[i]] += 1;
		}
	}
	if (!(*uniq_chars)) {
		return NULL;
	}
	node_arr = (Node*)malloc(*uniq_chars * sizeof(Node));
	j = 0;
	for (i = 0; i < 256; i++) {
		if (histogram[i]) {
			node_arr[j].c = i;
			node_arr[j].freq = histogram[i];
			j++;
		}
	}
	max_idx = (*uniq_chars);
	for (i = 0; i < *uniq_chars; i++) {
		node = (Node*)malloc(sizeof(Node));
		node->c = node_arr[min(node_arr, max_idx)].c;
		node->freq = node_arr[min(node_arr, max_idx)].freq;
		append(&list, node);
		remove_at(node_arr, min(node_arr, max_idx), max_idx);
		max_idx--;
	}
	to_tree(&list);
	i = 0;
	j = 0;
	code = 0;
	codes = (CharCode*)malloc(*uniq_chars * sizeof(CharCode));
	traverse(list.head, codes, code, i, &j);
	sort_codes(codes, *uniq_chars);
	free(node_arr);
	free_all(list.head);
	return codes;
}

void sort_codes(CharCode *codes, int n) {
	int i;
	int j;
	CharCode temp;
	int lowest;
	for (i = 0; i < n - 1; i++) {
		lowest = i;
		for (j = i + 1; j < n; j++) {
			if (codes[j].c < codes[lowest].c) {
				lowest = j;
			}
		}
		temp = codes[lowest];
		codes[lowest] = codes[i];
		codes[i] = temp;
	}
}

void free_all(Node *node) {
	if (node->left != NULL) {
		free_all(node->left);
	}
	if (node->right != NULL) {
		free_all(node->right);
	}
	free(node);
}

void traverse(Node *node, CharCode *codes, int code, int i, int *j) {
	if (!node->left && !node->right) {
		codes[*j].code = code; 
		codes[*j].count = node->freq;
		codes[*j].c = node->c;
		codes[*j].digits = i;
		(*j)++;
		return;
	}
	if (node->left != NULL) {
		traverse(node->left, codes, code << 1, i + 1, j);
	}
	if (node->right != NULL) {
		traverse(node->right, codes, (code << 1) + 1, i + 1, j);
	}
}

void remove_at(Node *node_arr, int i, int n) {
	for (; i < n - 1; i++) {
		node_arr[i] = node_arr[i + 1];
	}
}

int min(Node *node_arr, int n) {
	int i;
	int lo = 0;
	for (i = 1; i < n; i++) {
		if (node_arr[i].freq < node_arr[lo].freq ||
			(node_arr[i].freq == node_arr[lo].freq &&
			node_arr[i].c < node_arr[lo].c)) {
			lo = i;
		}
	}
	return lo;
}

void to_tree(LinkedList *list) {
	if (!list->head) {
		return;
	}
	while (list->head->next) {
		treeify(list);
	}
}

void treeify(LinkedList *list) {
	Node *new_node = (Node*)malloc(sizeof(Node));
	new_node->c = '\0';
	new_node->left = list->head;
	new_node->freq = list->head->freq;
	list->head = list->head->next;
	new_node->right = list->head;
	new_node->freq += list->head->freq;
	list->head = list->head->next;
	insert(list, new_node);
}

void insert(LinkedList *list, Node *node) {
	Node *current_node = list->head;
	if (NULL == current_node) {
		append(list, node);
		return;
	}
	if (node->freq < current_node->freq ||
		(node->freq == current_node->freq &&
		node->c <= current_node->c)) {
		node->next = current_node;
		list->head = node;
		return;
	}
	while (current_node->next && (current_node->next->freq < node->freq ||
		(current_node->next->freq == node->freq &&
		current_node->next->c < node->c))) {
		current_node = current_node->next;
	}
	node->next = current_node->next;
	current_node->next = node;
}

void append(LinkedList *list, Node *node) {
	if (NULL == list->head) {
		list->head = node;
		list->tail = node;
	}
	else {
		list->tail->next = node;
		list->tail = node;
	}
}

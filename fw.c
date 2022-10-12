/* Wesley Tam
 * 10/11/2022
 * CSC-357 Nico
 * Takes in a file and returns the top k most common words in the file */

#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

char *really_long_word(FILE *file) {
	/* Input: file pointer
 	 * Output: char pointer to the next word
 	 * Word: a string of alphabetic characters
 	 * 	delimited by something that isn’t */
	char *word;
	char *temp;
	int buffer, strlength, c;

	if (!file) {
		perror("input");
		exit(EXIT_FAILURE);
        }
	buffer = sizeof(char) * 10;
	word = (char *)malloc(buffer);
	/* Create array that can hold 10 characters */
	if (word == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	strlength = 0;
	while ((c = fgetc(file)) != EOF && isalpha(c)) {
		c = tolower(c);	/* unchanged if cant convert */
		if (strlength + 2 >= buffer) {
			buffer *= 2;
			temp = realloc(word, buffer);
			if (temp != NULL) {
				word = temp;
			} else {
				perror("realloc");
				exit(EXIT_FAILURE);
			}
		}
		word[strlength++] = c;
	}
	if (strlength == 0) {
		if (c == EOF) {
			free(word);
			return NULL;
		}	
	}
	word[strlength++] = '\0';
	return word;
}

int compareEntry(const void *ptr1, const void *ptr2) {
	/* Compares two valid Entry pointers */
	const Entry *entry1, *entry2;
	entry1 = *(const Entry **)ptr1;
	entry2 = *(const Entry **)ptr2;

	if (entry1 == NULL)
		return 1;
	else if (entry2 == NULL)
		return -1;
	if (entry1->value != entry2->value)
		return entry2->value - entry1->value;
	else
		return strcmp(entry2->key, entry1->key);
}

int main(int argc, char **argv) {
	/* Reads a file and returns a sorted list of the n most
 	 * common words in the file. If two words are tied, 
 	 * sort them in lexical order. */
	int n = 10, i = 1, print;
	FILE *file;
	char *word;
	char *end;
	Hashmap *h = (Hashmap *)malloc(sizeof(Hashmap));

	if (argc == 1) {
		file = stdin;
	} else if (!strcmp(argv[i], "-n")) {
		if (argv[++i] == NULL) {
			fprintf(stderr, 
				"usage: %s [-n num] [ file1 [ file 2 ...] ]\n", 
				argv[0]);
			exit(EXIT_FAILURE);
		} else {
			n = strtol(argv[i], &end, 10);
			if (n < 0 || *end) {
				fprintf(stderr, 
					"usage: %s [-n num] [filename] \n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
		}
		i++;
	}

	h = create_hm(h, 2);
	if (argc == 1 || (argc == 3 && strcmp(argv[1], "-n") == 0)) {
		file = stdin;
		while ((word = really_long_word(file)) != NULL) {
			if (in_map(h, word) != 0) {
				h = insert(h, word, get_value(h, word) + 1);
			} else {
				h = insert(h, word, 1);
			}
		}
		fclose(file);
	}
	for (; i < argc; i++) {
		file = fopen(argv[i], "r");
		if (file == NULL) {
			fprintf(stderr, "%s: No such file or directory\n",
				argv[i]);
		} else {
			while ((word = really_long_word(file)) != NULL) {
				if (in_map(h, word) != 0) {
					h = insert(h, word, 
						get_value(h, word) + 1);
				} else {
					h = insert(h, word, 1);
				}
			}
			fclose(file);
		}
	}

	if (in_map(h, "")) {
		h->hm[get_index(h, "")]->value = 0;
		h->num_items -= 1;
	}
	qsort(h->hm, h->size, sizeof(Entry *), compareEntry);
	print = (n >= h->num_items) ? h->num_items : n;
	printf("The top %d words (out of %d) are:\n", n, h->num_items);
	for (i = 0; i < print; i++) {
		printf("%9d %s\n", h->hm[i]->value, h->hm[i]->key);
	}
	destroy_hm(h);
	return 0;
}

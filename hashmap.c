/* Wesley Tam
 * September 10, 2022
 * Professor Nico 
 * Hash map with quadratic probing */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hashmap.h"


Hashmap *create_hm(Hashmap *h, int size) {
	/* Input: integer size
 	 * Output: a Hashmap of that size or the next prime number after */
	if (size <= 1) {
		perror("hashmap size too small");
		exit(EXIT_FAILURE);
	} else if (is_prime(size) == 0)
		size = next_prime(size);
	h->hm = (Entry **)calloc(size, sizeof(Entry));
	h->size = size;
	h->num_items = 0;
	return h;
}

void destroy_hm(Hashmap *h) {
	/* Input: a Hashmap pointer
	 *  Output: no output, free all pointers associated with Hashmap */
	int i;

	if (h != NULL) {
		for (i = 0; i < h->size; i++) {
			if (h->hm[i] != NULL) {
				free(h->hm[i]->key);
			}
			free(h->hm[i]);
		}
		free(h->hm);
	}
	free(h);
}

int is_prime(int n) {
	/* Input: integer n
	 * Output: boolean if n is prime (0 = false, 1 = true) */
	int i;

	if (n <= 1)
		return 0;
	for (i = 2; i < (int)(sqrt(n) + 1); i++) {
		if (n % i == 0)
			return 0;
	}
	return 1;
}

int next_prime(int n) {
	/* Input: integer n
	 * Output: next prime integer after n */
	int nxt_prime, i;

	nxt_prime = n + 1;
	while (i = is_prime(nxt_prime), i == 0)
		nxt_prime += 1;
	return nxt_prime;
}

unsigned long exponent(int base, int power) {
	/* Input: integer base, integer power
 	 * Output: integer base raised to power */
	unsigned long ans;
	int i;

	ans = 1;
	for (i = 0; i < power; i++)
		ans *= base;
	return ans;
}

unsigned long horner_hash(const char *key) {
	/* Hashing algorithm for hashmap */
	int n, i;
	unsigned long sum;

	n = (strlen(key) < 8) ? strlen(key) : 8;
	sum = 0;
	for (i = 0; i < n; i++)
		sum += key[i] * (exponent(31, (n - 1 - i)));
	return sum;
}

double get_load_factor(Hashmap *hm) {
	double load_factor;
	
	load_factor = (double)hm->num_items / (double)hm->size;
	return load_factor;
}

int get_index(Hashmap *h, const char *key) {
        /* Input: a Hashmap pointer and a character pointer
	 * Output: the index of the key in the Hashmap, -1 when not in map */
	int collisions;
	unsigned long hash;

	hash = horner_hash(key) % h->size;
	collisions = 0;
	while (h->hm[hash] != NULL) {
		if (strcmp(h->hm[hash][0].key, key) == 0)
			return hash;
		collisions += 1;
		hash = (horner_hash(key) + exponent(collisions, 2)) % h->size;
	}
	return -1;
}

Hashmap *insert(Hashmap *h, char *key, int value) {
	/* Inserts Entries (a string and int value) into a given hashmap
 	 * using horner hash algorithm and quadratic probing */
	int collisions, index, new_size, i;
	unsigned long hash, rehash;
	Entry *entry, *reentry;
	char *str;
	
	hash = horner_hash(key);
	collisions = 0;
	index = (hash + exponent(collisions, 2)) % h->size;
	while (h->hm[index] != NULL && strcmp(h->hm[index]->key, key) != 0) {
		collisions += 1;
		index = (hash + exponent(collisions, 2)) % h->size;
	}
	if (h->hm[index] == NULL)
		h->num_items += 1;
	/* Entry is a duplicate word */
	else if (h->hm[index] != NULL) {
		free(h->hm[index]->key);
		free(h->hm[index]);
	}
	entry = (Entry *)malloc(sizeof(Entry));
	entry->key = key;
	entry->value = value;
	h->hm[index] = entry;
	
	if (get_load_factor(h) > 0.5) {
		/* Hashmap size increased to next prime number after
 		 * twice the current size, with all Entries rehashed */
		Hashmap *rehash_map = (Hashmap *)malloc(sizeof(Hashmap));
		new_size = next_prime(2 * h->size);
		rehash_map = create_hm(rehash_map, new_size);
		for (i = 0; i < h->size; i++) {
			if (h->hm[i] != NULL) {
				rehash = horner_hash(h->hm[i]->key);
				collisions = 0;
				index = (rehash + 
					exponent(collisions, 2)) % new_size;
				while (rehash_map->hm[index] != NULL) {
					collisions += 1;
					index = (rehash + 
						exponent(collisions, 2)) 
						% new_size;
				}
				str = (char *)malloc((strlen(h->hm[i]->key) + 1)
					* sizeof(char));
				str = strcpy(str, h->hm[i]->key);
				reentry = (Entry *)malloc(sizeof(Entry));
 				reentry->key = str;
				reentry->value = h->hm[i]->value;
				rehash_map->hm[index] = reentry;
			}
		}
		rehash_map->size = new_size;
		rehash_map->num_items = h->num_items;
		/* Now all Entries are rehashed into rehash_map.
 		 * Now free the original hashmap and return the
 		 * rehashed map as the new one */
		destroy_hm(h);
		return rehash_map;
	}
	return h;
}

int in_map(Hashmap *h, const char *key) {
	/* Input: a Hashmap pointer and a character pointer
	 * Output: boolean if string is in hashmap (0 = false, 1 = true) */
	int collisions;
	unsigned long hash;

	hash = horner_hash(key);
	collisions = 0;
	while (h->hm[hash % h->size] != NULL) {
		if (strcmp(h->hm[hash % h->size][0].key, key) == 0)
			return 1;
		collisions += 1;
		hash = (horner_hash(key) + exponent(collisions, 2)) % h->size;
	}
	return 0;
}

int get_value(Hashmap *h, const char *key) {
	/* Input: a Hashmap pointer and a character pointer
 	 * Output: returns the value associated with the key 
 	 *  		or -1 if the key does not exist */
	int collisions;
	unsigned long hash;

	hash = horner_hash(key) % h->size;
	collisions = 0;
	while (h->hm[hash] != NULL) {
		if (strcmp(h->hm[hash][0].key, key) == 0)
			return h->hm[hash][0].value;
		collisions += 1;
		hash = (horner_hash(key) + exponent(collisions, 2)) % h->size;
	}
	return -1;
}

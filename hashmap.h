
typedef struct Entry {
	char *key;
	int value;
} Entry;

typedef struct Hashmap {
	Entry **hm;
	int size;
	int num_items;
} Hashmap;

Hashmap *create_hm(Hashmap *h, int size);
void destroy_hm(Hashmap *);
int is_prime(int n);
int next_prime(int n);
unsigned long exponent(int base, int power);
unsigned long horner_hash(const char *);
double get_load_factor(Hashmap *hm);
Hashmap *insert(Hashmap *h, char *key, int value);
int in_map(Hashmap *h, const char *key);
int get_index(Hashmap *h, const char *key);
int get_value(Hashmap *h, const char *key);

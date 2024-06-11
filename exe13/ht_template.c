#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_RATE 4
#define MEMORY_ALLOCATION_ERROR  (-1)

#define TEST 0

typedef union {
	int int_data;
	char char_data;
	// ... other primitive types used
	void *ptr_data;
} data_union;

typedef struct ht_element {
	struct ht_element *next;
	data_union data;
} ht_element;

typedef void (*DataFp)(data_union);
typedef void (*DataPFp)(data_union*);
typedef int (*CompareDataFp)(data_union, data_union);
typedef size_t (*HashFp)(data_union, size_t);
typedef data_union (*CreateDataFp)(void*);

typedef struct {
	size_t size;
	size_t no_elements;
	ht_element *ht;
	DataFp dump_data;
	CreateDataFp create_data;
	DataFp free_data;
	CompareDataFp compare_data;
	HashFp hash_function;
	DataPFp modify_data;
} hash_table;

void dump_list(const hash_table* p_table, size_t n);

void print_table(hash_table *ht){
	printf("\n\n######## TABLE ########\n\n");
	for (int i = 0; i < ht->size; i++){
		dump_list(ht, i);
		printf("\n");
	}
}

void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr) return ptr;
	exit(MEMORY_ALLOCATION_ERROR);
}

// ---------------------- functions to implement


// initialize table fields
void init_ht(hash_table *p_table, size_t size, DataFp dump_data, CreateDataFp create_data,
		 DataFp free_data, CompareDataFp compare_data, HashFp hash_function, DataPFp modify_data) {
	p_table->size = size;
	p_table->no_elements = 0;
	p_table->ht = (ht_element *)safe_malloc(size*sizeof(ht_element));
	for(int i = 0; i < p_table->size; i++){
		p_table->ht[i].next = NULL;
	}
	p_table->dump_data = dump_data;
	p_table->create_data = create_data;
	p_table->free_data = free_data;
	p_table->compare_data = compare_data;
	p_table->hash_function = hash_function;
	p_table->modify_data = modify_data;

#if TEST
	printf("\n\n######## INIT HT ########\n\n");
	print_table(p_table);
#endif

}

// print elements of the list with hash n
void dump_list(const hash_table* p_table, size_t n) {
	for(ht_element *ptr = (p_table->ht)[n].next; NULL != ptr; ptr = ptr->next){
		p_table->dump_data(ptr->data);
	}
#if TEST
	printf(" -> NULL\n\n");
#endif
}

// Free element pointed by data_union using free_data() function
void free_element(DataFp free_data, ht_element *to_delete) {
	if (NULL == free_data)
		return;
	
	free_data(to_delete->data);
}

// free all elements from the table (and the table itself)
void free_table(hash_table* p_table) {
	ht_element *elem = p_table->ht;
	ht_element *ptr;
	ht_element *toDelete;
	for(int i = 0; i < p_table->size; i++){
		ptr = elem->next;
		while (NULL != ptr)
		{
			free_element(p_table->free_data, ptr);
			toDelete = ptr;
			ptr = ptr->next;
			free(toDelete);
		}
		
		elem++;
	}
	free(p_table->ht);
}

// calculate hash function for integer k
size_t hash_base(int k, size_t size) {
	static const double c = 0.618033988; // (sqrt(5.) – 1) / 2.;
	double tmp = k * c;
	return (size_t)floor((double)size * (tmp - floor(tmp)));
}

void rehash(hash_table *p_table) {
	size_t newSize = 2 * p_table->size;
	ht_element *newTable = (ht_element *)safe_malloc(newSize * sizeof(ht_element));
	for (int i = 0; i < newSize; i++){
		newTable[i].next = NULL;
	}
	ht_element *currentElement;
	ht_element *sentinal;
	size_t newPos;

	for (int i = 0; i < p_table->size; i++){
		sentinal = &p_table->ht[i];
		while (NULL != sentinal->next){
			currentElement = sentinal->next;
			sentinal->next = sentinal->next->next;
			newPos = p_table->hash_function(currentElement->data, newSize) % newSize;
			currentElement->next = newTable[newPos].next;
			newTable[newPos].next = currentElement;
		}
	}

	free(p_table->ht);

	p_table->ht = newTable;
	p_table->size = newSize;
}

// find element; return pointer to previous
ht_element *find_previous(hash_table *p_table, data_union data) {
	size_t pos = p_table->hash_function(data, p_table->size) % p_table->size;
	ht_element *prev = &p_table->ht[pos];
	ht_element *elem = p_table->ht[pos].next;

	while (NULL != elem)
	{
		if (p_table->compare_data(elem->data, data) == 0)
			break;
		prev = elem;
		elem = elem->next;
	}

	return prev;
}

// return pointer to element with given value
ht_element *get_element(hash_table *p_table, data_union *data) {
	return find_previous(p_table, *data)->next;
}

// insert element
void insert_element(hash_table *p_table, data_union *data) {
	ht_element *elem = get_element(p_table, data);
	if (NULL != elem){
#if TEST
	printf("\n\n######## INSERT ########\n\n");
	p_table->dump_data(*data);
	printf("is already in hash table\n\n");
#endif
		if (NULL != p_table->modify_data){
			p_table->modify_data(&elem->data);
		}
		return;
	}
	
	size_t pos = p_table->hash_function(*data, p_table->size) % p_table->size;
	ht_element *newElement = (ht_element *)safe_malloc(sizeof(ht_element));
	newElement->data = *data;
	newElement->next = p_table->ht[pos].next;
	p_table->ht[pos].next = newElement;
	(p_table->no_elements)++;

	if ((p_table->no_elements / p_table->size) > MAX_RATE){
		rehash(p_table);
	}

#if TEST
	printf("\n\n######## INSERT ########\n\n");
	print_table(p_table);
#endif
}

// remove element
void remove_element(hash_table *p_table, data_union data) {
	size_t pos = p_table->hash_function(data, p_table->size) % p_table->size;
	ht_element *ptr = p_table->ht[pos].next;
	ht_element *prev = &p_table->ht[pos];


	while (NULL != ptr && p_table->compare_data(ptr->data, data))
	{
		prev = ptr;
		ptr = ptr->next;
	}

	if (NULL == ptr)
		return;

	free_element(p_table->free_data, ptr);

	prev->next = ptr->next;

	(p_table->no_elements)--;

	free(ptr);

#if TEST
	printf("\n\n######## REMOVE ########\n\n");
	print_table(p_table);
#endif
}

// type-specific definitions

// int element

size_t hash_int(data_union data, size_t size) {
	return hash_base(data.int_data, size);
}

void dump_int(data_union data) {
	printf("%d ", data.int_data);
}

int cmp_int(data_union a, data_union b) {
	if (a.int_data < b.int_data)
		return -1;
	else if (a.int_data > b.int_data)
		return 1;
	return 0;
}

data_union create_int(void* value) {
	data_union data;
    scanf("%i", &data.int_data);

    if(value != NULL)
        ((data_union*)value)->int_data = data.int_data;

    return data;
}

// char element

size_t hash_char(data_union data, size_t size) {
	return hash_base((int)data.char_data, size);
}

void dump_char(data_union data) {
	printf("%c ", data.char_data);
}

int cmp_char(data_union a, data_union b) {
	if (a.char_data < b.char_data)
		return -1;
	else if (a.char_data > b.char_data)
		return 1;
	return 0;
}

data_union create_char(void* value) {
	data_union data;
	scanf(" %c", &data.char_data);

	if (NULL != value){
		((data_union *)value)->char_data = data.char_data;
	}

	return data;
}

// Word element

typedef struct DataWord {
	char *word;
	int counter;
} DataWord;

void dump_word(data_union data) {
	DataWord *dataWord = (DataWord *)data.ptr_data;
	printf("%s %d", dataWord->word, dataWord->counter);
}

void free_word(data_union data) {
	DataWord *toDelete = (DataWord *)data.ptr_data;
	free(toDelete->word);
	free(toDelete);
}

int cmp_word(data_union a, data_union b) {
	DataWord *word1 = (DataWord *)(a.ptr_data);
	DataWord *word2 = (DataWord *)(b.ptr_data);
	return strcmp(word1->word, word2->word);
}

size_t hash_word(data_union data, size_t size) {
	int s = 0;
	DataWord *dw = (DataWord*)data.ptr_data;
	char *p = dw->word;
	while (*p) {
		s += *p++;
	}
	return hash_base(s, size);
}

void modify_word(data_union *data) {
	DataWord *dataWord = (DataWord *)data->ptr_data;
	(dataWord->counter)++;
}

data_union create_data_word(void *value) {
	data_union data;
	DataWord *newWord = (DataWord *)safe_malloc(sizeof(DataWord));

	newWord->word = (void *)strdup((char *)value);
	newWord->counter = 1;

	for(int i = 0; i < strlen(newWord->word); i++){
		newWord->word[i] = (char)tolower((int)newWord->word[i]);
	}

	data.ptr_data = newWord;

	return data;
}

// read text, parse it to words, and insert these words to the hashtable
void stream_to_ht(hash_table *p_table, FILE *stream) {
	const char delimits[] = " \r\t\n.,?!:;-";
	char buffor[BUFFER_SIZE];
	char *token;
	data_union data;


	while (fgets(buffor, sizeof(buffor), stream)){
		token = strtok(buffor, delimits);
		while (NULL != token)
		{
			data = p_table->create_data(token);
			insert_element(p_table, &data);
			token = strtok(NULL, delimits);
		}
	}
}

// test primitive type list
void test_ht(hash_table *p_table, int n) {
	char op;
	data_union data;
	for (int i = 0; i < n; ++i) {
		scanf(" %c", &op);
//		p_table->create_data(&data);
		data = p_table->create_data(NULL); // should give the same result as the line above
		switch (op) {
			case 'r':
				remove_element(p_table, data);
				break;
			case 'i':
				insert_element(p_table, &data);
				break;
			default:
				printf("No such operation: %c\n", op);
				break;
		}
	}
}

int main(void) {
	int to_do, n;
	size_t index;
	hash_table table;
	char buffer[BUFFER_SIZE];
	data_union data;

	scanf ("%d", &to_do);
	switch (to_do) {
		case 1: // test integer hash table
			scanf("%d %zu", &n, &index);
			init_ht(&table, 4, dump_int, create_int, NULL, cmp_int, hash_int, NULL);
			test_ht(&table, n);
			printf ("%zu\n", table.size);
			dump_list(&table, index);
			break;
		case 2: // test char hash table
			scanf("%d %zu", &n, &index);
			init_ht(&table, 4, dump_char, create_char, NULL, cmp_char, hash_char, NULL);
			test_ht(&table, n);
			printf ("%zu\n", table.size);
			dump_list(&table, index);
			break;
		case 3: // read words from text, insert into hash table, and print
			scanf("%s", buffer);
			init_ht(&table, 8, dump_word, create_data_word, free_word, cmp_word, hash_word, modify_word);
			stream_to_ht(&table, stdin);
			printf ("%zu\n", table.size);
			data = table.create_data(buffer);
			ht_element *e = get_element(&table, &data);
			if (e) table.dump_data(e->data);
			if (table.free_data) table.free_data(data);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	free_table(&table);

	return 0;
}


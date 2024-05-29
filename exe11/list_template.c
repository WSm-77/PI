#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

#define TEST 0

struct List;
typedef void (*DataFp)(void*);
typedef void (*ConstDataFp)(const void*);
typedef  int (*CompareDataFp)(const void*, const void*);

typedef struct ListElement {
	struct ListElement *next;
	void *data;
} ListElement;

typedef struct {
	ListElement *head;
	ListElement *tail;
	ConstDataFp dump_data;
	DataFp free_data;
	CompareDataFp compare_data;
	DataFp modify_data;
} List;

void dump_list(const List* p_list);

char to_lower(char c){
	return 'a' <= c && c <= 'z' ? c : c + 32;
}

void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if(ptr) return ptr;
	printf("malloc error\n");
	exit(EXIT_FAILURE);
}

void *safe_strdup(const char *string) {
	void *ptr = strdup(string);
	if(ptr) return ptr;
	printf("strdup error\n");
	exit(EXIT_FAILURE);
}

// --- generic functions --- for any data type

void init_list(List *p_list, ConstDataFp dump_data, DataFp free_data,
		CompareDataFp compare_data, DataFp modify_data) {
	p_list->dump_data = dump_data;
	p_list->free_data = free_data;
	p_list->compare_data = compare_data;
	p_list->modify_data = modify_data;
	ListElement *guardian = (ListElement *)safe_malloc(sizeof(ListElement));
	guardian->data = NULL;
	guardian->next = NULL;
	p_list->head = guardian;
	p_list->tail = guardian;

#if TEST
	printf("\nINIT LIST:\n");
	printf("head: %p, tail: %p\n", p_list->head, p_list->tail);
	dump_list(p_list);
	printf("\n");
#endif
}

// Print elements of the list
void dump_list(const List* p_list) {
	ListElement *ptr = p_list->head->next;

	while (NULL != ptr)
	{
		(p_list->dump_data)(ptr->data);
		ptr = ptr->next;
	}
	printf("\n");
}

// Print elements of the list if comparable to data
void dump_list_if(List *p_list, void *data) {
	ListElement *ptr = p_list->head->next;

	while (NULL != ptr)
	{
		if (0 == (p_list->compare_data)(ptr->data, data)){
			(p_list->dump_data)(ptr->data);
		}

		ptr = ptr->next;
	}
	printf("\n");
}

// Free element pointed by data using free_data() function
void free_element(DataFp free_data, ListElement *to_delete) {
	if (NULL == to_delete->data)
		return;
	free_data(to_delete->data);
}

// Free all elements of the list
void free_list(List* p_list) {
	p_list->tail = NULL;
	ListElement *next_elem;

	while (NULL != p_list->head)
	{
		free_element(p_list->free_data, p_list->head);
		next_elem = p_list->head->next;
		
		// delete list element
		free(p_list->head);

		p_list->head = next_elem;
	}
}

// Push element at the beginning of the list
void push_front(List *p_list, void *data) {
	ListElement *new_element = (ListElement *)safe_malloc(sizeof(ListElement));
	if (p_list->head == p_list->tail){
		p_list->tail = new_element;
	}

	new_element->data = data;
	new_element->next = p_list->head->next;
	p_list->head->next = new_element;


#if TEST
	printf("\nPUSH FRONT:\n");
	printf("head: %p, tail: %p\n", p_list->head, p_list->tail);
	dump_list(p_list);
	printf("\n");
#endif
}

// Push element at the end of the list
void push_back(List *p_list, void *data) {
	p_list->tail->next = (ListElement *)safe_malloc(sizeof(ListElement));
	p_list->tail = p_list->tail->next;
	p_list->tail->data = data;
	p_list->tail->next = NULL;

#if TEST
	printf("\nPUSH BACK:\n");
	printf("head: %p, tail: %p\n", p_list->head, p_list->tail);
	dump_list(p_list);
	printf("\n");
#endif
}

// Remove the first element
void pop_front(List *p_list) {
	if (NULL == p_list->head->next)
		return;

	ListElement *to_delete = p_list->head->next;
	if (to_delete == p_list->tail){
		p_list->tail = p_list->head;
		p_list->head->next = NULL;
	}
	else{
		p_list->head->next = to_delete->next;
	}


	free_element(p_list->free_data, to_delete);
	free(to_delete);
#if TEST
	printf("\nPOP FRONT:\n");
	printf("head: %p, tail: %p\n", p_list->head, p_list->tail);
	dump_list(p_list);
	printf("\n");
#endif
}

// Reverse the list
void reverse(List *p_list) {
	if (NULL == p_list->head->next)
		return;

	ListElement *prev = NULL;
	ListElement *next;
	ListElement *ptr = p_list->head->next;

	while (NULL != ptr)
	{
		next = ptr->next;
		ptr->next = prev;
		prev = ptr;
		ptr = next;
	}

	// set ptr to new-tail/old-head
	ptr = p_list->head->next;
	p_list->head->next = p_list->tail;
	p_list->tail = ptr;

#if TEST
	printf("\nREVERSE:\n");
	printf("head: %p, tail: %p\n", p_list->head, p_list->tail);
	dump_list(p_list);
	printf("\n");
#endif
}

// find element in sorted list after which to insert given element
ListElement* find_insertion_point(const List *p_list, void *data) {
	ListElement *insertion_point = p_list->head;

	while (NULL != insertion_point->next)
	{
		if (-1 == (p_list->compare_data)(data, insertion_point->next->data))
			break;

		insertion_point = insertion_point->next;
	}
	
	return insertion_point;
}

// Insert element after 'previous'
void push_after(List *p_list, void *data, ListElement *previous) {
	if (0 == (p_list->compare_data)(data, previous->data)){
		if (NULL !=p_list->modify_data){
			(p_list->modify_data)(previous->data);
		}
	}
	else{
		ListElement *new_element = (ListElement *)safe_malloc(sizeof(ListElement));
		new_element->data = data;
		new_element->next = previous->next;
		previous->next = new_element;

		if (previous == p_list->tail){
			p_list->tail = new_element;
			p_list->tail->next = NULL;
		}
	}
}

// Insert element preserving order
void insert_in_order(List *p_list, void *p_data) {
	ListElement *prevsious = find_insertion_point(p_list, p_data);
	push_after(p_list, p_data, prevsious);

#if TEST
	printf("\nINSERT IN ORDER:\n");
	printf("head: %p, tail: %p\n", p_list->head, p_list->tail);
	dump_list(p_list);
	printf("\n");
#endif
}

// -----------------------------------------------------------
// --- type-specific definitions

// int element

void dump_int(const void *d) {
	printf("%d ", *(int *)d);
}

void free_int(void *d) {
	free(d);
	d = NULL;
}

int cmp_int(const void *a, const void *b) {
	if (NULL == a || NULL == b)
		return 2;

	int val1 = *(int *)a;
	int val2 = *(int *)b;

	if (val1 < val2)
		return -1;
	else if (val1 > val2)
		return 1;
	
	return 0;
}

int *create_data_int(int v) {
	int *data = (int *)safe_malloc(sizeof(int));
	*data = v;

	return data;
}

// Word element

typedef struct DataWord {
	char *word;
	int counter;
} DataWord;

void dump_word (const void *d) {
	DataWord *word = (DataWord *)d;
	printf("%s ", word->word);
}

void dump_word_lowercase(const void *d) {
	DataWord *data_word = (DataWord *)d;
	char *c = data_word->word;

	while ('\0' != *c)
	{
		printf("%c", to_lower(*c));
		c++;
	}	

	printf(" ");
}

void free_word(void *d) {
	DataWord *word = (DataWord *)d;
	free(word->word);
	word->word = NULL;
}

int cmp_word_alphabet(const void *a, const void *b) {
	if (NULL == a || NULL == b)
		return 2;

	char *c1 = (char *)(((DataWord *)a)->word);
	char *c2 = (char *)(((DataWord *)b)->word);

	while (*c1 == *c2)
	{
		if ('\0' == *c1){
			return 0;
		}

		c1++;
		c2++;
	}

	return (*c1) < (*c2) ? -1 : 1;
}

int cmp_word_alphabet_case_insensitive(const void *a, const void *b) {
	if (NULL == a || NULL == b)
		return 2;

	char *c1 = (char *)(((DataWord *)a)->word);
	char *c2 = (char *)(((DataWord *)b)->word);

	while (to_lower(*c1) == to_lower(*c2))
	{
		if ('\0' == *c1){
			return 0;
		}

		c1++;
		c2++;
	}

	return to_lower(*c1) < to_lower(*c2) ? -1 : 1;
}

int cmp_word_counter(const void *a, const void *b) {
	int cnt1 = (int)(((DataWord *)a)->counter);
	int cnt2 = (int)(((DataWord *)b)->counter);

	if (cnt1 == cnt2)
		return 0;
	else if (cnt1 < cnt2)
		return -1;
	
	return 1;
	
}

void modify_word(void *p) {
	DataWord *ptr = (DataWord *)p;

	(ptr->counter)++;
}

void *create_data_word(const char *string, int counter) {
	DataWord *new_data_word = (DataWord *)safe_malloc(sizeof(DataWord));
	new_data_word->word = (char *)safe_strdup(string);

	

	new_data_word->counter = counter;

	return new_data_word;
}

// read text, parse it to words, and insert those words to the list.
// Order of insertions is given by the last parameter of type CompareDataFp.
// (comparator function address). If this address is not NULL the element is
// inserted according to the comparator. Otherwise, read order is preserved.
void stream_to_list(List *p_list, FILE *stream) {
		const char delimits[] = " \r\t\n.,?!:;-";

	void (*add_to_list_function)(List *, void *);

	add_to_list_function = NULL == p_list->compare_data ? push_back : insert_in_order;

	char buffor[BUFFER_SIZE];
	char *token;
	
	while (fgets(buffor, sizeof(buffor), stream)){
		token = strtok(buffor, delimits);
		while (NULL != token)
		{
			add_to_list_function(p_list, create_data_word(token, 1));
			token = strtok(NULL, delimits);
		}
	}

#if TEST
	printf("\nSTREAM TO LIST:\n");
	printf("head: %p, tail: %p\n", p_list->head, p_list->tail);
	dump_list(p_list);
	printf("\n");
#endif

}

// test integer list
void list_test(List *p_list, int n) {
	char op;
	int v;
	for (int i = 0; i < n; ++i) {
		scanf(" %c", &op);
		switch (op) {
			case 'f':
				scanf("%d", &v);
				push_front(p_list, create_data_int(v));
				break;
			case 'b':
				scanf("%d", &v);
				push_back(p_list, create_data_int(v));
				break;
			case 'd':
				pop_front(p_list);
				break;
			case 'r':
				reverse(p_list);
				break;
			case 'i':
				scanf("%d", &v);
				insert_in_order(p_list, create_data_int(v));
				break;
			default:
				printf("No such operation: %c\n", op);
				break;
		}
	}
}

#if TEST
// test word list
void list_test_word(List *p_list, int n) {
	char op;
	char buffor[BUFFER_SIZE];
	for (int i = 0; i < n; ++i) {
		scanf(" %c", &op);
		switch (op) {
			case 'f':
				scanf("%s", buffor);
				push_front(p_list, create_data_word(buffor,1));
				break;
			case 'b':
				scanf("%s", buffor);
				push_back(p_list, create_data_word(buffor,1));
				break;
			case 'd':
				pop_front(p_list);
				break;
			case 'r':
				reverse(p_list);
				break;
			case 'i':
				scanf("%s", buffor);
				insert_in_order(p_list, create_data_word(buffor,1));
				break;
			default:
				printf("No such operation: %c\n", op);
				break;
		}
	}
}
#endif

int main(void) {
	int to_do, n;
	List list;

	scanf ("%d", &to_do);
	switch (to_do) {
		case 1: // test integer list
			scanf("%d",&n);
			init_list(&list, dump_int, free_int, cmp_int, NULL);
			list_test(&list, n);
			dump_list(&list);
			free_list(&list);
			break;
		case 2: // read words from text, insert into list, and print
			init_list(&list, dump_word, free_word, NULL, NULL);
			stream_to_list(&list, stdin);
			dump_list(&list);
			free_list(&list);
			break;
		case 3: // read words, insert into list alphabetically, print words encountered n times
			scanf("%d",&n);
			init_list(&list, dump_word_lowercase, free_word, cmp_word_alphabet_case_insensitive, modify_word);
			stream_to_list(&list, stdin);
			list.compare_data = cmp_word_counter;
			DataWord data = { NULL, n };
			dump_list_if(&list, &data);
			free_list(&list);
			break;
#if TEST
		case 4:
			scanf("%d",&n);
			init_list(&list, dump_word, free_word, cmp_word_alphabet, NULL);
			list_test_word(&list, n);
			dump_list(&list);
			free_list(&list);
			break;
#endif

		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}


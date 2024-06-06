#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// list node
typedef struct Node {
	int *data;
	size_t size;
	struct Node* next;
	struct Node* prev;
} Node;

// doubly linked list
typedef struct List {
	Node *head;
	Node *tail;
} List;

// iterator
typedef struct iterator {
	struct Node* node_ptr; // pointer to the node
	size_t position; // position of the element within the node
} iterator;

// forward initialization
iterator begin(List* list) {
	iterator it = {list->head, 0 };
	return it;
}

// backward initialization
iterator end(List* list) {
	iterator it = { list->tail, list->tail->size }; // points to the element after the last one
	return it;
}

void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr) return ptr;
	exit(EXIT_FAILURE);
}

void *safe_realloc(void *old_ptr, size_t size) {
	void *ptr = realloc(old_ptr, size);
	if (ptr) return ptr;
	free(old_ptr);
	exit(EXIT_FAILURE);
}

Node *create_node(int *data, size_t size, Node *next, Node *prev) {
	Node *node = safe_malloc(sizeof(Node));
	node->data = data;
	node->size = size;
	node->next = next;
	node->prev = prev;
	return node;
}

// initialize list
void init(List *list) {
	list->head = NULL;
	list->tail = NULL;
}

void free_node(Node **node) {
	if (*node == NULL) return;
	if ((*node)->data != NULL) free((*node)->data);
	free(*node);
	*node = NULL;
}

// to implement ...

// push front
void push_front(List *list, int *data, size_t size) {
	Node *newNode = create_node(data, size, list->head, NULL);
	
	// new element is first element in the list
	if (NULL == list->head){
		list->head = newNode;
		list->tail = newNode;
	}
	else {
		list->head->prev = newNode;
		list->head = newNode;
	}
}

// append element to the list
void push_back(List *list, int *data, size_t size) {
	Node *newNode = create_node(data, size, NULL, list->tail);
	
	// new element is first element in the list
	if (NULL == list->head){
		list->head = newNode;
		list->tail = newNode;
	}
	else {
		list->tail->next = newNode;
		list->tail = newNode;
	}
}

int pop_front(List *list) {
	// list is empty
	if (NULL == list->head){
		return 0;
	}

	Node *toDelete = list->head;
	list->head = list->head->next;

	if (NULL == list->head){
		list->tail = NULL;
	}
	else {
		list->head->prev = NULL;
	}

	free_node(&toDelete);

	return 1;
}

int pop_back(List *list) {
	// list is empty
	if (NULL == list->tail){
		return 0;
	}

	Node *toDelete = list->tail;
	list->tail = list->tail->prev;

	if (NULL == list->tail){
		list->head = NULL;
	}
	else {
		list->tail->next = NULL;
	}

	free_node(&toDelete);

	return 1;
}

// set iterator to move n elements forward from its current position
void skip_forward(iterator* itr, size_t n) {
	while ( itr->position + n >= itr->node_ptr->size)
	{
		n -= (itr->node_ptr->size - itr->position);
		itr->node_ptr = itr->node_ptr->next;
		itr->position = 0;

		if (NULL == itr->node_ptr){
			printf("fatal error, index out of range!!!");
			exit(1);
		}
	}
	itr->position += n;
}

// forward iteration - get n-th element in the list
int get_forward(List *list, size_t n) {
	iterator itr = begin(list);
	skip_forward(&itr, n - 1);

	return (itr.node_ptr->data)[itr.position];
}

// set iterator to move n elements backward from its current position
void skip_backward(iterator* itr, size_t n) {
	while (itr->position < n) {
		n -= itr->position;
		itr->node_ptr = itr->node_ptr->prev;
		itr->position = itr->node_ptr->size;

		if (NULL == itr->node_ptr){
			printf("fatal error, index out of range!!!");
			exit(1);
		}
	}

	itr->position -= n;
}

// backward iteration - get n-th element from the end of the list
int get_backward(List *list, size_t n) {
	iterator itr = end(list);
	skip_backward(&itr, n);

	return (itr.node_ptr->data)[itr.position];
}

void remove_node(List *list, Node **toDelete) {
	if (list->head == *toDelete && list->tail == *toDelete){
		list->head = NULL;
		list->tail = NULL;
	}
	else if (list->head == *toDelete) {
		list->head = list->head->next;
		list->head->prev = NULL;
	}
	else if (list->tail == *toDelete) {
		list->tail = list->tail->prev;
		list->tail->next = NULL;
	}
	else {
		(*toDelete)->prev->next = (*toDelete)->next;
		(*toDelete)->next->prev = (*toDelete)->prev;
	}

	free_node(toDelete);
}

// remove n-th element; if array empty remove node
void remove_at(List *list, size_t n) {
	iterator itr = begin(list);
	skip_forward(&itr, n - 1);
	
	(itr.node_ptr->size)--;
	
	if (0 == itr.node_ptr->size){
		remove_node(list, &itr.node_ptr);
		return;
	}

	for (int i = itr.position; i < itr.node_ptr->size; i++){
		itr.node_ptr->data[i] = itr.node_ptr->data[i + 1];
	}

	itr.node_ptr->data = safe_realloc(itr.node_ptr->data, (itr.node_ptr->size)*sizeof(int));
}

size_t digits(int n) {
	if (0 == n)
		return 1;
	else if (INT_MIN == n)
		n++;

	n = abs(n);
	size_t digs = 0;

	while ( n > 0 ){
		digs++;
		n /= 10;
	}

	return digs;
}

Node *insert_before(List *list, int value){
	Node *ptr = list->head;

	size_t digs = digits(value);

	while (NULL != ptr)
	{
		if (digits(ptr->data[0]) > digs)
			break;
		
		ptr = ptr->next;
	}

	return ptr;
}

void add_value_to_node(Node *node, int value){
		size_t idx = 0;

		while (idx < node->size && node->data[idx] < value)
		{
			idx++;
		}
		
		size_t newSize = node->size + 1;
		int *newData = (int *)safe_malloc(sizeof(int) * newSize);
		memcpy(newData, node->data, sizeof(int) * (idx));
		newData[idx] = value;
		memcpy(newData + idx + 1, node->data + idx, sizeof(int) * (node->size - idx));

		free(node->data);

		node->data = newData;
		(node->size)++;
}

void insert_in_order(List *list, int value) {
	Node *ptr = insert_before(list, value);
	size_t digs = digits(value);

	if (NULL == ptr) {
		if (list->tail != NULL && digits(list->tail->data[0]) == digs){
			add_value_to_node(list->tail, value);
		}
		else {
			int *data = (int *)safe_malloc(sizeof(int) * 1);
			data[0] = value;
			push_back(list, data, 1);
		}
	}
	// if ptr is head of the list
	else if (NULL == ptr->prev){
		int *data = (int *)safe_malloc(sizeof(int) * 1);
		data[0] = value;
		push_front(list, data, 1);
	}
	// if doesn't exist node that contains numbers with "digs" digits
	else if (digits(ptr->prev->data[0]) != digs){
		int *data = (int *)safe_malloc(sizeof(int) * 1);
		data[0] = value;
		Node *newNode = create_node(data, 1, ptr, ptr->prev);

		newNode->next->prev = newNode;
		newNode->prev->next = newNode;
	}
	else {
		add_value_to_node(ptr->prev, value);
	}
}

// -------------------------------------------------------------
// helper functions

// print list
void dumpList(const List *list) {
	for(Node *node = list->head; node != NULL; node = node->next) {
		printf("-> ");
		for (int k = 0; k < node->size; k++) {
			printf("%d ", node->data[k]);
		}
		printf("\n");
	}
}

// free list
void freeList(List *list) {
	while(pop_front(list));
}

// read int vector
void read_vector(int array[], size_t n) {
	for (size_t i = 0; i < n; ++i) {
		scanf("%d", array + i);
	}
}

// initialize the list and push data
void read_list(List *list) {
	int n;
	size_t size;
	scanf("%d", &n);
	for (int i = 0; i < n; i++) {
		scanf("%zu", &size);
		int *array = (int*) safe_malloc(size * sizeof(int));
		read_vector(array, size);
		push_back(list, array, size);
	}
}

int main(void) {
	int to_do, value;
	size_t size, offset;
	List list;
	init(&list);

	scanf("%d", &to_do);
	switch (to_do) {
		case 1:
			read_list(&list);
			dumpList(&list);
			break;
		case 2:
			read_list(&list);
			scanf("%zu", &size);
			for (int i = 0; i < size; i++) {
				scanf("%zu", &offset);
				printf("%d ", get_forward(&list, offset));
			}
			printf("\n");
			break;
		case 3:
			read_list(&list);
			scanf("%zu", &size);
			for (int i = 0; i < size; i++) {
				scanf("%zu", &offset);
				printf("%d ", get_backward(&list, offset));
			}
			printf("\n");
			break;
		case 4:
			read_list(&list);
			scanf("%zu", &size);
			for (int i = 0; i < size; i++) {
				scanf("%zu", &offset);
				remove_at(&list, offset);
			}
			dumpList(&list);
			break;
		case 5:
			scanf("%zu", &size);
			for (int i = 0; i < size; i++) {
				scanf("%d", &value);
				insert_in_order(&list, value);
			}
			dumpList(&list);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	freeList(&list);

	return 0;
}


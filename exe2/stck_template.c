#include <stdio.h>

#define STACK_SIZE 10

enum state { OK = 0, UNDERFLOW = -1, OVERFLOW = -2 };

int stack[STACK_SIZE];
int top = 0;

void queue_print(void);

int stack_top_value()
{
	if (top == 0){
		return 0;
	}
	
	return stack[top - 1];
}

int stack_push(int x) {
	if( top >= STACK_SIZE){
		return OVERFLOW;
	}

	stack[top] = x;
	top++;

	return OK;
}

int stack_pop(void) {
	if (top == 0){
		return UNDERFLOW;
	}

	int topValue = stack[top - 1];
	stack[top - 1] = 0;
	top--;

	return topValue;
}

int stack_state(void) {
	return top;
}

// FIFO queue with shifts

#define QUEUE_SIZE 10

int queue[QUEUE_SIZE];
int in = 0, curr_nr = 0;

int out_of_queue(){
	if (in == 0){
		return 0;
	}

	int remaining = curr_nr - queue[in - 1];
	return remaining;
}

int queue_push(int in_nr) { // in_nr clients try to enter the queue
	int result = OK;
	int queueIndex = in;
	int currentClientNumber = curr_nr + 1;
	curr_nr += in_nr;
	while (in_nr > 0){
		if (queueIndex >= QUEUE_SIZE){
			result = OVERFLOW;
			break;
		}
		queue[queueIndex] = currentClientNumber;
		currentClientNumber++;
		queueIndex++;
		in_nr--;
	}
	in = queueIndex;
	// queue_print();
	return result;
}

int queue_pop(int out_nr) { // out_nr clients leaves the queue
	if(in - out_nr < 0 || out_nr > QUEUE_SIZE){
		in = 0;
		return UNDERFLOW;
	}
	
	// printf("in1: %d\n", in);

	int prevIndex = out_nr;
	int i = 0;
	in -= out_nr;

	while (prevIndex < QUEUE_SIZE){
		queue[i] = queue[prevIndex];
		// in++;
		i++;
		prevIndex++;
	}
	// printf("in2: %d\n", in);

	return in;
}

int queue_state(void) {
	return in;
}

void queue_print(void) {
	int i = 0;
	while (i < in){
		printf("%d ", queue[i]);
		i++;
	}
	printf("\n");
}

// Queue with cyclic buffer

#define CBUFF_SIZE 10

int cbuff[CBUFF_SIZE];
int out = 0, len = 0;


int cbuff_push(int cli_nr) { // client with number cli_nr enters the queue
	if(len == CBUFF_SIZE){
		return OVERFLOW;
	}

	int newElementIndex = (out + len) % CBUFF_SIZE;
	len++;
	cbuff[newElementIndex] = cli_nr; 

	return OK;
}

int cbuff_pop(void) { // longest waiting client leaves the queue
	if(len == 0){
		return UNDERFLOW;
	}

	int currentClient = cbuff[out];
	len--;
	out = (out + 1) % CBUFF_SIZE;
	
	return currentClient;
}

int cbuff_state(void) {
		return len;
}

void cbuff_print(void) {
	int index = out;
	// printf("cubuff: ");
	for (int i = 0; i < len; i++){
		printf("%d ", cbuff[index]);
		index = (index + 1) % CBUFF_SIZE;
	}
	printf("\n");
}

int main(void) {
	int to_do, n, client_no, answer;
	scanf("%d", &to_do);
	switch(to_do) {
		case 1: // stack
			do {
				scanf("%d", &n);
				if (n > 0) {
					if ((answer = stack_push(n)) < 0) printf("%d ", answer);
				} else if (n < 0) {
					printf("%d ", stack_pop());
				} else printf("\n%d\n", stack_state());
			} while(n != 0);
			break;
		case 2: // FIFO queue with shifts
			do {
				scanf("%d", &n);
				if (n > 0) {
					if ((answer = queue_push(n)) < 0) printf("%d ", answer);
					// if ((answer = queue_push(n)) < 0) printf("push: %d\n", answer);
				} else if (n < 0) {
					if ((answer = queue_pop(-n)) < 0) printf("%d ", answer);
				} else {
					printf("\n%d\n", queue_state());
					queue_print();
				}
			} while(n != 0);
			break;
		case 3: // queue with cyclic buffer
			client_no = 0;
			do {
				scanf("%d", &n);
				if (n > 0) {
					if ((answer = cbuff_push(++client_no)) < 0) printf("%d ", answer);
				} else if (n < 0) {
					printf("%d ", cbuff_pop());
				} else {
					printf("\n%d\n", cbuff_state());
					cbuff_print();
				}
			} while(n != 0);
			break;
		default: 
			printf("NOTHING TO DO!\n");
	}
	return 0;
}

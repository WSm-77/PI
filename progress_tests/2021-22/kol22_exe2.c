#include <stdio.h>
#include <stdlib.h>
#include <float.h>

typedef struct {
    double *results;
    int len;
} Data;

typedef struct tagQueueElement {
    Data data;
    struct tagQueueElement *next;
} QueueElement;

typedef struct {
    QueueElement *head;
    QueueElement *tail;
} Queue;

void free_queue(Queue *pqueue) {
    QueueElement *current = pqueue->head;
    while (current != NULL) {
        QueueElement *temp = current;
        current = current->next;
        free(temp);
    }
    pqueue->head = NULL;
    pqueue->tail = NULL;
}

void push_copy(Queue *pqueue, const Data *pdata) {
    QueueElement *new_element = (QueueElement *)malloc(sizeof(QueueElement));
    new_element->data = *pdata;
    new_element->next = NULL;
    if (pqueue->tail != NULL) {
        pqueue->tail->next = new_element;
    } else {
        pqueue->head = new_element;
    }
    pqueue->tail = new_element;
}

int peek(const Queue *pqueue, Data *pdata) {
    if (pqueue->head == NULL) {
        return 0;
    }
    *pdata = pqueue->head->data;
    return 1;
}

int pop(Queue *pqueue, Data *pdata) {
    if (pqueue->head == NULL) {
        return 0;
    }
    QueueElement *temp = pqueue->head;
    *pdata = temp->data;
    pqueue->head = temp->next;
    if (pqueue->head == NULL) {
        pqueue->tail = NULL;
    }
    free(temp);
    return 1;
}

int get_total_count(const Queue *pqueue) {
    int total_count = 0;
    QueueElement *current = pqueue->head;
    while (current != NULL) {
        total_count += current->data.len;
        current = current->next;
    }
    return total_count;
}

// Helper function to print the queue contents
void print_queue(const Queue *pqueue) {
    QueueElement *current = pqueue->head;
    printf("Queue contents:\n");
    while (current != NULL) {
        printf("Data len: %d, Results: ", current->data.len);
        for (int i = 0; i < current->data.len; ++i) {
            printf("%f ", current->data.results[i]);
        }
        printf("\n");
        current = current->next;
    }
    printf("Total count: %d\n", get_total_count(pqueue));
}

int main() {
    // Initialize queue
    Queue queue = {NULL, NULL};

    // Create some sample data
    double results1[] = {1.1, 2.2, 3.3};
    Data data1 = {results1, 3};

    double results2[] = {4.4, 5.5};
    Data data2 = {results2, 2};

    double results3[] = {6.6};
    Data data3 = {results3, 1};

    // Push data to the queue
    push_copy(&queue, &data1);
    push_copy(&queue, &data2);
    push_copy(&queue, &data3);

    // Print the queue contents
    print_queue(&queue);

    // Peek at the first element
    Data peek_data;
    if (peek(&queue, &peek_data)) {
        printf("Peeked data len: %d, Results: ", peek_data.len);
        for (int i = 0; i < peek_data.len; ++i) {
            printf("%f ", peek_data.results[i]);
        }
        printf("\n");
    } else {
        printf("Queue is empty, cannot peek.\n");
    }

    // Pop an element from the queue
    Data pop_data;
    if (pop(&queue, &pop_data)) {
        printf("Popped data len: %d, Results: ", pop_data.len);
        for (int i = 0; i < pop_data.len; ++i) {
            printf("%f ", pop_data.results[i]);
        }
        printf("\n");
    } else {
        printf("Queue is empty, cannot pop.\n");
    }

    // Print the queue contents after pop
    print_queue(&queue);

    // Free the queue
    free_queue(&queue);

    // Verify the queue is empty
    print_queue(&queue);

    return 0;
}
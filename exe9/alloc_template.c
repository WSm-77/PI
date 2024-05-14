#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define TAB_SIZE  1000
#define BUF_SIZE  1000

#define TEST 0

// 1
////////////////////////////////////////////////////////////

int get(int cols, int row, int col, const int* A) {
	return A[row*cols + col];
}

void set(int cols, int row, int col, int* A, int value) {
	A[row*cols + col] = value;
}

// matrix AB size is rowsA x colsB
void prod_mat(int rowsA, int colsA, int colsB, int* A, int* B, int* AB) {
	int sum;
	for (int R = 0; R < rowsA; R++){
		for (int C = 0; C < colsB; C++){
			sum = 0;
			for (int i = 0; i < colsA; i++){
				sum += get(colsA, R, i, A) * get(colsB, i, C, B);
			}
			set(colsB, R, C, AB, sum);
		}
	}
}

void read_mat(int rows, int cols, int* matrix) {
	for (int R = 0; R < rows; R++){
		for (int C = 0; C < cols; C++){
			scanf("%d", matrix);
			matrix++;
		}
	}
}

void print_mat(int rows, int cols, int* matrix) {
	for (int R = 0; R < rows; R++){
		for (int C = 0; C < cols; C++){
			printf("%d ", *matrix);
			matrix++;
		}
		printf("\n");
	}
}

// 2
/////////////////////////////////////////////////////////////

int char_to_int(char ch){
	return ch - '0';
}

int read_ints_from_line(int *chPtr, int *intBuf) {
#if TEST
		printf("read_ints_from_line (start)\n\n");
#endif

	int number = 0;
	int numbersCnt = 0;
	int correction = 1;
	int ch = fgetc(stdin);
	while (EOF != ch && '\n' != ch){
		if ('-' == ch){
			correction = -1;
		}
		else if (' ' == ch){
			*intBuf = number * correction;
			intBuf++;
			numbersCnt++;
			number = 0;
			correction = 1;
		}
		else {
			number *= 10;
			number += char_to_int(ch);
		}

		ch = fgetc(stdin);
	}

	*intBuf = number * correction;

#if TEST
	printf("read_ints_from_line (end)\n\n");
#endif

	*chPtr = ch;

#if TEST
	if (EOF == ch)
		printf("EOF\n");
#endif

	return numbersCnt + 1;
}

int read_int_lines_cont(int* ptrArray[]) {

#if TEST
	printf("read_int_lines_cont (start)\n\n");
#endif

	int linesCnt = 0;
	int ch;
	int *nextPtrInPtrArray = ptrArray[0];
	do {
		nextPtrInPtrArray += read_ints_from_line(&ch, ptrArray[linesCnt]);
		linesCnt++;
		ptrArray[linesCnt] = nextPtrInPtrArray;
	} while(ch != EOF);

#if TEST
		printf("\nread_int_lines_cont (end)\n\n");
#endif

	return linesCnt;
}

void write_int_line_cont(int *ptrArray[], int n) {
	int *ptr = ptrArray[n];
	while (ptr != ptrArray[n + 1]){
		printf("%d ", *ptr);
		ptr++;
	}
	printf("\n");
}

// 3
///////////////////////////////////////////////////////////

int read_char_lines(char *array[]) {
	char buffor[BUF_SIZE];
	char *line;
	int lineLen;
	int linesCnt = 0;
	while(!feof(stdin)){
		fgets(buffor, BUF_SIZE, stdin);
		lineLen = strlen(buffor) + 1;	// we have to add 1, because of '\0' char
		line = (char *)malloc(lineLen * sizeof(char));
		memcpy(line, buffor, lineLen * sizeof(char));
		array[linesCnt] = line;
		linesCnt++;
	}

	array[linesCnt] = NULL;

	return linesCnt;
}

void write_char_line(char *array[], int n) {
	printf("%s", array[n]);
}

void delete_lines(char *array[]) {
	char *ptr = array[0];
	int i = 0;
	while (NULL != ptr){
		free(ptr);
		i++;
		ptr = array[i];
	}
}

// 4
////////////////////////////////////////////////

typedef struct {
	int *values;
	int len;
	double average;
} line_type;

int sum_function(int array[], int howManyNumbersToSum){
	int sum = 0;
	for (int i = 0; i < howManyNumbersToSum; i++){
		sum += array[i];
	}
	return sum;
}

int read_int_lines(line_type linesArray[]) {
	int values[TAB_SIZE];
	int numbersInLineCnt;
	int linesCnt = 0;
	int ch;
	int *valuesPtr;
	int sum;
	while (1) {
		numbersInLineCnt = read_ints_from_line(&ch, values);

		if (EOF == ch){
			break;
		}

#if TEST
		for (int i = 0; i < numbersInLineCnt; i++)
			printf("%d ", values[i]);
		printf("\n");
#endif

		sum = sum_function(values, numbersInLineCnt);
		valuesPtr = (int *)malloc(numbersInLineCnt * sizeof(int));
		memcpy(valuesPtr, values, numbersInLineCnt * sizeof(int));

#if TEST
		printf("\nvaluesPtr:\n\n");
		for (int i = 0; i < numbersInLineCnt; i++)
			printf("%d ", valuesPtr[i]);
		printf("\n");
#endif

		linesArray[linesCnt] = (line_type){valuesPtr, numbersInLineCnt, (double)(sum / numbersInLineCnt)};
		linesCnt++;
	}

	return linesCnt;
}

void write_int_line(line_type linesArray[], int n) {
	for(int i = 0; i < linesArray[n].len; i++){
		printf("%d ", linesArray[n].values[i]);
	}
	printf("\n%.2f\n", linesArray[n].average);
}

void delete_int_lines(line_type linesArray[], int lineCount) {
	for (int i = 0; i < lineCount; i++){
		free(linesArray[i].values);
	}
}

int cmp (const void *a, const void *b) {
	double avarage1 = ((line_type *)a)->average;
	double avarage2 = ((line_type *)b)->average;

	if (avarage1 < avarage2)
		return -1;
	else if (avarage1 > avarage2)
		return 1;
	else
		return 0;
}

void sort_by_average(line_type linesArray[], int lineCount) {
	qsort(linesArray, lineCount, sizeof(line_type), cmp);
}

// 5
///////////////////////////////////////////////////////////

typedef struct {
	int r, c, v;
} triplet;

int read_sparse(triplet* tripletArray, int nTriplets) {
	for (int i = 0; i < nTriplets; i++){
		scanf("%d %d %d", &tripletArray[i].r, &tripletArray[i].c, &tripletArray[i].v);
	}
	return nTriplets;
}

int cmp_triplets(const void* t1, const void* t2) {
	int r1 = ((triplet *)t1)->r;
	int r2 = ((triplet *)t2)->r;

	if (r1 < r2)
		return -1;
	else if (r1 > r2)
		return 1;

	int c1 = ((triplet *)t1)->c;
	int c2 = ((triplet *)t2)->c;

	return c1 < c2 ? -1 : 1;

}

void make_CSR(triplet* tripletArray, int nTriplets, int rows, int* V, int* C, int* R) {
	qsort(tripletArray, nTriplets, sizeof(triplet), cmp_triplets);

#if TEST
	printf("\nTripletsArray:\n");
	for (int i = 0; i < nTriplets; i++){
		printf("%d %d %d\n", tripletArray[i].v, tripletArray[i].r, tripletArray[i].c);
	}
	printf("\n");
#endif

	int currentTriplet = 0;
	int prevNumOfTriplets = 0;
	for(int currentRow = 0; currentRow <= rows; currentRow++){
		while (currentTriplet < nTriplets && currentRow == tripletArray[currentTriplet].r){
#if TEST
	printf("Current triplet: val - %d, row - %d, coll - %d\n",
	       tripletArray[currentTriplet].v, tripletArray[currentTriplet].r, tripletArray[currentTriplet].c);
#endif
			V[currentTriplet] = tripletArray[currentTriplet].v;
			C[currentTriplet] = tripletArray[currentTriplet].c;
			currentTriplet++;
		}

#if TEST
	printf("\nCurrent row %d:\n", currentRow);
#endif
		R[currentRow] = prevNumOfTriplets;
		prevNumOfTriplets = currentTriplet;
#if TEST
	printf("\ntriplet values after updating R vector:\n");
	for (int i = 0; i <= currentRow; i++){
		printf("Current triplet: val - %d, coll - %d, row - %d\n",
		tripletArray[i].v, tripletArray[i].c, R[i]);
	}
	printf("\n");
#endif
	}
}

void multiply_by_vector(int rows, const int* V, const int* C, const int* R,
						const int* x, int* y) {
	int sum;
	for (int i = 0; i < rows; i++){
		sum = 0;
		for (int j = R[i]; j < R[i+1]; j++){
			sum += V[j]*x[C[j]];
		}
		y[i] = sum;
	}
}

void read_vector(int* v, int n) {
	for (int i = 0; i < n; i++){
		scanf("%d", v);
		v++;
	}
}

void write_vector(int* v, int n) {
	for (int i = 0; i < n; i++){
		printf("%d ", v[i]);
	}
	printf("\n");
}

// auxiliary
////////////////////////////////////////////////////////////

int read_int(void) {
	char c_buf[BUF_SIZE];
	fgets(c_buf, BUF_SIZE, stdin);
	return (int)strtol(c_buf, NULL, 10);
}

int main(void) {
	int to_do = read_int();

	int A[TAB_SIZE], B[TAB_SIZE], AB[TAB_SIZE];
	int n, lines_counter, rowsA, colsA, rowsB, colsB;
	int rows, cols, n_triplets;
	char* char_lines_array[TAB_SIZE] = { NULL };
	int continuous_array[TAB_SIZE];
	int* ptr_array[TAB_SIZE];
	triplet triplet_array[TAB_SIZE];
	int V[TAB_SIZE], C[TAB_SIZE], R[TAB_SIZE];
	int x[TAB_SIZE], y[TAB_SIZE];
	line_type int_lines_array[TAB_SIZE];

	switch (to_do) {
		case 1:
			scanf("%d %d", &rowsA, &colsA);
			read_mat(rowsA, colsA, A);
			scanf("%d %d", &rowsB, &colsB);
			read_mat(rowsB, colsB, B);
			prod_mat(rowsA, colsA, colsB, A, B, AB);
			print_mat(rowsA, colsB, AB);
			break;
		case 2:
			n = read_int() - 1; // we count from 1 :)
			ptr_array[0] = continuous_array;
			read_int_lines_cont(ptr_array);
			write_int_line_cont(ptr_array, n);
			break;
		case 3:
			n = read_int() - 1;
			read_char_lines(char_lines_array);
			write_char_line(char_lines_array, n);
			delete_lines(char_lines_array);
			break;
		case 4:
			n = read_int() - 1;
			lines_counter = read_int_lines(int_lines_array);
			sort_by_average(int_lines_array, lines_counter);
			write_int_line(int_lines_array, n);
			delete_int_lines(int_lines_array, lines_counter);
			break;
		case 5:
			scanf("%d %d %d", &rows, &cols, &n_triplets);
			n_triplets = read_sparse(triplet_array, n_triplets);
			read_vector(x, cols);
			make_CSR(triplet_array, n_triplets, rows, V, C, R);
			multiply_by_vector(rows, V, C, R, x, y);
			write_vector(V, n_triplets);
			write_vector(C, n_triplets);
			write_vector(R, rows + 1);
			write_vector(y, rows);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}


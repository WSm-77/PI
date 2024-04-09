#include <stdio.h>
#include <math.h>

#define SIZE 40
#define true 1
#define false 0

#define TEST 0

typedef unsigned short my_bool;

void read_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		scanf("%lf", x++);
	}
}

void print_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		printf("%.4f ", x[i]);
	}
	printf("\n");
}

void read_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			scanf("%lf", &A[i][j]);
		}
	}
}

void print_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			printf("%0.4f ", A[i][j]);
		}
		printf("\n");
	}
}

// 1. Calculate matrix product, AB = A X B
// A[m][p], B[p][n], AB[m][n]
void mat_product(double A[][SIZE], double B[][SIZE], double AB[][SIZE], int m, int p, int n) {
	for (int row=0; row < m; row++){
		for (int coll=0; coll < n; coll++){
			AB[row][coll] = 0.0;
			for(int i = 0; i < p; i++){
				AB[row][coll] += A[row][i] * B[i][coll];
			}
		}
	}
}


// 2. Matrix triangulation and determinant calculation - simplified version
// (no rows' swaps). If A[i][i] == 0, function returns NAN.
// Function may change A matrix elements.
double gauss_simplified(double A[][SIZE], int n) {
	double determinant = 1.0;

	for(int rowAndColl = 0; rowAndColl < n; rowAndColl++){

		double scalar = A[rowAndColl][rowAndColl];

		if (0.0 == scalar){
			return NAN;
		}

		determinant *= scalar;

		for (int i = rowAndColl; i < n; i++){
			A[rowAndColl][i] /= scalar;
		}

		for (int row = rowAndColl + 1; row < n; row++){
			scalar = A[row][rowAndColl];
			for (int coll = rowAndColl; coll < n; coll++){
				A[row][coll] -= A[rowAndColl][coll] * scalar;
			}
		}
		
	}

	#if TEST
		printf("\n");
		print_mat(A, n, n);
	#endif

	return determinant;
}

void backward_substitution_index(double A[][SIZE], const int indices[], double x[], int n) {
}

double double_abs(double val){
	return val < 0.0 ? -val : val;
}

my_bool swap_max_row(double A[][SIZE], int rowsOrder[], int n, int rowAndColl){
	// finding max row
	double maxi = double_abs(A[rowsOrder[rowAndColl]][rowAndColl]);
	int maxRowIndex = rowAndColl;
	for (int i = rowAndColl; i < n; i++){
		double currentAbsVal = double_abs(A[rowsOrder[i]][rowAndColl]);
		if (currentAbsVal > maxi){
			maxi = currentAbsVal;
			maxRowIndex = i;
		}
	}

	if (maxRowIndex != rowAndColl){
		int tmp = rowsOrder[rowAndColl];
		rowsOrder[rowAndColl] = rowsOrder[maxRowIndex];
		rowsOrder[maxRowIndex] = tmp;

		return true;
	}

	return false;
}

#if TEST
	void print_ordered_complementary_mat(double A[][SIZE], double B[], int rowsOrder[], int m, int n) {
		for(int row = 0; row < m; ++row) {
			for(int coll = 0; coll < n; ++coll) {
				printf("%.4f ", A[rowsOrder[row]][coll]);
			}
			printf("%.4f\n", B[rowsOrder[row]]);
		}
	}

	void print_ordered_mat(double A[][SIZE], int rowsOrder[], int m, int n) {
		for(int row = 0; row < m; ++row) {
			for(int coll = 0; coll < n; ++coll) {
				printf("%.4f ", A[rowsOrder[row]][coll]);
			}
			printf("\n");
		}
	}
#endif

// 3. Matrix triangulation, determinant calculation, and Ax = b solving - extended version
// (Swap the rows so that the row with the largest, leftmost nonzero entry is on top. While
// swapping the rows use index vector - do not copy entire rows.)
// If max A[i][i] < eps, function returns 0.
// If det != 0 && b != NULL && x != NULL then vector x should contain solution of Ax = b.

double gauss(double A[][SIZE], const double b[], double x[], const int n, const double eps) {
	double determinant = 1.0;
	double B[n];

	// index - row number in matrix, rowsOrder[index] - row number in matrix A
	int rowsOrder[n];
	for (int i = 0; i < n; i++){
		rowsOrder[i] = i;
		B[i] = b[i];
	}

	for(int rowAndColl = 0; rowAndColl < n; rowAndColl++){

		if (swap_max_row(A, rowsOrder, n, rowAndColl)){
			determinant = -determinant;
		}

		double scalar = A[rowsOrder[rowAndColl]][rowAndColl];

		if (double_abs(scalar) < eps){
			return 0.0;
		}

		determinant *= scalar;

		// update current row
		for (int i = rowAndColl; i < n; i++){
			A[rowsOrder[rowAndColl]][i] /= scalar;
		}
		B[rowsOrder[rowAndColl]] /= scalar;

		// update remaining rows
		for (int row = rowAndColl + 1; row < n; row++){
			scalar = A[rowsOrder[row]][rowAndColl];
			for (int coll = rowAndColl; coll < n; coll++){
				A[rowsOrder[row]][coll] -= A[rowsOrder[rowAndColl]][coll] * scalar;
			}
			B[rowsOrder[row]] -= B[rowsOrder[rowAndColl]] * scalar;
		}		
	}

	for (int rowAndColl = n - 1; 0 <= rowAndColl; rowAndColl--){
		for (int coll = n - 1; coll > rowAndColl; coll--){
			#if TEST
				printf("B: %.4f, x: %.4f\n", x[coll], B[rowsOrder[coll]]);
			#endif

			B[rowsOrder[rowAndColl]] -= x[coll] * A[rowsOrder[rowAndColl]][coll];
			A[rowsOrder[rowAndColl]][coll] = 0.;
		}

		x[rowAndColl] = B[rowsOrder[rowAndColl]];

		#if TEST
			printf("\n");
			print_ordered_complementary_mat(A, B, rowsOrder, n, n);
		#endif
	}


	return determinant;
}

// 4. Returns the determinant; B contains the inverse of A (if det(A) != 0)
// If max A[i][i] < eps, function returns 0.
double matrix_inv(double A[][SIZE], double B[][SIZE], int n, double eps) {
	double determinant = 1.0;
	int rowsOrder[n];

	double I[n][SIZE];

	for(int row = 0; row < n; row++){
		for (int coll = 0; coll < n; coll++){
			I[row][coll] = 0.0;
		}
	}

	for (int i = 0; i < n; i++){
		rowsOrder[i] = i;
		I[i][i] = 1.0;
	}

	for(int rowAndColl = 0; rowAndColl < n; rowAndColl++){

		if (swap_max_row(A, rowsOrder, n, rowAndColl) && determinant != 0.0){
			determinant = -determinant;
		}

		double scalar = A[rowsOrder[rowAndColl]][rowAndColl];

		if (double_abs(scalar) < eps){
			determinant = 0.0;
		}

		determinant *= scalar;

		// update current row
		for (int i = 0; i < n; i++){
			A[rowsOrder[rowAndColl]][i] /= scalar;
			I[rowsOrder[rowAndColl]][i] /= scalar;
		}

		// update remaining rows
		for (int row = rowAndColl + 1; row < n; row++){

			// update A matrix
			scalar = A[rowsOrder[row]][rowAndColl];
			for (int coll = rowAndColl; coll < n; coll++){
				A[rowsOrder[row]][coll] -= A[rowsOrder[rowAndColl]][coll] * scalar;
			}

			// update I matrix
			for (int coll = 0; coll < n; coll++){
				I[rowsOrder[row]][coll] -= I[rowsOrder[rowAndColl]][coll] * scalar;
			}
		}
	}

	#if TEST
		printf("\nmatrix A:\n");
		print_ordered_mat(A, rowsOrder, n, n);
		printf("\nmatrix I:\n");
		print_ordered_mat(I, rowsOrder, n, n);
	#endif

	for (int row = n - 2; 0 <= row; row--){
		for (int coll = n - 1; coll > row; coll--){
			double scalar = A[rowsOrder[row]][coll];
			for (int i = 0; i < n; i++){
				I[rowsOrder[row]][i] -= scalar * I[rowsOrder[coll]][i];
			}
			A[rowsOrder[row]][coll] = 0.;
		}

	#if TEST
		printf("\nmatrix A:\n");
		print_ordered_mat(A, rowsOrder, n, n);
		printf("\nmatrix I:\n");
		print_ordered_mat(I, rowsOrder, n, n);
	#endif

	}

	for (int row = 0; row < n; row++){
		for (int coll = 0; coll < n; coll++){
			B[row][coll] = I[rowsOrder[row]][coll];
		}
	}


	return determinant;
}

int main(void) {

	double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
	double b[SIZE], x[SIZE], det, eps = 1.e-13;

	int to_do;
	int m, n, p;

	scanf ("%d", &to_do);

	switch (to_do) {
		case 1:
			scanf("%d %d %d", &m, &p, &n);
			read_mat(A, m, p);
			read_mat(B, p, n);
			mat_product(A, B, C, m, p, n);
			print_mat(C, m, n);
			break;
		case 2:
			scanf("%d", &n);
			read_mat(A, n, n);
			printf("%.4f\n", gauss_simplified(A, n));
			break;
		case 3:
			scanf("%d", &n);
			read_mat(A,n, n);
			read_vector(b, n);
			det = gauss(A, b, x, n, eps);
			printf("%.4f\n", det);
			if (det) print_vector(x, n);
			break;
		case 4:
			scanf("%d", &n);
			read_mat(A, n, n);
			det = matrix_inv(A, B, n, eps);
			printf("%.4f\n", det);
			if (det) print_mat(B, n, n);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}


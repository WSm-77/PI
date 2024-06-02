#include <stdio.h>
#include <stdlib.h>
#include <float.h>

typedef struct _matrix
{
    int rows;
    int cols;
    double **data;
} Matrix;

void print_matrix(Matrix *pmatrix){
    for (int r = 0; r < pmatrix->rows; r++){
        for (int c = 0; c < pmatrix->cols; c++){
            printf("%.2lf ", (pmatrix->data)[r][c]);
        }
        printf("\n");
    }
}

int create_matrix(Matrix *pmatrix, int rows, int cols){
    if (rows < 1 || cols < 1){
        return 0;
    }


    pmatrix->rows = rows;
    pmatrix->cols = cols;

    pmatrix->data = malloc(sizeof(double *) * rows);

    for (int r = 0; r < rows; r++){
        (pmatrix->data)[r] = malloc(sizeof(double) * cols);
    }

    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols; c++){
            (pmatrix->data)[r][c] = 0;
        }
    }

    return 1;
}

void free_matrix(Matrix *pmatrix){
    for (int r = 0; r < pmatrix->rows; r++){
        free(pmatrix->data[r]);
    }

    free(pmatrix->data);
    pmatrix->data = NULL;
}

int get(Matrix *pmatrix, int row, int coll, double *val){
    if (row < 0 || coll < 0 || row >= pmatrix->rows || coll >= pmatrix->cols || NULL == pmatrix->data){
        return 0;
    }

    *val = (pmatrix->data)[row][coll];
    return 1;
}

int set(Matrix *pmatrix, int row, int coll, double val){
    if (row < 0 || coll < 0 || row >= pmatrix->rows || coll >= pmatrix->cols || NULL == pmatrix->data){
        return 0;
    }

    (pmatrix->data)[row][coll] = val;

    return 1;
}

void create_identity_matrix(Matrix *pmatrix, int size){
    (void)create_matrix(pmatrix, size, size);

    for (int i = 0; i < size; i++){
        pmatrix->data[i][i] = 1.0;
    }
}

double dot_prod_vect(const double *vect1, const double *vect2, int size){
    double product = 0.0;

    for (int i = 0; i < size; i++){
        product += vect1[i] * vect2[i];
    }
    
    return product;
}

void mul_vect(double *result, Matrix *pmatrix, const double *vect){
    for (int i = 0; i < pmatrix->rows; i++){
        result[i] = dot_prod_vect(pmatrix->data[i], vect, pmatrix->cols);
    }
}

void max_elements_in_rows(double *maxdata, Matrix *pmatrix){
    for (int r = 0; r < pmatrix->rows; r++){
        double maxVal = -DBL_MAX;
        for (int c = 0; c < pmatrix->cols; c++){
            if (pmatrix->data[r][c] > maxVal){
                maxVal = pmatrix->data[r][c];
            }
        }
        maxdata[r] = maxVal;
    }
}

int main(){
    // Matrix matrix;
    // int r = 5, c = 10;

    // if (create_matrix(&matrix, r, c)){
    //     print_matrix(&matrix);
    // }

    // free_matrix(&matrix);

    Matrix m;
    create_identity_matrix(&m, 5);
    print_matrix(&m);

    printf("\n");

    set(&m, 0, 1, 5);
    print_matrix(&m);

    double a;
    get(&m, 0, 1, &a);
    printf("%f \n", a);


    double v1[] = {1, 2, 3, 4, 5};
    double v2[] = {1, 2, 3, 4, 5};
    double v3[5];
    max_elements_in_rows(v3, &m);

    printf("%f \n", dot_prod_vect(v1, v2, 5));

    for(int i = 0; i < 5; i++)
        printf("%f ", v3[i]);

    printf("\n");

    for(int i = 0; i < m.rows; i++){
        for(int j = 0; j < m.cols; j++){
            set(&m, i, j, i);
        }
    }

    print_matrix(&m);

    mul_vect(v3, &m, v1);
    for(int i = 0; i < 5; i++)
        printf("%f ", v3[i]);

    printf("\n");

    free_matrix(&m);

    return 0;
}
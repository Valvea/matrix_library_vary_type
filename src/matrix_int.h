#ifndef MATRIX_INT_H
#define MATRIX_INT_H
#include "matrix_common.h"

#ifdef __cplusplus
extern "C" {
#endif

int **scan_matrix_int(int **matrix_ptr, int *rows, int *cols);
int **create_matrix_int(int rows, int cols);
int **unit_matrix_int(int rows, int cols);
int **copy_matrix_int(const int *const *matrix, int rows, int cols);
int fill_matrix_int(int **matrix, int rows, int cols);
void print_matrix_int(const int *const *matrix, int rows, int cols);
int **sort_matrix_int(int **matrix, int rows, int cols);
double **cast_matrix_to_double(const int *const *matrix, int rows, int cols);
void free_matrix_int(int **matrix);

int search_array_index_int(search_element element_type, const int array[], int value, int length);
int *scale_array_int(const int array[], const int scalar, int length);
int *scale_array_inplace_int(int array[], int scalar, int length);
int *add_array_int(const int array_a[], const int array_b[], int length);
int *add_array_inplace_int(int array_a[], const int array_b[], int length);
int *sub_array_int(const int array_a[], const int array_b[], int length);
int *sub_array_inplace_int(int array_a[], const int array_b[], int length);
int sum_array_int(const int array[], int length);
int min_array_int(const int array[], int length);
int max_array_int(const int array[], int length);
int dot_arrays_int(const int array_a[], const int array_b[], int length);
int *dot_arrays_Hadamard_int(const int array_a[], const int array_b[], int length);

int **from_array_to_matrix_int(const int array[], int length_array, int rows, int cols);
int *row_to_array_int(const int *const *matrix, int array[], int row_idx, int cols);
int *col_to_array_int(const int *const *matrix, int array[], int col_idx, int rows);

int set_row_int(int **matrix, const int array[], int row_idx, int rows, int cols);
int set_col_int(int **matrix, const int array[], int col_idx, int rows, int cols);

void swap_rows_int(int **matrix, int idx_a, int idx_b, int rows);

int **T_matrix_int(const int *const *matrix_origin, int rows, int cols);
int **T_matrix_inplace_int(int **matrix_origin, int rows, int cols);
int **dot_matrix_int(const int *const *matrix_A, const int *const *matrix_B, int rowsA, int colsA, int rowsB,
                     int colsB);
int **sum_matrix_int(const int *const *matrix_A, const int *const *matrix_B, int rows, int cols);
int **sub_matrix_int(const int *const *matrix_A, const int *const *matrix_B, int rows, int cols);
int **dot_matrix_Hadamard_int(const int *const *matrix_A, const int *const *matrix_B, int rows, int cols);
int *matrix_to_array_int(int *const *matrix, int rows);

#ifdef __cplusplus
}
#endif

#endif  // MATRIX_INT_H

#ifndef MATRIX_DOUBLE_H
#define MATRIX_DOUBLE_H
#include "matrix_common.h"

#ifdef __cplusplus
extern "C" {
#endif

double **scan_matrix_double(double **matrix_ptr, int *rows, int *cols);
double **create_matrix_double(int rows, int cols);
double **unit_matrix_double(int rows, int cols);
double **copy_matrix_double(const double *const *matrix, int rows, int cols);
int fill_matrix_double(double **matrix, int rows, int cols);
void print_matrix_double(const double *const *matrix, int rows, int cols);
double **sort_matrix_double(double **matrix, int rows, int cols);
void free_matrix_double(double **matrix);

int search_array_index_double(search_element element_type, const double array[], double value, int length);
double *scale_array_double(const double array[], const double scalar, int length);
double *scale_array_inplace_double(double array[], double scalar, int length);
double *add_array_double(const double array_a[], const double array_b[], int length);
double *add_array_inplace_double(double array_a[], const double array_b[], int length);
double *sub_array_double(const double array_a[], const double array_b[], int length);
double *sub_array_inplace_double(double array_a[], const double array_b[], int length);
double sum_array_double(const double array[], int length);
double min_array_double(const double array[], int length);
double max_array_double(const double array[], int length);
double dot_arrays_double(const double array_a[], const double array_b[], int length);
double *dot_arrays_Hadamard_double(const double array_a[], const double array_b[], int length);

double **from_array_to_matrix_double(const double array[], int length_array, int rows, int cols);
double *row_to_array_double(const double *const *matrix, double array[], int row_idx, int cols);
double *col_to_array_double(const double *const *matrix, double array[], int col_idx, int rows);

int set_row_double(double **matrix, const double array[], int row_idx, int rows, int cols);
int set_col_double(double **matrix, const double array[], int col_idx, int rows, int cols);

void swap_rows_double(double **matrix, int idx_a, int idx_b, int rows);

double **T_matrix_double(const double *const *matrix_origin, int rows, int cols);
double **T_matrix_inplace_double(double **matrix_origin, int rows, int cols);
double **dot_matrix_double(const double *const *matrix_A, const double *const *matrix_B, int rowsA, int colsA,
                           int rowsB, int colsB);
double **sum_matrix_double(const double *const *matrix_A, const double *const *matrix_B, int rows, int cols);
double **sub_matrix_double(const double *const *matrix_A, const double *const *matrix_B, int rows, int cols);
double **dot_matrix_Hadamard_double(const double *const *matrix_A, const double *const *matrix_B, int rows,
                                    int cols);
double *matrix_to_array_double(double *const *matrix, int rows);

#ifdef __cplusplus
}
#endif

#endif  // MATRIX_DOUBLE_H

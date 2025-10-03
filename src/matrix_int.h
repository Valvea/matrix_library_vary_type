#ifndef MATRIX_INT_H
#define MATRIX_INT_H
int **scan_matrix_int(int **matrix_ptr, int *rows, int *cols);
int **create_matrix_int(int rows, int cols);
int fillmatrix_int(int **matrix, int rows, int cols);
void printmatrix_int(const int *const *matrix, int rows, int cols);
int **sort_matrix_int(int **matrix, int rows, int cols);
void free_matrix_int(int **matrix);

int sumarray_int(const int *array, int length);
int minarray_int(const int *array, int length);
int maxarray_int(const int *array, int length);
int dot_arrays_int(const int *array_a, const int *array_b, int length);

int **from_array_to_matrix_int(const int array[], int length_array, int rows,
                               int cols);
int *row_to_array_int(const int *const *matrix, int array[], int row_idx,
                      int cols);
int *col_to_array_int(const int *const *matrix, int array[], int col_idx,
                      int rows);

int **T_matrix_int(const int *const *matrix_origin, int rows, int cols);
int **dot_matrix_int(const int *const *matrix_A, const int *const *matrix_B,
                     int rowsA, int colsA, int rowsB, int colsB);
int **sum_matrix_int(const int *const *matrix_a, const int *const *matrix_b,
                     int rows, int cols);
int *matrix_to_array_int(int *const *matrix, int rows);

#endif // MATRIX_INT_H
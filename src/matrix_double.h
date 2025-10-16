#ifndef MATRIX_DOUBLE_H
#define MATRIX_DOUBLE_H
#include "matrix_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Сканирует матрицу из stdin; при необходимости переаллокация через create_matrix_double */
double **scan_matrix_double(double **matrix_ptr, int *rows, int *cols);
/* Выделяет матрицу rows×cols с нулевой инициализацией */
double **create_matrix_double(int rows, int cols);
/* Возвращает единичную матрицу; только для квадратных размеров */
double **unit_matrix_double(int rows, int cols);
/* Создаёт глубокую копию матрицы */
double **copy_matrix_double(const double *const *matrix, int rows, int cols);
/* Заполняет матрицу данными, считанными из stdin */
int fill_matrix_double(double **matrix, int rows, int cols);
/* Печатает матрицу в stdout */
void print_matrix_double(const double *const *matrix, int rows, int cols);
/* Возвращает отсортированную копию исходной матрицы */
double **sort_matrix_double(double **matrix, int rows, int cols);
/* Освобождает память матрицы double */
void free_matrix_double(double **matrix);

/* Возвращает индекс элемента по стратегии поиска (минимум/максимум/равенство/максимальный модуль) */
int search_array_index_double(search_element element_type, const double array[], double value, int length);
/* Масштабирует массив, возвращая новый буфер */
double *scale_array_double(const double array[], const double scalar, int length);
/* Масштабирует массив на месте */
double *scale_array_inplace_double(double array[], double scalar, int length);
/* Суммирует два массива в новом буфере */
double *add_array_double(const double array_a[], const double array_b[], int length);
/* Суммирует массивы, записывая результат в array_a */
double *add_array_inplace_double(double array_a[], const double array_b[], int length);
/* Вычитает array_b из array_a в новом буфере */
double *sub_array_double(const double array_a[], const double array_b[], int length);
/* Вычитает array_b из array_a на месте */
double *sub_array_inplace_double(double array_a[], const double array_b[], int length);
/* Сумма элементов массива */
double sum_array_double(const double array[], int length);
/* Минимальный элемент массива */
double min_array_double(const double array[], int length);
/* Максимальный элемент массива */
double max_array_double(const double array[], int length);
/* Скалярное произведение массивов */
double dot_arrays_double(const double array_a[], const double array_b[], int length);
/* Поэлементное произведение (Адамарова) массивов */
double *dot_arrays_Hadamard_double(const double array_a[], const double array_b[], int length);

/* Строит матрицу из плоского массива длиной rows*cols */
double **from_array_to_matrix_double(const double array[], int length_array, int rows, int cols);
/* Копирует выбранную строку в заданный буфер */
double *row_to_array_double(const double *const *matrix, double array[], int row_idx, int cols);
/* Копирует выбранный столбец в буфер */
double *col_to_array_double(const double *const *matrix, double array[], int col_idx, int rows);

/* Заменяет строку матрицы содержимым массива */
int set_row_double(double **matrix, const double array[], int row_idx, int rows, int cols);
/* Заменяет столбец матрицы */
int set_col_double(double **matrix, const double array[], int col_idx, int rows, int cols);

/* Меняет местами две строки */
void swap_rows_double(double **matrix, int idx_a, int idx_b, int rows);

/* Строит транспонированную копию матрицы */
double **T_matrix_double(const double *const *matrix_origin, int rows, int cols);
/* Возвращает транспонированную матрицу, освобождая исходную */
double **T_matrix_inplace_double(double **matrix_origin, int rows, int cols);
/* Стандартное умножение матриц A×B */
double **dot_matrix_double(const double *const *matrix_A, const double *const *matrix_B, int rowsA, int colsA,
                           int rowsB, int colsB);
/* Поэлементная сумма матриц */
double **sum_matrix_double(const double *const *matrix_A, const double *const *matrix_B, int rows, int cols);
/* Поэлементная разность матриц */
double **sub_matrix_double(const double *const *matrix_A, const double *const *matrix_B, int rows, int cols);
/* Поэлементное произведение (Адамара) матриц */
double **dot_matrix_Hadamard_double(const double *const *matrix_A, const double *const *matrix_B, int rows,
                                    int cols);
/* Возвращает указатель на плоский буфер, расположенный после массива строк */
double *matrix_to_array_double(double *const *matrix, int rows);

#ifdef __cplusplus
}
#endif

#endif  // MATRIX_DOUBLE_H

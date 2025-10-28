#ifndef MATRIX_INT_H
#define MATRIX_INT_H
#include "matrix_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Считывает матрицу из stdin, при необходимости выделяя новую */
int **scan_matrix_int(int **matrix_ptr, int *rows, int *cols);
/* Выделяет матрицу rows×cols и обнуляет элементы */
int **create_matrix_int(int rows, int cols);
/* Создаёт единичную матрицу (только для квадратных размеров) */
int **unit_matrix_int(int rows, int cols);
/* Возвращает глубокую копию матрицы */
int **copy_matrix_int(const int *const *matrix, int rows, int cols);
/* Заполняет матрицу данными из stdin */
int fill_matrix_int(int **matrix, int rows, int cols);
/* Печатает матрицу в stdout */
void print_matrix_int(const int *const *matrix, int rows, int cols);
/* Возвращает отсортированную копию матрицы */
int **sort_matrix_int(int **matrix, int rows, int cols);
/* Создаёт double-матрицу, копируя значения int */
double **cast_matrix_to_double(const int *const *matrix, int rows, int cols);
/* Освобождает память int-матрицы */
void free_matrix_int(int **matrix);

/* Возвращает индекс элемента по заданной стратегии поиска */
int search_array_index_int(search_element element_type, const int array[], int value, int length);
/* Создаёт новый массив, умноженный на скаляр */
int *scale_array_int(const int array[], const int scalar, int length);
/* Масштабирует массив на месте */
int *scale_array_inplace_int(int array[], int scalar, int length);
/* Складывает массивы, создавая новый буфер */
int *add_array_int(const int array_a[], const int array_b[], int length);
/* Складывает массивы в первом аргументе */
int *add_array_inplace_int(int array_a[], const int array_b[], int length);
/* Вычитает массивы в новом буфере */
int *sub_array_int(const int array_a[], const int array_b[], int length);
/* Вычитает массивы на месте */
int *sub_array_inplace_int(int array_a[], const int array_b[], int length);
/* Возвращает сумму элементов массива */
int sum_array_int(const int array[], int length);
/* Минимальный элемент массива */
int min_array_int(const int array[], int length);
/* Максимальный элемент массива */
int max_array_int(const int array[], int length);
/* Скалярное произведение массивов */
int dot_arrays_int(const int array_a[], const int array_b[], int length);
/* Поэлементное произведение массивов. */
int *dot_arrays_Hadamard_int(const int array_a[], const int array_b[], int length);
/* Поэлементное произведение массивов (результат в array_a). */
int *dot_arrays_Hadamard_inplace_int(int array_a[], const int array_b[], int length);

/* Строит матрицу из плоского массива длиной rows*cols */
int **from_array_to_matrix_int(const int array[], int length_array, int rows, int cols);
/* Копирует выбранную строку в буфер */
int *row_to_array_int(const int *const *matrix, int array[], int row_idx, int cols);
/* Копирует выбранный столбец в буфер */
int *col_to_array_int(const int *const *matrix, int array[], int col_idx, int rows);

/* Заменяет строку матрицы значениями массива */
int set_row_int(int **matrix, const int array[], int row_idx, int rows, int cols);
/* Заменяет столбец матрицы */
int set_col_int(int **matrix, const int array[], int col_idx, int rows, int cols);

/* Меняет местами строки матрицы */
void swap_rows_int(int **matrix, int idx_a, int idx_b, int rows);

/* Возвращает транспонированную копию матрицы */
int **T_matrix_int(const int *const *matrix_origin, int rows, int cols);
/* Возвращает транспонированную матрицу, освобождая исходную */
int **T_matrix_inplace_int(int **matrix_origin, int rows, int cols);
/* Стандартное умножение матриц A×B */
int **dot_matrix_int(const int *const *matrix_A, const int *const *matrix_B, int rowsA, int colsA, int rowsB,
                     int colsB);
/* Поэлементная сумма матриц. */
int **sum_matrix_int(const int *const *matrix_A, const int *const *matrix_B, int rows, int cols);
/* Поэлементная сумма матриц (результат в matrix_A). */
int **sum_matrix_inplace_int(int **matrix_A, const int *const *matrix_B, int rows, int cols);
/* Поэлементная разность матриц. */
int **sub_matrix_int(const int *const *matrix_A, const int *const *matrix_B, int rows, int cols);
/* Поэлементная разность матриц (результат в matrix_A). */
int **sub_matrix_inplace_int(int **matrix_A, const int *const *matrix_B, int rows, int cols);
/* Поэлементное произведение (Адамара) матриц. */
int **dot_matrix_Hadamard_int(const int *const *matrix_A, const int *const *matrix_B, int rows, int cols);
/* Поэлементное произведение (Адамара) матриц (результат в matrix_A). */
int **dot_matrix_Hadamard_inplace_int(int **matrix_A, const int *const *matrix_B, int rows, int cols);
/* Возвращает указатель на плоский буфер, следующий за массивом указателей на строки */
int *matrix_to_array_int(int *const *matrix, int rows);

#ifdef __cplusplus
}
#endif

#endif  // MATRIX_INT_H

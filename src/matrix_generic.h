// matrix_generic.h — единый фасад для int/double реализаций.
// Тип подбирается по аргументам вызова (C11 _Generic).
#ifndef MATRIX_GENERIC_H
#define MATRIX_GENERIC_H

#include "matrix_double.h"
#include "matrix_int.h"

/* ============ СОЗДАНИЕ МАТРИЦ ============ */

/* по имени типа:
   int    **A = create_matrix_as(int,    r, c);
   double **B = create_matrix_as(double, r, c);
*/
#define create_matrix_as(T, rows, cols) \
    _Generic(((T)0), int: create_matrix_int, double: create_matrix_double)((rows), (cols))

/* по «образцу» указателя:
   int **A = NULL;    A = create_matrix_like(A, r, c);
   double **B = NULL; B = create_matrix_like(B, r, c);
*/
#define create_matrix_like(MPTR, rows, cols) \
    _Generic((MPTR), int **: create_matrix_int, double **: create_matrix_double)((rows), (cols))

#define unit_matrix_as(T, rows, cols) \
    _Generic(((T)0), int: unit_matrix_int, double: unit_matrix_double)((rows), (cols))

#define unit_matrix_like(MPTR, rows, cols) \
    _Generic((MPTR), int **: unit_matrix_int, double **: unit_matrix_double)((rows), (cols))

/* ============ БАЗОВЫЕ ОПЕРАЦИИ / IO ============ */

/* scan: реализация ожидает T** → без кастов */
#define scan_matrix(MPTR, rows_ptr, cols_ptr) \
    _Generic((MPTR), int **: scan_matrix_int, double **: scan_matrix_double)((MPTR), (rows_ptr), (cols_ptr))

/* print: сигнатура const T * const * → нужен явный каст */
#define print_matrix(MPTR, rows, cols)                                        \
    _Generic((MPTR),                                                          \
        int **: print_matrix_int((const int *const *)(MPTR), (rows), (cols)), \
        double **: print_matrix_double((const double *const *)(MPTR), (rows), (cols)))

/* fill: ожидает T** → без кастов */
#define fill_matrix(MPTR, rows, cols) \
    _Generic((MPTR), int **: fill_matrix_int, double **: fill_matrix_double)((MPTR), (rows), (cols))

/* free: ожидает T** → без кастов */
#define free_matrix(MPTR) _Generic((MPTR), int **: free_matrix_int, double **: free_matrix_double)((MPTR))

/* ============ ПРЕОБРАЗОВАНИЯ / ДОСТУП ============ */

/* matrix → flat array: сигнатура T * const * → нужен явный каст */
#define matrix_to_array(MPTR, rows)                                \
    _Generic((MPTR),                                               \
        int **: matrix_to_array_int((int *const *)(MPTR), (rows)), \
        double **: matrix_to_array_double((double *const *)(MPTR), (rows)))

/* flat array → matrix: сигнатура const T* → _Generic по ARR */
#define from_array_to_matrix(ARR, len, rows, cols)                                                       \
    _Generic((ARR), const int *: from_array_to_matrix_int, const double *: from_array_to_matrix_double)( \
        (ARR), (len), (rows), (cols))

/* row_to_array: сигнатура const T * const * → каст */
#define row_to_array(MPTR, BUF, row_idx, cols)                                          \
    _Generic((MPTR),                                                                    \
        int **: row_to_array_int((const int *const *)(MPTR), (BUF), (row_idx), (cols)), \
        double **: row_to_array_double((const double *const *)(MPTR), (BUF), (row_idx), (cols)))

/* col_to_array: сигнатура const T * const * → каст */
#define col_to_array(MPTR, BUF, rows, col_idx)                                          \
    _Generic((MPTR),                                                                    \
        int **: col_to_array_int((const int *const *)(MPTR), (BUF), (col_idx), (rows)), \
        double **: col_to_array_double((const double *const *)(MPTR), (BUF), (col_idx), (rows)))

/* set_row / set_col: ожидают T** + const T[] → без кастов */
#define set_row(MPTR, BUF, row_idx, rows, cols) \
    _Generic((MPTR), int **: set_row_int, double **: set_row_double)((MPTR), (BUF), (row_idx), (rows), (cols))

#define set_col(MPTR, BUF, col_idx, rows, cols) \
    _Generic((MPTR), int **: set_col_int, double **: set_col_double)((MPTR), (BUF), (col_idx), (rows), (cols))

#define swap_rows(MPTR, idx_a, idx_b, rows) \
    _Generic((MPTR), int **: swap_rows_int, double **: swap_rows_double)((MPTR), (idx_a), (idx_b), (rows))

/* ============ МАТРИЧНЫЕ ОПЕРАЦИИ ============ */

/* sort: ожидает T** → без кастов */
#define sort_matrix(MPTR, rows, cols) \
    _Generic((MPTR), int **: sort_matrix_int, double **: sort_matrix_double)((MPTR), (rows), (cols))

/* transpose: сигнатура const T * const * → каст */
#define T_matrix(MPTR, rows, cols)                                        \
    _Generic((MPTR),                                                      \
        int **: T_matrix_int((const int *const *)(MPTR), (rows), (cols)), \
        double **: T_matrix_double((const double *const *)(MPTR), (rows), (cols)))

#define T_matrix_inplace(MPTR, rows, cols) \
    _Generic((MPTR), int **: T_matrix_inplace_int, double **: T_matrix_inplace_double)((MPTR), (rows), (cols))

/* sum_matrix: обе матрицы const T * const * → касты */
#define sum_matrix(A, B, rows, cols)                                                                 \
    _Generic((A),                                                                                    \
        int **: sum_matrix_int((const int *const *)(A), (const int *const *)(B), (rows), (cols)),    \
        double **: sum_matrix_double((const double *const *)(A), (const double *const *)(B), (rows), \
                                     (cols)))

#define sum_matrix_inplace(A, B, rows, cols)                                                         \
    _Generic((A),                                                                                    \
        int **: sum_matrix_inplace_int((A), (const int *const *)(B), (rows), (cols)),                \
        double **: sum_matrix_inplace_double((A), (const double *const *)(B), (rows), (cols)))

#define sub_matrix(A, B, rows, cols)                                                                 \
    _Generic((A),                                                                                    \
        int **: sub_matrix_int((const int *const *)(A), (const int *const *)(B), (rows), (cols)),    \
        double **: sub_matrix_double((const double *const *)(A), (const double *const *)(B), (rows), \
                                     (cols)))

#define sub_matrix_inplace(A, B, rows, cols)                                                         \
    _Generic((A),                                                                                    \
        int **: sub_matrix_inplace_int((A), (const int *const *)(B), (rows), (cols)),                \
        double **: sub_matrix_inplace_double((A), (const double *const *)(B), (rows), (cols)))

#define dot_matrix_Hadamard(A, B, rows, cols)                                                              \
    _Generic((A),                                                                                          \
        int **: dot_matrix_Hadamard_int((const int *const *)(A), (const int *const *)(B), (rows), (cols)), \
        double **: dot_matrix_Hadamard_double((const double *const *)(A), (const double *const *)(B),      \
                                              (rows), (cols)))

#define dot_matrix_Hadamard_inplace(A, B, rows, cols)                                                      \
    _Generic((A),                                                                                          \
        int **: dot_matrix_Hadamard_inplace_int((A), (const int *const *)(B), (rows), (cols)),             \
        double **: dot_matrix_Hadamard_inplace_double((A), (const double *const *)(B), (rows), (cols)))

/* dot_matrix: параметры const T * const * → касты */
#define dot_matrix(A, rowsA, colsA, B, rowsB, colsB)                                                        \
    _Generic((A),                                                                                           \
        int **: dot_matrix_int((const int *const *)(A), (rowsA), (colsA), (const int *const *)(B), (rowsB), \
                               (colsB)),                                                                    \
        double **: dot_matrix_double((const double *const *)(A), (rowsA), (colsA),                          \
                                     (const double *const *)(B), (rowsB), (colsB)))

/* ============ ВЕКТОРНЫЕ ФУНКЦИИ ============ */

#define dot_arrays(A, B, len) \
    _Generic((A), const int *: dot_arrays_int, const double *: dot_arrays_double)((A), (B), (len))

#define dot_arrays_Hadamard(A, B, len)                                                               \
    _Generic((A), const int *: dot_arrays_Hadamard_int, const double *: dot_arrays_Hadamard_double)( \
        (A), (B), (len))

#define dot_arrays_Hadamard_inplace(A, B, len)                                                        \
    _Generic((A), int *: dot_arrays_Hadamard_inplace_int, double *: dot_arrays_Hadamard_inplace_double)((A), (B), (len))

#define search_array_index(ARR, ELEMENT, KIND, LEN)                                                  \
    _Generic((ARR), const int *: search_array_index_int, const double *: search_array_index_double)( \
        (KIND), (ARR), (ELEMENT), (LEN))

#define scale_array(ARR, VALUE, LEN) \
    _Generic((ARR), const int *: scale_array_int, const double *: scale_array_double)((ARR), (VALUE), (LEN))

#define scale_array_inplace(ARR, VALUE, LEN)                                                              \
    _Generic((ARR), int *: scale_array_inplace_int, double *: scale_array_inplace_double)((ARR), (VALUE), \
                                                                                          (LEN))

#define add_array(A, B, LEN) \
    _Generic((A), const int *: add_array_int, const double *: add_array_double)((A), (B), (LEN))

#define add_array_inplace(A, B, LEN) \
    _Generic((A), int *: add_array_inplace_int, double *: add_array_inplace_double)((A), (B), (LEN))

#define sub_array(A, B, LEN) \
    _Generic((A), const int *: sub_array_int, const double *: sub_array_double)((A), (B), (LEN))

#define sub_array_inplace(A, B, LEN) \
    _Generic((A), int *: sub_array_inplace_int, double *: sub_array_inplace_double)((A), (B), (LEN))

#define sum_array(ARR, LEN) \
    _Generic((ARR), const int *: sum_array_int, const double *: sum_array_double)((ARR), (LEN))

#define min_array(ARR, LEN) \
    _Generic((ARR), const int *: min_array_int, const double *: min_array_double)((ARR), (LEN))

#define max_array(ARR, LEN) \
    _Generic((ARR), const int *: max_array_int, const double *: max_array_double)((ARR), (LEN))

#endif /* MATRIX_GENERIC_H */

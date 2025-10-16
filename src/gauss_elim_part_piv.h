#ifndef GAUSS_ELIM_PART_PIV_H
#define GAUSS_ELIM_PART_PIV_H
#include "matrix_common.h"
#include "matrix_double.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Результат работы алгоритма: статус, детерминант, обратная матрица и число перестановок */
typedef struct {
    int status;            /* 0 — успех, положительные значения обозначают особые случаи */
    double determinant;    /* Вычисленный детерминант (для INVERSE используется как побочный результат) */
    double **invers_matrix;/* Указатель на обратную матрицу, если она найдена */
    int swaps;             /* Количество перестановок строк при частичном выборе опорного элемента */

} result_solve;

/* Режим работы: вычисление только детерминанта либо расширенная задача для обратной матрицы */
typedef enum { DET, INVERSE } solve_mode;

/* Выполняет LU-разложение с частичным выбором опорного элемента (partial pivoting).
   В режиме DET возвращает только детерминант, в режиме INVERSE дополнительно строит обратную матрицу. */
result_solve matrix_det_lu_double(solve_mode mode, const double * const*matrix_origin, int rows, int cols);

#ifdef __cplusplus
}
#endif

#endif

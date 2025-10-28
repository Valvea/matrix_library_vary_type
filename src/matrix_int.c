#define MATRIX_TYPE int /* Базовый тип элементов матрицы */
#define MATRIX_TAG int /* Суффикс, который будет подставлен в имена функций */
#define MATRIX_SCANF "%d"  /* Спецификатор для чтения элементов */
#define MATRIX_PRINTF "%d" /* Спецификатор для печати элементов */

#include "matrix_template.h"
#undef MATRIX_TYPE
#undef MATRIX_TAG
#undef MATRIX_SCANF
#undef MATRIX_PRINTF
#include "matrix_double.h"

double **cast_matrix_to_double(const int *const *matrix, int rows, int cols) {
    if (rows <= 0 || cols <= 0 || !matrix) return NULL;

    double **result = create_matrix_double(rows, cols);

    if (!result) return NULL;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            result[row][col] = (double)matrix[row][col];
        }
    }
    return result;
}

#include "matrix_double.h"
#include "matrix_int.h"

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
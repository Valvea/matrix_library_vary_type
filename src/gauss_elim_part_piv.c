#include "gauss_elim_part_piv.h"

#include <math.h>

/* Реализует LU-разложение с частичным выбором опорного элемента
 * и опциональным поиском обратной матрицы. */
result_solve matrix_det_lu_double(solve_mode mode, const double *const *matrix_origin, int rows, int cols) {
    result_solve out = {.status = -1, .determinant = 0, .invers_matrix = NULL, .swaps = 0};

    /* Проверяем входные данные: нужна квадратная ненулевая матрица */
    if (!matrix_origin || rows <= 0 || cols <= 0 || rows != cols) return out;

    /* Делаем независимую копию исходной матрицы, чтобы не портить вход */
    double **matrix = copy_matrix_double(matrix_origin, rows, cols);
    if (!matrix) {
        out.status = -2;
        return out;
    }

    double **RHS = NULL;
    if (mode == INVERSE) {
        /* Инициализируем правую часть единичной матрицей (после перестановок) */
        RHS = unit_matrix_double(rows, cols);
        if (!RHS) {
            out.status = -2;
            free_matrix_double(matrix);
            return out;
        }
    }

    out.determinant = 1.;
    /* Прямой ход: строим LU-разложение с перестановками строк */
    for (int col = 0; col < cols; col++) {
        int row_of_max = col;
        double max_in_col = 0.;
        /* Находим строку с максимальным по модулю элементом в текущем столбце */
        for (int row = col; row < cols; row++) {
            double value = module_value(matrix[row][col]);
            if (value > max_in_col) {
                max_in_col = value;
                row_of_max = row;
            }
        }

        /* Если столбец нулевой — матрица сингулярна, детерминант равен 0 */
        if (max_in_col == 0.0) {
            if (RHS) free_matrix_double(RHS);
            free_matrix_double(matrix);
            out.determinant = 0.;
            out.status = 1;
            return out;
        }

        double pivot_abs = module_value(matrix[row_of_max][col]);
        /* Дополнительная относительная проверка малого pivot */
        if (pivot_abs < MATRIX_EPSILON * max_in_col) {
            out.status = 1;
            out.determinant = 0.0;
            if (RHS) free_matrix_double(RHS);
            free_matrix_double(matrix);
            return out;
        }

        /* Если лучшая строка не текущая — меняем строки местами */
        if (row_of_max != col) {
            swap_rows_double(matrix, row_of_max, col, rows);
            if (RHS) swap_rows_double(RHS, row_of_max, col, rows);
            out.swaps++;
        }

        /* Опорный элемент на диагонали */
        double diag = matrix[col][col];
        if (module_value(diag) < MATRIX_EPSILON) {
            if (RHS) free_matrix_double(RHS);
            free_matrix_double(matrix);
            out.determinant = 0.;
            out.status = 1;
            return out;
        }
        /* Накопление детерминанта как произведения диагональных элементов U */
        out.determinant *= diag;

        /* Зануляем элементы под диагональю и сохраняем коэффициенты в L */
        for (int row = col + 1; row < rows; row++) {
            double coeff = matrix[row][col] / diag;
            matrix[row][col] = coeff;
            for (int right_col = col + 1; right_col < cols; right_col++) {
                matrix[row][right_col] -= coeff * matrix[col][right_col];
            }
        }
    }

    /* Учитываем чётность перестановок строк */
    out.determinant = out.swaps % 2 == 1 ? -out.determinant : out.determinant;

    if (mode == DET) {
        out.status = 0;
        if (RHS) free_matrix_double(RHS);
        free_matrix_double(matrix);
        return out;
    }

    /* Прямой ход для системы L·Y = P·I (RHS уже содержит перестановки) */
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < row; ++col) {
            double coeff = matrix[row][col];
            if (coeff != 0.0) {
                for (int item = 0; item < cols; ++item) {
                    RHS[row][item] -= coeff * RHS[col][item];
                }
            }
        }
        /* Диагональ нижней треугольной матрицы равна 1, делить не нужно */
    }

    /* Обратный ход U·X = Y */
    for (int row_back = rows - 1; row_back >= 0; --row_back) {
        double uii = matrix[row_back][row_back];
        if (fabs(uii) < MATRIX_EPSILON) {
            out.status = 1;
            out.determinant = 0.0;
            free_matrix_double(matrix);
            free_matrix_double(RHS);
            return out;
        }
        /* Нормируем строку */
        for (int item = 0; item < cols; ++item) RHS[row_back][item] /= uii;

        /* Вычитаем вклад текущей строки из всех верхних */
        for (int row_forward = 0; row_forward < row_back; ++row_forward) {
            double uki = matrix[row_forward][row_back];
            if (uki != 0.0) {
                for (int item = 0; item < cols; ++item) RHS[row_forward][item] -= uki * RHS[row_back][item];
            }
        }
    }

    out.invers_matrix = RHS;  /* RHS теперь содержит A^{-1} */
    out.status = 0;
    free_matrix_double(matrix);
    return out;
}

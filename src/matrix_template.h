// Шаблон для создания матриц с различными типами данных
#ifndef MATRIX_TYPE
#error "Задает тип int double до включения этого файла"
#endif

#ifndef MATRIX_TAG
#error "Определяет TAG (int, double) для названий функций"
#endif

#ifndef MATRIX_SCANF
#error "Определяет MATRIX_SCANF ( \"%d\", \"%lf\")"
#endif

#ifndef MATRIX_PRINTF
#error "Определяет MATRIX_PRINTF ( \"%d\", \"%f\")"
#endif

// Хелпер для склейки имён: function_(create_matrix) -> create_matrix_int /
// create_matrix_double
#define MATRIX_CAT(a, b) MATRIX_CAT_(a, b)
#define MATRIX_CAT_(a, b) a##_##b
#define function_(name) MATRIX_CAT(name, MATRIX_TAG)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix_common.h"

void function_(free_matrix)(MATRIX_TYPE **matrix) { free(matrix); }

inline void function_(swap_elements)(MATRIX_TYPE *first, MATRIX_TYPE *second) {
    MATRIX_TYPE temp = *first;
    *first = *second;
    *second = temp;
}

static int input_row(MATRIX_TYPE *row, int cols) {
    if (!row || cols <= 0) return 0;
    MATRIX_TYPE temp = 0;
    int index = 0;
    char delimiter = OUTPUT_DELIMITER;
    while (index < cols) {
        int scanned = scanf(MATRIX_SCANF "%c", &temp, &delimiter);

        switch (scanned) {
            case EOF:
            case 0:
                return index;
                break;
            case 1:
                if (feof(stdin)) {
                    row[index++] = temp;
                    return index;
                } else
                    return index;
                break;
            case 2:
                if (strchr(ROW_DELIMITERS, delimiter))
                    row[index++] = temp;
                else
                    return index;
                break;
        }
    }
    return index;
}

static void output_row(const MATRIX_TYPE *row, int cols) {
    if (cols <= 0) return;
    for (int col = 0; col < cols - 1; col++) printf(MATRIX_PRINTF "%c", row[col], OUTPUT_DELIMITER);
    printf(MATRIX_PRINTF, row[cols - 1]);
}

MATRIX_TYPE **function_(create_matrix)(int rows, int cols) {
    // единый блок: [rows указателей][rows*cols элементов]
    if (rows <= 0 || cols <= 0) return NULL;
    MATRIX_TYPE **matrix = calloc(1, rows * sizeof *matrix + rows * cols * sizeof **matrix);
    if (!matrix) return NULL;
    MATRIX_TYPE *flat_ptr = (MATRIX_TYPE *)(matrix + rows);
    for (int row = 0; row < rows; row++) matrix[row] = flat_ptr + row * cols;
    return matrix;
}

MATRIX_TYPE **function_(unit_matrix)(int rows, int cols) {
    // единый блок: [rows указателей][rows*cols элементов]
    if (rows <= 0 || cols <= 0 || rows != cols) return NULL;
    MATRIX_TYPE **template_matrix = function_(create_matrix)(rows, cols);

    if (!template_matrix) return NULL;

    for (int row = 0; row < rows; row++) {
        template_matrix[row][row] = (MATRIX_TYPE)1;
    }

    return template_matrix;
}

MATRIX_TYPE *function_(matrix_to_array)(MATRIX_TYPE *const *matrix, int rows) {
    if (!matrix || rows <= 0) return NULL;
    return (MATRIX_TYPE *)(matrix + rows);
}

int function_(fill_matrix)(MATRIX_TYPE **matrix, int rows, int cols) {
    for (int r = 0; r < rows; r++)
        if (input_row(matrix[r], cols) != cols) return 0;
    return 1;
}

void function_(print_matrix)(const MATRIX_TYPE *const *matrix, int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        output_row(matrix[row], cols);
        printf("%c", NEW_LINE);
    }
}

MATRIX_TYPE **function_(scan_matrix)(MATRIX_TYPE **matrix_ptr, int *rows, int *cols) {
    char end_char = NEW_LINE;

    if (scanf("%d %d%c", rows, cols, &end_char) == 3 && end_char == NEW_LINE) {
        matrix_ptr = function_(create_matrix)(*rows, *cols);
        if (!matrix_ptr) return NULL;
        if (!function_(fill_matrix)(matrix_ptr, *rows, *cols)) {
            free(matrix_ptr);
            return NULL;
        } else
            return matrix_ptr;

    } else
        return NULL;
}

int function_(search_array_index)(search_element kind, const MATRIX_TYPE *array, const MATRIX_TYPE element,
                                  int length) {
    if (!array || length <= 0) return -1;

    if (length == 1) return equal_values(array[0], element) ? 0 : -1;

    switch (kind) {
        case FIRST_MIN: {
            int founded_idx = 0;
            MATRIX_TYPE min = element;
            for (int index = 1; index < length; ++index) {
                if (array[index] < min) return index;
            }
            return founded_idx;
        }

        case FIRST_MAX: {
            int founded_idx = 0;
            MATRIX_TYPE max = element;
            for (int index = 1; index < length; ++index) {
                if (array[index] > max) return index;
            }
            return founded_idx;
        }

        case MODULE_MAX: {
            int founded_idx = 0;
            MATRIX_TYPE module_max = module_value(element);
            for (int index = 1; index < length; ++index) {
                MATRIX_TYPE current = module_value(array[index]);
                if (current > module_max) {
                    module_max = current;
                    founded_idx = index;
                }
            }
            return founded_idx;
        }

        case FIRST_EQUAL: {
            for (int index = 0; index < length; ++index) {
                if (equal_values(array[index], element)) return index;  // первый совпавший
            }
            return -1;
        }
    }

    // на случай расширения enum в будущем
    return -1;
}

MATRIX_TYPE *function_(scale_array)(const MATRIX_TYPE *array, const MATRIX_TYPE value, int length) {
    if (!array || length <= 0) return NULL;
    MATRIX_TYPE *new_array = malloc(length * sizeof *new_array);
    if (!new_array) return NULL;
    for (int index = 0; index < length; index++) new_array[index] = array[index] * value;
    return new_array;
}

MATRIX_TYPE *function_(scale_array_inplace)(MATRIX_TYPE *array, const MATRIX_TYPE value, int length) {
    if (!array || length <= 0) return NULL;

    for (int index = 0; index < length; index++) array[index] *= value;
    return array;
}

MATRIX_TYPE *function_(add_array)(const MATRIX_TYPE *array_a, const MATRIX_TYPE *array_b, int length) {
    if (!array_a || !array_b || length <= 0) return NULL;
    MATRIX_TYPE *new_array = malloc(length * sizeof *new_array);
    if (!new_array) return NULL;
    for (int index = 0; index < length; index++) new_array[index] = array_a[index] + array_b[index];
    return new_array;
}

MATRIX_TYPE *function_(sub_array)(const MATRIX_TYPE *array_a, const MATRIX_TYPE *array_b, int length) {
    if (!array_a || !array_b || length <= 0) return NULL;
    MATRIX_TYPE *new_array = malloc(length * sizeof *new_array);
    if (!new_array) return NULL;
    for (int index = 0; index < length; index++) new_array[index] = array_a[index] - array_b[index];
    return new_array;
}

MATRIX_TYPE *function_(add_array_inplace)(MATRIX_TYPE *array_a, const MATRIX_TYPE *array_b, int length) {
    if (!array_a || !array_b || length <= 0) return NULL;

    for (int index = 0; index < length; index++) array_a[index] += array_b[index];
    return array_a;
}

MATRIX_TYPE *function_(sub_array_inplace)(MATRIX_TYPE *array_a, const MATRIX_TYPE *array_b, int length) {
    if (!array_a || !array_b || length <= 0) return NULL;
    for (int index = 0; index < length; index++) array_a[index] -= array_b[index];
    return array_a;
}

MATRIX_TYPE function_(sum_array)(const MATRIX_TYPE *array, int length) {
    if (!array || length <= 0) return 0;
    MATRIX_TYPE sum = 0;
    for (int index = 0; index < length; index++) sum += array[index];
    return sum;
}

MATRIX_TYPE function_(min_array)(const MATRIX_TYPE *array, int length) {
    int index = function_(search_array_index)(FIRST_MIN, array, array[0], length);
    if (index == -1) return 0;

    return array[index];
}

MATRIX_TYPE function_(max_array)(const MATRIX_TYPE *array, int length) {
    int index = function_(search_array_index)(FIRST_MAX, array, array[0], length);
    if (index == -1) return 0;

    return array[index];
}

MATRIX_TYPE function_(dot_arrays)(const MATRIX_TYPE *array_a, const MATRIX_TYPE *array_b, int length) {
    MATRIX_TYPE mult = 0;

    if (!array_a || !array_b || length <= 0) return mult;

    for (int index = 0; index < length; index++) {
        mult += array_a[index] * array_b[index];
    }

    return mult;
}

MATRIX_TYPE *function_(dot_arrays_Hadamard)(const MATRIX_TYPE *array_a, const MATRIX_TYPE *array_b,
                                            int length) {
    if (!array_a || !array_b || length <= 0) return NULL;
    MATRIX_TYPE *new_array = malloc(length * sizeof *new_array);
    if (!new_array) return NULL;
    for (int index = 0; index < length; index++) new_array[index] = array_a[index] * array_b[index];
    return new_array;
}

MATRIX_TYPE **function_(sum_matrix)(const MATRIX_TYPE *const *matrix_A, const MATRIX_TYPE *const *matrix_B,
                                    int rows, int cols) {
    if (!matrix_A || !matrix_B || rows <= 0 || cols <= 0) return NULL;
    MATRIX_TYPE **result_matrix = function_(create_matrix)(rows, cols);
    if (!result_matrix) return NULL;

    for (int row = 0; row < rows; row++) {
        MATRIX_TYPE *temp_row = function_(add_array)(matrix_A[row], matrix_B[row], cols);
        if (!temp_row) {
            function_(free_matrix)(result_matrix);
            return NULL;
        }
        memcpy(result_matrix[row], temp_row, cols * sizeof *temp_row);
        free(temp_row);
    }

    return result_matrix;
}

MATRIX_TYPE **function_(sub_matrix)(const MATRIX_TYPE *const *matrix_A, const MATRIX_TYPE *const *matrix_B,
                                    int rows, int cols) {
    if (!matrix_A || !matrix_B || rows <= 0 || cols <= 0) return NULL;
    MATRIX_TYPE **result_matrix = function_(create_matrix)(rows, cols);
    if (!result_matrix) return NULL;

    for (int row = 0; row < rows; row++) {
        MATRIX_TYPE *temp_row = function_(sub_array)(matrix_A[row], matrix_B[row], cols);
        if (!temp_row) {
            function_(free_matrix)(result_matrix);
            return NULL;
        }
        memcpy(result_matrix[row], temp_row, cols * sizeof *temp_row);
        free(temp_row);
    }

    return result_matrix;
}

MATRIX_TYPE **function_(dot_matrix_Hadamard)(const MATRIX_TYPE *const *matrix_A,
                                             const MATRIX_TYPE *const *matrix_B, int rows, int cols) {
    if (!matrix_A || !matrix_B || rows <= 0 || cols <= 0) return NULL;
    MATRIX_TYPE **result_matrix = function_(create_matrix)(rows, cols);
    if (!result_matrix) return NULL;

    for (int row = 0; row < rows; row++) {
        MATRIX_TYPE *temp_row = function_(dot_arrays_Hadamard)(matrix_A[row], matrix_B[row], cols);
        if (!temp_row) {
            function_(free_matrix)(result_matrix);
            return NULL;
        }
        memcpy(result_matrix[row], temp_row, cols * sizeof *temp_row);
        free(temp_row);
    }

    return result_matrix;
}

MATRIX_TYPE *function_(row_to_array)(const MATRIX_TYPE *const *matrix_origin, MATRIX_TYPE array[],
                                     int row_idx, int cols) {
    if (!array || !matrix_origin) return NULL;

    memcpy(array, matrix_origin[row_idx], cols * sizeof *matrix_origin[row_idx]);

    return array;
}

MATRIX_TYPE *function_(col_to_array)(const MATRIX_TYPE *const *matrix_origin, MATRIX_TYPE array[],
                                     int col_idx, int rows) {
    if (!array || !matrix_origin) return NULL;

    int row = 0;

    for (; row < rows; row++) {
        array[row] = matrix_origin[row][col_idx];
    }

    return array;
}

MATRIX_TYPE **function_(T_matrix)(const MATRIX_TYPE *const *matrix_origin, int rows, int cols) {
    if (!matrix_origin || rows <= 0 || cols <= 0) return NULL;

    int rowsT = cols, colsT = rows;

    MATRIX_TYPE *array = calloc(rows, sizeof *array);
    if (!array) return NULL;
    MATRIX_TYPE **T_matrix = function_(create_matrix)(rowsT, colsT);
    if (!T_matrix)

    {
        free(array);
        return NULL;
    }

    for (int rowT = 0; rowT < rowsT; rowT++) {
        memcpy(T_matrix[rowT], function_(col_to_array)(matrix_origin, array, rowT, rows),
               rows * sizeof *array);
    }

    free(array);
    return T_matrix;
}

MATRIX_TYPE **function_(T_matrix_inplace)(MATRIX_TYPE **matrix_origin, int rows, int cols) {
    if (!matrix_origin || rows <= 0 || cols <= 0) return NULL;

    MATRIX_TYPE **matrix_T = function_(T_matrix)((const MATRIX_TYPE *const *)matrix_origin, rows, cols);

    if (!matrix_T) return matrix_origin;

    function_(free_matrix)(matrix_origin);

    return matrix_T;
}

MATRIX_TYPE **function_(dot_matrix)(const MATRIX_TYPE *const *matrix_A, const MATRIX_TYPE *const *matrix_B,
                                    int rowsA, int colsA, int rowsB, int colsB) {
    if (!matrix_A || !matrix_B || rowsA <= 0 || colsA <= 0 || rowsB <= 0 || colsB <= 0 || colsA != rowsB)
        return NULL;

    int rowsR = rowsA, colsR = colsB, length_array = colsA;

    MATRIX_TYPE **result_matrix = function_(create_matrix)(rowsR, colsR);

    if (!result_matrix) return NULL;

    MATRIX_TYPE **matrix_T = function_(T_matrix)(matrix_B, rowsB, colsB);

    if (!matrix_T) {
        function_(free_matrix)(result_matrix);
        return NULL;
    }

    for (int row = 0; row < rowsR; row++) {
        for (int col = 0; col < colsR; col++) {
            result_matrix[row][col] = function_(dot_arrays)(matrix_A[row], matrix_T[col], length_array);
        }
    }

    function_(free_matrix)(matrix_T);

    return result_matrix;
}

MATRIX_TYPE **function_(copy_matrix)(const MATRIX_TYPE *const *matrix, int rows, int cols) {
    if (!matrix || rows <= 0 || cols <= 0) return NULL;

    MATRIX_TYPE **copy = function_(create_matrix)(rows, cols);
    if (!copy) return NULL;

    for (int row = 0; row < rows; ++row) {
        memcpy(copy[row], matrix[row], cols * sizeof **matrix);
    }
    return copy;
}

MATRIX_TYPE **function_(from_array_to_matrix)(const MATRIX_TYPE array[], int length_array, int rows,
                                              int cols) {
    if (!array || (rows * cols) != length_array || !length_array || rows <= 0 || cols <= 0) return NULL;

    MATRIX_TYPE **matrix = calloc(1, rows * sizeof *matrix + rows * cols * sizeof **matrix);

    if (!matrix) return NULL;

    MATRIX_TYPE *flat_ptr = function_(matrix_to_array)(matrix, rows);

    for (int row = 0; row < rows; row++) matrix[row] = flat_ptr + cols * row;

    memcpy(flat_ptr, array, length_array * sizeof *array);

    return matrix;
}

void function_(insertion_sort)(MATRIX_TYPE array[], int length) {
    for (int index = 1; index < length; index++) {
        MATRIX_TYPE key_element = array[index];
        int left_index = index - 1;

        while (left_index >= 0 && array[left_index] > key_element) {
            array[left_index + 1] = array[left_index];
            left_index--;
        }
        array[left_index + 1] = key_element;
    }
}

static void function_(_quick_sort_core)(MATRIX_TYPE *array, int length) {
    if (length == 2) {
        if (array[0] > array[1]) function_(swap_elements)(&array[0], &array[1]);
        return;
    }

    if (length <= 24) {
        function_(insertion_sort)(array, length);
        return;
    }

    int support_idx = 0;
    MATRIX_TYPE support_element = array[support_idx];
    int right_scanner = length - 1;      // правый сканер
    int left_scanner = support_idx + 1;  // левый сканер

    int seen_less = 0, seen_greater = 0;
    while (right_scanner > left_scanner) {
        while (left_scanner < length && support_element >= array[left_scanner]) {
            if (array[left_scanner] < support_element) seen_less = 1;
            ++left_scanner;
        }
        // границы проверяем первыми — безопасно
        while (right_scanner > 0 && array[right_scanner] > support_element) {
            seen_greater = 1;
            --right_scanner;
        }

        if (right_scanner > left_scanner) {
            function_(swap_elements)(&array[right_scanner], &array[left_scanner]);
            // указатели не инкрементируем здесь: следующий проход сам их подвинет
            // через while
        }
    }

    /* если ни одного < и ни одного > не встретили — подмассив был весь равный
     */
    if (!seen_less && !seen_greater) return;

    // ставим опору на финальное место (последний <= support_element)
    if (support_element > array[right_scanner])
        function_(swap_elements)(&array[support_idx], &array[right_scanner]);

    // независимая рекурсия по левой/правой части
    const int left_length = right_scanner;
    const int right_length = length - right_scanner - 1;

    if (left_length >= 2) function_(_quick_sort_core)(array, left_length);
    if (right_length >= 2) function_(_quick_sort_core)(array + right_scanner + 1, right_length);
}

void function_(_quick_sort_)(MATRIX_TYPE *array, int length) {
    if (length < 2) return;

    int sorted = 1;
    int reversed = 1;

    for (int index = 1; index < length && (sorted || reversed); ++index) {
        if (array[index - 1] > array[index]) sorted = 0;
        if (array[index - 1] < array[index]) reversed = 0;
    }

    if (sorted) return;

    if (reversed) {
        // развернуть и выйти
        for (int left = 0, right = length - 1; left < right; ++left, --right)
            function_(swap_elements)(&array[left], &array[right]);
        return;
    }

    // обычная работа
    function_(_quick_sort_core)(array, length);
}

MATRIX_TYPE **function_(sort_matrix)(MATRIX_TYPE **matrix, int rows, int cols) {
    if (!matrix || rows <= 0 || cols <= 0) return NULL;

    MATRIX_TYPE *array = function_(matrix_to_array)(matrix, rows);
    if (!array) return NULL;

    int array_length = rows * cols;

    MATRIX_TYPE *temp_block_memory = malloc(array_length * sizeof *temp_block_memory);

    if (!temp_block_memory) return NULL;

    array = memcpy(temp_block_memory, array, array_length * sizeof *array);

    function_(_quick_sort_)(array, array_length);

    MATRIX_TYPE **sorted_matrix = function_(from_array_to_matrix)(array, array_length, rows, cols);

    free(temp_block_memory);

    return sorted_matrix;
}

int function_(set_row)(MATRIX_TYPE **matrix, const MATRIX_TYPE array[], int row_idx, int rows, int cols) {
    if (!matrix || !array || (row_idx < 0 || row_idx >= rows) || rows <= 0 || cols <= 0) return -1;

    memmove(matrix[row_idx], array, cols * sizeof *array);

    return row_idx;
}

int function_(set_col)(MATRIX_TYPE **matrix, const MATRIX_TYPE array[], int col_idx, int rows, int cols) {
    if (!matrix || !array || (col_idx < 0 || col_idx >= cols) || rows <= 0 || cols <= 0) return -1;

    for (int row = 0; row < rows; row++) matrix[row][col_idx] = array[row];

    return col_idx;
}

void function_(swap_rows)(MATRIX_TYPE **matrix, int idx_a, int idx_b, int rows) {
    if (!matrix || (idx_a < 0 || idx_a >= rows) || (idx_b < 0 || idx_b >= rows)) return;

    MATRIX_TYPE *temp_row = matrix[idx_a];
    matrix[idx_a] = matrix[idx_b];
    matrix[idx_b] = temp_row;
}

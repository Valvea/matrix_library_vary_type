#include "matrix_core.h"

#include <stdlib.h>
#include <string.h>

/* Возвращает размер элемента для конкретного типа матрицы либо 0, если тип неподдерживаемый */
static size_t element_size_of(MatType mat_type) {
    switch (mat_type) {
        case MAT_F64:
            return sizeof(double);
        case MAT_INT32:
            return sizeof(int);
        default:
            return 0;
    }
}

/* Убеждается, что массив указателей на строки существует; при необходимости выделяет его */
static void **ensure_row_ptrs(Matrix *m) {
    if (!m->row.vp) {
        void **rp = malloc((size_t)m->rows * sizeof(void *));
        if (!rp) return NULL;
        m->row.vp = rp;
    }
    return m->row.vp;
}

/* Возвращает массив указателей на строки для double-матрицы, создавая его при необходимости */
double **mat_rows_f64(Matrix *matrix) {
    if (!matrix || matrix->type != MAT_F64) return NULL;
    // лениво создаём row-пойнтеры поверх плоского буфера (для wrap без make_row_ptrs)
    if (ensure_row_ptrs(matrix)) {
        double *flat_as_double = (double *)matrix->flat_data;
        for (int row = 0; row < matrix->rows; ++row)
            matrix->row.d[row] = flat_as_double + (size_t)row * matrix->cols;
    } else
        return NULL;

    return matrix->row.d;
}

/* Возвращает массив указателей на строки для int-матрицы, создавая его при необходимости */
int **mat_rows_i32(Matrix *matrix) {
    if (!matrix || matrix->type != MAT_INT32) return NULL;
    // лениво создаём row-пойнтеры поверх плоского буфера (для wrap без make_row_ptrs)
    if (ensure_row_ptrs(matrix)) {
        int *flat_as_int = (int *)matrix->flat_data;
        for (int row = 0; row < matrix->rows; ++row)
            matrix->row.i[row] = flat_as_int + (size_t)row * matrix->cols;
    } else
        return NULL;

    return matrix->row.i;
}

/* Освобождает Matrix и, если указано, связанный с ней буфер данных */
void mat_free(Matrix *matrix) {
    if (!matrix) return;
    if (matrix->owns && matrix->block) free(matrix->block);
    if (matrix->row.vp && matrix->row.vp != matrix->block) free(matrix->row.vp);
    free(matrix);
}

/* Выделяет структуру Matrix с единым блоком памяти для row-пойнтеров и плоских данных */
Matrix *mat_new(MatType mat_type, int rows, int cols) {
    if (rows <= 0 || cols <= 0) return NULL;

    size_t element_size = element_size_of(mat_type);
    if (!element_size) return NULL;

    // единый блок: [rows * sizeof(void*)] + [rows*cols * element_size]
    size_t rowptrs_size_bytes = (size_t)rows * sizeof(void *);
    size_t flat_data_size_bytes = (size_t)rows * (size_t)cols * element_size;

    void *block_pointer = calloc(1, rowptrs_size_bytes + flat_data_size_bytes);
    if (!block_pointer) return NULL;

    Matrix *matrix = (Matrix *)calloc(1, sizeof(Matrix));
    if (!matrix) {
        free(block_pointer);
        return NULL;
    }

    matrix->rows = rows;
    matrix->cols = cols;
    matrix->owns = 1;
    matrix->type = mat_type;
    matrix->block = block_pointer;

    matrix->row.vp = NULL;
    matrix->flat_data = (unsigned char *)block_pointer + rowptrs_size_bytes;

    // проставим row-указатели
    switch (mat_type) {
        case MAT_F64: {
            matrix->row.d = mat_rows_f64(matrix);
            if (!matrix->row.d) {
                mat_free(matrix);
                return NULL;
            }
            break;
        }
        case MAT_INT32: {
            matrix->row.i = mat_rows_i32(matrix);
            if (!matrix->row.i) {
                mat_free(matrix);
                return NULL;
            }
            break;
        }
        default:
            break;
    }

    return matrix;
}

/* Оборачивает внешний row-major буфер в Matrix.
 * be_owner=1 заставляет mat_free освобождать переданный flat_data_row_major.
 * Если make_row_ptrs=1, сразу создаются массивы указателей на строки для соответствующего типа. */
Matrix *mat_wrap_buffer(MatType mat_type, void *flat_data_row_major, int be_owner, int rows, int cols,
                        int make_row_ptrs) {
    if (!flat_data_row_major || rows <= 0 || cols <= 0) return NULL;
    size_t element_size = element_size_of(mat_type);
    if (!element_size) return NULL;

    Matrix *matrix = (Matrix *)calloc(1, sizeof(Matrix));
    if (!matrix) return NULL;

    matrix->rows = rows;
    matrix->cols = cols;
    matrix->type = mat_type;
    matrix->owns = be_owner ? 1 : 0;
    matrix->block = be_owner ? flat_data_row_major : NULL;  // чужой буфер — не освобождаем
    matrix->flat_data = flat_data_row_major;
    matrix->row.vp = NULL;

    if (make_row_ptrs) {
        switch (mat_type) {
            case MAT_F64: {
                matrix->row.d = mat_rows_f64(matrix);
                break;
            }
            case MAT_INT32: {
                matrix->row.i = mat_rows_i32(matrix);
                break;
            }
            default:
                break;
        }
    }
    return matrix;
}

/* Глубоко копирует матрицу в новую структуру Matrix */
Matrix *mat_clone(const Matrix *source) {
    if (!source) return NULL;
    Matrix *dest = mat_new(source->type, source->rows, source->cols);
    if (!dest) return NULL;

    size_t element_size = element_size_of(source->type);
    memcpy(dest->flat_data, source->flat_data, (size_t)source->rows * (size_t)source->cols * element_size);
    return dest;
}

/* Копирует данные из source в dest с проверкой размеров и типа */
MatErr mat_copy(const Matrix *source, Matrix *dest) {
    if (!source || !dest) return ME_BAD_ARGS;
    if (source->type != dest->type || source->rows != dest->rows || source->cols != dest->cols)
        return ME_TYPE_MISMATCH;

    size_t element_size = element_size_of(source->type);
    memcpy(dest->flat_data, source->flat_data, (size_t)source->rows * (size_t)source->cols * element_size);
    return ME_OK;
}

/* Возвращает небезопасный указатель на буфер double */
double *mat_data_f64(Matrix *matrix) {
    return (matrix && matrix->type == MAT_F64) ? (double *)matrix->flat_data : NULL;
}
/* Возвращает константный указатель на буфер double */
const double *mat_cdata_f64(const Matrix *matrix) {
    return (matrix && matrix->type == MAT_F64) ? (const double *)matrix->flat_data : NULL;
}
/* Возвращает небезопасный указатель на буфер int */
int *mat_data_i32(Matrix *matrix) {
    return (matrix && matrix->type == MAT_INT32) ? (int *)matrix->flat_data : NULL;
}
/* Возвращает константный указатель на буфер int */
const int *mat_cdata_i32(const Matrix *matrix) {
    return (matrix && matrix->type == MAT_INT32) ? (const int *)matrix->flat_data : NULL;
}

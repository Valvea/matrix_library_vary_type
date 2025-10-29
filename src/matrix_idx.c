#include "matrix_idx.h"

#include <limits.h>

#include "matrix_common.h"
#include "matrix_core.h"

/* Возвращает указатель на элемент матрицы или NULL при нарушении предусловий. */
static void *value_ptr_internal(Idx index) {
    if (!index.ptr || !index.ptr->flat_data) return NULL;
    if (index.ptr->rows <= 0 || index.ptr->cols <= 0) return NULL;
    if (index.row < 0 || index.row >= index.ptr->rows) return NULL;
    if (index.col < 0 || index.col >= index.ptr->cols) return NULL;

    switch (index.ptr->type) {
        case MAT_F64: {
            double **rows = mat_rows_f64(index.ptr);
            return rows ? (void *)&rows[index.row][index.col] : NULL;
        }
        case MAT_INT32: {
            int **rows = mat_rows_i32(index.ptr);
            return rows ? (void *)&rows[index.row][index.col] : NULL;
        }
        default:
            return NULL;
    }
}

/* Возвращает указатель только для чтения. */
const void *read_value_ptr_from_Idx(Idx index) { return value_ptr_internal(index); }

/* Возвращает указатель с правом записи. */
void *get_value_ptr_from_Idx(Idx index) { return value_ptr_internal(index); }

/* Записывает значение из new_value в ptr_data согласно типу матрицы. */
int set_value_from_ptr_Idx(MatType type, void *ptr_data, const void *new_value) {
    if (!ptr_data || !new_value || type == MAT_INVALID) return 0;

    switch (type) {
        case MAT_F64: {
            double *data = (double *)ptr_data;
            *data = *(const double *)new_value;
            return 1;
        }
        case MAT_INT32: {
            int *data = (int *)ptr_data;
            *data = *(const int *)new_value;
            return 1;
        }
        default:
            return 0;
    }
}

/* Выделяет начальную память под Matrix_Idxs. */
int create_mat_indices(Matrix_Idxs *ptr) {
    if (!ptr) return 0;
    size_t capacity = STANDART_IDXS_CAPACITY;
    ptr->indices = calloc(capacity, sizeof(*ptr->indices));
    if (!ptr->indices) {
        ptr->count = 0;
        ptr->capacity = 0;
        return 0;
    }
    ptr->count = 0;
    ptr->capacity = capacity;
    return 1;
}

/* Добавляет индекс в Matrix_Idxs, при необходимости расширяет буфер. */
int push_mat_idx(Matrix_Idxs *ptr, Idx index) {
    if (!ptr || (!ptr->indices && ptr->capacity != 0)) return -1;

    if (ptr->count >= ptr->capacity) {
        size_t new_capacity = ptr->capacity ? 2 * ptr->capacity : STANDART_IDXS_CAPACITY;
        Idx *temp_arr = realloc(ptr->indices, new_capacity * sizeof(*ptr->indices));
        if (!temp_arr) return -1;
        ptr->indices = temp_arr;
        ptr->capacity = new_capacity;
    }
    ptr->indices[ptr->count] = index;
    return (int)ptr->count++;
}

/* Удаляет и возвращает последний индекс из Matrix_Idxs. */
Idx pop_last_mat_idx(Matrix_Idxs *ptr) {
    if (!ptr || !ptr->indices || ptr->count == 0) return (Idx){0};
    return ptr->indices[--ptr->count];
}

/* Возвращает индекс по позиции i и уменьшает счётчик без сжатия. */
Idx pop_mat_idx(Matrix_Idxs *ptr, size_t i) {
    if (!ptr || !ptr->indices || ptr->count == 0 || i >= ptr->count) return (Idx){0};
    ptr->count--;
    return ptr->indices[i];
}

/* Возвращает количество добавленных индексов (последняя колонка входит, -1 при ошибке). */
int mat_slice(Matrix *m, Idx from, Idx to, Matrix_Idxs *slices) {
    if (!slices || !m || !m->flat_data || !m->rows || !m->cols) return -1;

    Matrix *matrix = m;

    from.row = ((from.row % matrix->rows) + matrix->rows) % matrix->rows;
    to.row = ((to.row % matrix->rows) + matrix->rows) % matrix->rows;

    from.col = ((from.col % matrix->cols) + matrix->cols) % matrix->cols;
    to.col = ((to.col % matrix->cols) + matrix->cols) % matrix->cols;

    int start_row = from.row < to.row ? from.row : to.row;
    int end_row = to.row > from.row ? to.row : from.row;

    int start_col = from.col < to.col ? from.col : to.col;
    int end_col = start_col == from.col ? to.col : from.col;

    size_t start_idxs = slices->count;

    for (int row = start_row; row <= end_row; row++) {
        for (int col = start_col; col <= end_col; col++) {
            if (push_mat_idx(slices, (Idx){.row = row, .col = col, .ptr = matrix}) < 0) {
                slices->count = start_idxs;
                return -1;
            }
        }
    }

    size_t produced = slices->count - start_idxs;
    if (produced > INT_MAX) {
        slices->count = start_idxs;
        return -1;
    }

    return (int)produced;
}

/* Проверяет, выполняется ли одиночное условие Condition для заданного значения. */
static int OP_search_implement(MatType matrix_type, const void *matrix_value, const Condition *condition) {
    if (!condition || !matrix_value) return -1;

    switch (matrix_type) {
        case MAT_F64: {
            if (condition->type_value != VAL_DOUBLE) return -1;
            double val = *(const double *)matrix_value;
            double ref = condition->value.as_double;

            switch (condition->kind) {
                case OP_LESS:
                    return val < ref;
                case OP_ABOVE:
                    return val > ref;
                case OP_EQUAL:
                    return equal_type_double(val, ref);
                case OP_NOT_EQUAL:
                    return !equal_type_double(val, ref);
                case OP_LESS_EQUAL:
                    return (val < ref) || equal_type_double(val, ref);
                case OP_ABOVE_EQUAL:
                    return (val > ref) || equal_type_double(val, ref);
                    ;
            }

            return -1;  // неизвестная операция
        }

        case MAT_INT32: {
            if (condition->type_value != VAL_INT) return -1;
            int val = *(const int *)matrix_value;
            int ref = condition->value.as_int;

            switch (condition->kind) {
                case OP_LESS:
                    return val < ref;
                case OP_ABOVE:
                    return val > ref;
                case OP_EQUAL:
                    return val == ref;
                case OP_NOT_EQUAL:
                    return val != ref;
                case OP_LESS_EQUAL:
                    return val <= ref;
                case OP_ABOVE_EQUAL:
                    return val >= ref;
            }

            return -1;  // неизвестная операция
        }

        default:
            return -1;
    }
}

/* Проверяет одно значение матрицы на соответствие набору условий. */
static int validate_clause(MatType matrix_type, const void *matrix_value, const Clause *clause) {
    if (!clause || clause->mode == NO_MODE || clause->count == 0) return -1;

    switch (clause->mode) {
        case MATCH_ALL: {
            for (size_t idx = 0; idx < clause->count; idx++) {
                int r = OP_search_implement(matrix_type, matrix_value, &clause->conditions[idx]);
                if (r == -1) return -1;  // ошибка — пробрасываем наверх
                if (r == 0) return 0;    // одно условие не прошло — весь AND не прошёл
            }
            return 1;
        }
        case MATCH_ANY: {
            for (size_t idx = 0; idx < clause->count; idx++) {
                int r = OP_search_implement(matrix_type, matrix_value, &clause->conditions[idx]);
                if (r == -1) return -1;  // ошибка — пробрасываем наверх
                if (r == 1) return 1;    // любое прошло — OR прошёл
            }
            return 0;
        }
        default:
            return -1;
    }
}

/* Проверяет наборы условий. */
static int final_decision(MatType matrix_type, const void *matrix_value, const Query *input) {
    switch (input->mode) {
        case MATCH_ALL: {
            for (size_t idx = 0; idx < input->count; idx++) {
                int r = validate_clause(matrix_type, matrix_value, &input->clauses[idx]);
                if (r == -1) return -1;  // ошибка изнутри
                if (r == 0) return 0;    // одна клауза не прошла — весь AND не прошёл
            }
            return 1;
        }
        case MATCH_ANY:
        case NO_MODE: {
            for (size_t idx = 0; idx < input->count; idx++) {
                int r = validate_clause(matrix_type, matrix_value, &input->clauses[idx]);
                if (r == -1) return -1;
                if (r == 1) return 1;
            }
            return 0;
        }
        default:
            return -1;
    }
}

/* Собирает индексы элементов, удовлетворяющих запросу, и возвращает их количество (-1 при ошибке). */
int mat_where(Matrix *matrix, Matrix_Idxs *indices, const Query *query_in) {
    if (!matrix || !indices || !query_in || !matrix->flat_data) return -1;
    if (matrix->rows <= 0 || matrix->cols <= 0) return -1;
    if (query_in->count == 0) return -1;
    if (query_in->count > MAX_CLAUSES_PER_QUERY) return -1;
    for (size_t i = 0; i < query_in->count; ++i) {
        if (query_in->clauses[i].mode == NO_MODE || query_in->clauses[i].count == 0) return -1;
        if (query_in->clauses[i].count > MAX_CONDS_PER_CLAUSE) return -1;
    }

    size_t start_count = indices->count;
    Idx idx = {.ptr = matrix};

    for (int row = 0; row < matrix->rows; row++) {
        for (int col = 0; col < matrix->cols; col++) {
            idx.row = row;
            idx.col = col;
            int result = final_decision(matrix->type, read_value_ptr_from_Idx(idx), query_in);
            if (result == -1) {
                indices->count = start_count;
                return -1;
            }
            if (result) {
                int pushed = push_mat_idx(indices, idx);
                if (pushed == -1) {
                    indices->count = start_count;
                    return -1;
                }
            }
        }
    }

    return (int)(indices->count - start_count);
}

Query create_Query(const Clause *clauses, size_t clause_count, MatchMode mode) {
    Query query = {.mode = mode, .count = 0};
    if (!clauses || clause_count == 0) {
        return query;  // пустой запрос
    }

    size_t limit = clause_count < MAX_CLAUSES_PER_QUERY ? clause_count : MAX_CLAUSES_PER_QUERY;
    for (size_t i = 0; i < limit; ++i) {
        query.clauses[i] = clauses[i];
    }
    query.count = limit;
    return query;
}

/* Сбрасывает логический размер, не освобождая память. */
void clear_mat_indices(Matrix_Idxs *ptr) {
    if (ptr && ptr->indices) ptr->count = 0;
}

/* Полностью освобождает память, занятую Matrix_Idxs. */
void free_mat_indices(Matrix_Idxs *ptr) {
    if (!ptr) return;
    free(ptr->indices);
    ptr->indices = NULL;
    ptr->count = 0;
    ptr->capacity = 0;
}

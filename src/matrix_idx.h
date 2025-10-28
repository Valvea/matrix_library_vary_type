#ifndef MATRIX_IDX_H
#define MATRIX_IDX_H

#include <stddef.h>

#include "matrix_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Стартовая вместимость массива индексов. */
#define STANDART_IDXS_CAPACITY 100

typedef struct {
    /* Позиция в матрице; отрицательные значения нормализуются позднее. */
    int row;
    int col;
    Matrix *ptr;
} Idx;

typedef struct {
    /* Динамический массив индексов, собранных утилитами. */
    Idx *indices;
    size_t count;
    size_t capacity;
} Matrix_Idxs;

/* Инициализирует контейнер под хранение индексов. */
int create_mat_indices(Matrix_Idxs *ptr);

/* Добавляет новый индекс, при нехватке памяти расширяет массив. */
int push_mat_idx(Matrix_Idxs *ptr, Idx index);

/* Удаляет и возвращает последний индекс (или пустой Idx при ошибке). */
Idx pop_last_mat_idx(Matrix_Idxs *ptr);

/* Возвращает индекс по позиции i и уменьшает счётчик без сжатия массива. */
Idx pop_mat_idx(Matrix_Idxs *ptr, size_t i);

/* Обнуляет счётчик элементов, но сохраняет память. */
void clear_mat_indices(Matrix_Idxs *ptr);

/* Освобождает все ресурсы контейнера. */
void free_mat_indices(Matrix_Idxs *ptr);

/* Добавляет в slices прямоугольный срез между двумя координатами (последняя колонка входит, SIZE_MAX при ошибке). */
size_t mat_slice(Idx from, Idx to, Matrix_Idxs *slices);

/* Возвращает изменяемый указатель на элемент матрицы. */
void *get_value_ptr_from_Idx(Idx index);

/* Возвращает константный указатель на элемент матрицы. */
const void *read_value_ptr_from_Idx(Idx index);

/* Записывает значение new_value в ptr_data согласно типу матрицы. */
int set_value_from_ptr_Idx(MatType type, void *ptr_data, const void *new_value);

#ifdef __cplusplus
}
#endif

#endif

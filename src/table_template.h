#ifndef TABLE_TEMPLATE_H
#define TABLE_TEMPLATE_H

#ifndef TABLE_TYPE
#error "Определи MATRIX_TYPE перед включением matrix_table_template.h"
#endif
#ifndef TABLE_TAG
#error "Определи MATRIX_TAG (double, int, char, ...) перед включением matrix_table_template.h"
#endif

#define TABLE_CAT(a, b) TABLE_CAT_(a, b)
#define TABLE_CAT_(a, b) a##_##b
#define table_function(name) TABLE_CAT(name, TABLE_TAG)

#define FORMAT_OF(table_value) _Generic((table_value), int: "%d", double: "%f", char: "%c", default: "%s")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Символьные константы допустимых разделителей строк/элементов. */
typedef enum {
    ROW_DELIM_SPACE = ' ',
    ROW_DELIM_TAB = '\t',
    ROW_DELIM_NEWLINE = '\n',
    ROW_DELIM_COMMA = ',',
    ROW_DELIM_SEMICOLON = ';',
    ROW_DELIM_PIPE = '|',
} Delimiters;

/* Коды состояния для операций ввода/вывода таблиц. */
typedef enum {
    NORMAL,
    ERROR_FILE_READ,
    INVALID_SPECIEF,
    ERROR_FILE_WRITE,
    ERROR_MEM_ALLOC,
    FILE_EMPTY
} ProcssState;

/* Тип считываемой таблицы */
typedef enum {
    Read_NONE,
    Read_double,
    Read_long,
    Read_int,
    Read_char,
} Readtype;

/* Результат чтения таблицы из файла. */
typedef struct read_data {
    ProcssState status;         /* Итог операции. */
    void *flat_data;            /* Указатель на плоский буфер значений. */
    size_t capacity;            /* Выделенная ёмкость буфера. */
    size_t elems_fl_data_count; /* Количество фактически считанных элементов. */
    size_t rows_count;          /* Количество фактически пройденных строк. */
    Readtype type;              /* Cчитываемый тип. */
} read_data;

/* Разделители, воспринимаемые как границы между числами при вводе. */
extern const char DELIMITERS[];
/* Допустимые спецификаторы printf-подобных форматов. */
extern const char SPECIFIERS[];
/* Текущий символ-разделитель внутри строки. */
extern char NUMBER_DELIMETER[];
/* Текущий символ-разделитель строк. */
extern char ROW_DELIMETER[];

/* Установить/получить символ-разделитель для чисел. */
static inline void set_number_delimiter(Delimiters delim) { NUMBER_DELIMETER[0] = (char)delim; }
static inline char number_delimiter(void) { return NUMBER_DELIMETER[0]; }
/* Установить/получить символ-разделитель строк. */
static inline void set_row_delimiter(Delimiters delim) { ROW_DELIMETER[0] = (char)delim; }
static inline char row_delimiter(void) { return ROW_DELIMETER[0]; }

/* Считывание таблицы из файла в плоском представлении. */
read_data read_table_from_file(const char *filename, Readtype scan_type);

/* Запись таблицы в файл с контролем допустимости формата. */
ProcssState write_table_to_file(const char *filename, const char *format, void *const table_data, int rows,
                                int cols);

static TABLE_TYPE **table_function(create)(int rows, int cols) {
    if (rows <= 0 || cols <= 0) return NULL;

    size_t row_ptr_bytes = (size_t)rows * sizeof(TABLE_TYPE *);
    size_t data_bytes = (size_t)rows * cols * sizeof(TABLE_TYPE);

    TABLE_TYPE **matrix = malloc(row_ptr_bytes + data_bytes);
    if (!matrix) return NULL;

    TABLE_TYPE *data = (TABLE_TYPE *)(matrix + rows);
    for (int r = 0; r < rows; ++r) {
        matrix[r] = data + r * cols;
    }
    memset(data, 0, data_bytes);
    return matrix;
}

static void table_function(free)(TABLE_TYPE **matrix) { free(matrix); }

static TABLE_TYPE **table_function(clone)(TABLE_TYPE **src, int rows, int cols) {
    if (!src) return NULL;
    TABLE_TYPE **copy = table_function(create)(rows, cols);
    if (!copy) return NULL;
    for (int r = 0; r < rows; ++r) {
        memcpy(copy[r], src[r], (size_t)cols * sizeof(TABLE_TYPE));
    }
    return copy;
}

/* Печатает одну строку таблицы с разделителем NUMBER_DELIMETER */
static void output_row(const MATRIX_TYPE *row, int cols) {
    if (cols <= 0) return;
    for (int col = 0; col < cols - 1; col++) printf(FORMAT_OF(row[col]) "%c", row[col], number_delimiter());
    printf(FORMAT_OF(row[cols - 1]), row[cols - 1]);
}

/* Печатает nf,kbwe построчно */
static void table_function(print_matrix)(const TABLE_TYPE *const *table, int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        output_row(table[row], cols);
        printf("%c", row_delimiter());
    }
}

/* Создает матрицу и заполняет данными из плоского массива длиной rows*cols */
TABLE_TYPE **table_function(from_flat_to_table)(const TABLE_TYPE array[], size_t length_array, int rows,
                                                int cols) {
    if (!array || (size_t)rows * (size_t)cols != length_array || !length_array || rows <= 0 || cols <= 0)
        return NULL;
    TABLE_TYPE **table = table_function(create)(rows, cols);

    if (!table) return NULL;

    TABLE_TYPE *flat_ptr = (TABLE_TYPE *)(table + rows);

    memcpy(flat_ptr, array, length_array * sizeof *array);

    return table;
}

typedef int (*rank_fn)(TABLE_TYPE);

static int table_function(fun_cmp)(const void *first, const void *second, const rank_fn fun[],
                                   size_t fun_count) {
    TABLE_TYPE *a = first;
    TABLE_TYPE *b = second;

    unsigned pa = 0, pb = 0;
    for (size_t i = 0, weight = fun_count; i < fun_count; ++i, --weight) {
        pa += fun[i](*a) ? weight : 0;
        pb += fun[i](*b) ? weight : 0;
    }

    int cmp = (pa > pb) - (pa < pb);
    return cmp ? cmp : (*a > *b) - (*a < *b);
}

/* Классическая сортировка вставками: хороша для маленьких диапазонов */
void insertion_sort(TABLE_TYPE *array, int length,
                    int (*fun_cmp)(const void *first, const void *second,const rank_fn fun[],
                                   size_t fun_count)) {
    for (int index = 1; index < length; index++) {
        TABLE_TYPE key_element = array[index]; /* Элемент, который будем вставлять */
        int left_index = index - 1;

        /* Сдвигаем элементы вправо, пока не найдём позицию для key_element */
        while (left_index >= 0 && fun_cmp(&array[left_index], &key_element,fun,fun_count) > 0) {
            array[left_index + 1] = array[left_index];
            left_index--;
        }
        array[left_index + 1] = key_element; /* Вставка элемента на найденное место */
    }
}

#endif /* TABLE_TEMPLATE_H */
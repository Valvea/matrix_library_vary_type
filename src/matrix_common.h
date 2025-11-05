// matrix_common.h — общие определения и вспомогательные утилиты для работы с матрицами.
#ifndef MATRIX_COMMON_H
#define MATRIX_COMMON_H

#include <math.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MATRIX_EPSILON
#define MATRIX_EPSILON 1e-12 /* Допуск для сравнения чисел с плавающей точкой */
#endif

#ifndef MATRIX_EPSILON_REL
#define MATRIX_EPSILON_REL 1e-9
#endif


/* Стратегии поиска индекса в массиве. */
typedef enum {
    FIRST_LESS, /* Первый элемент, который строго меньше заданного значения. */
    FIRST_ABOVE, /* Первый элемент, который строго больше заданного значения. */
    FIRST_EQUAL, /* Первый элемент, который равен опорному значению. */
    MODULE_ABOVE, /* Элемент с максимальным модулем. */
    ABSOLUTE_MAX, /* Индекс абсолютного максимума. */
    ABSOLUTE_MIN, /* Индекс абсолютного минимума. */

} search_element;

/* Символьные константы допустимых разделителей строк/элементов. */
typedef enum {
    ROW_DELIM_SPACE = ' ',
    ROW_DELIM_TAB = '\t',
    ROW_DELIM_NEWLINE = '\n',
    ROW_DELIM_COMMA = ',',
    ROW_DELIM_SEMICOLON = ';',
    ROW_DELIM_PIPE = '|',
} RowDelimiters;

/* Коды состояния для операций ввода/вывода матриц. */
typedef enum {
    NORMAL,
    ERROR_FILE_READ,
    INVALID_SPECIEF,
    ERROR_FILE_WRITE,
    ERROR_MEM_ALLOC,
    FILE_EMPTY
} ProcssState;



/* Тип считываемой матрицы */
typedef enum {
    Read_NONE,
    Read_double ,
    Read_long,
    Read_int,
    Read_char,
} Readtype;



/* Результат чтения матрицы из файла. */
typedef struct read_data {
    ProcssState status;      /* Итог операции. */
    void *flat_data;       /* Указатель на плоский буфер значений. */
    size_t capacity;         /* Выделенная ёмкость буфера. */
    int elems_fl_data_count; /* Количество фактически считанных элементов. */
    Readtype type;           /* Cчитываемый тип. */
} read_data;

/* Разделители, воспринимаемые как границы между числами при вводе. */
extern const char ROW_DELIMITERS[];
/* Допустимые спецификаторы printf-подобных форматов. */
extern const char SPECIFIERS[];
/* Текущий символ-разделитель внутри строки. */
extern char NUMBER_DELIMETER[];
/* Текущий символ-разделитель строк. */
extern char ROW_DELIMETER[];

/* Установить/получить символ-разделитель для чисел. */
static inline void set_number_delimiter(RowDelimiters delim) { NUMBER_DELIMETER[0] = (char)delim; }
static inline char number_delimiter(void) { return NUMBER_DELIMETER[0]; }
/* Установить/получить символ-разделитель строк. */
static inline void set_row_delimiter(RowDelimiters delim) { ROW_DELIMETER[0] = (char)delim; }
static inline char row_delimiter(void) { return ROW_DELIMETER[0]; }

/* Считывание матрицы из файла в плоском представлении. */
read_data read_matrix_from_file(const char *filename, Readtype scan_type);

/* Запись матрицы в файл с контролем допустимости формата. */
ProcssState write_matrix_to_file(const char *filename, const char *format, void *const matrix_data, int rows,
                                 int cols);

/* Универсальная обёртка, возвращающая модуль значения произвольного числового типа */
#define module_value(value) \
    _Generic((value),       \
        int: abs,           \
        long: labs,         \
        long long: llabs,   \
        float: fabsf,       \
        double: fabs,       \
        long double: fabsl, \
        default: fabs)((value))

/* Сравнение целых чисел без допуска. */
static inline int equal_type_int(long long first, long long second) { return first == second; }
/* Сравнение double с учётом MATRIX_EPSILON и MATRIX_EPSILON_REL */
static inline int equal_type_double(double first, double second) {
    if (isnan(first) || isnan(second)) return 0;
    if (isinf(first) || isinf(second)) return first == second;  // +inf==+inf, -inf==-inf

    double diff = module_value(first - second);
    double scale = fmax(1.0, fmax(module_value(first), module_value(second)));
    return diff <= fmax(MATRIX_EPSILON, MATRIX_EPSILON_REL * scale);
}
/* Универсальное сравнение для целых и вещественных значений. */
#define equal_values(first, second) \
    _Generic(((first) + 0), double: equal_type_double, default: equal_type_int)((first), (second))

#ifdef __cplusplus
}
#endif

#endif  // MATRIX_COMMON_H

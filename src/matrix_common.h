// matrix_common.h
#ifndef MATRIX_COMMON_H
#define MATRIX_COMMON_H

#include <math.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ROW_DELIMITERS " \t\n,;|"  /* Разделители, которые трактуются как границы между числами при вводе */
#define NEW_LINE '\n'              /* Символ переноса строки для вывода матриц */
#define OUTPUT_DELIMITER ' '       /* Пробел между элементами строки матрицы */

#ifndef MATRIX_EPSILON
#define MATRIX_EPSILON 1e-12       /* Допуск для сравнения чисел с плавающей точкой */
#endif

/* Тип стратегии поиска индекса в массиве */
typedef enum {
  FIRST_MIN,   /* Первый элемент, который строго меньше заданного значения */
  FIRST_MAX,   /* Первый элемент, который строго больше заданного значения */
  FIRST_EQUAL, /* Первый элемент, который равен заданному значению */
  MODULE_MAX   /* Элемент с максимальным модулем */
} search_element;

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

/* Сравнение целых чисел без допуска */
static inline int equal_type_int(long long first, long long second) { return first == second; }
/* Сравнение double с учётом MATRIX_EPSILON */
static inline int equal_type_double(double first, double second) {
    return module_value(first - second) < MATRIX_EPSILON;
}
/* Универсальное сравнение для целых и вещественных значений */
#define equal_values(first, second) \
    _Generic(((first) + 0), double: equal_type_double, default: equal_type_int)((first), (second))

#ifdef __cplusplus
}
#endif

#endif  // MATRIX_COMMON_H

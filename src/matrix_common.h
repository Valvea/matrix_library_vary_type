// matrix_common.h
#ifndef MATRIX_COMMON_H
#define MATRIX_COMMON_H

#include <math.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ROW_DELIMITERS " \t\n,;|"
#define NEW_LINE '\n'
#define OUTPUT_DELIMITER ' '

#ifndef MATRIX_EPSILON
#define MATRIX_EPSILON 1e-12
#endif

typedef enum { FIRST_MIN, FIRST_MAX, FIRST_EQUAL, MODULE_MAX } search_element;

#define module_value(value) \
    _Generic((value),       \
        int: abs,           \
        long: labs,         \
        long long: llabs,   \
        float: fabsf,       \
        double: fabs,       \
        long double: fabsl, \
        default: fabs)((value))

static inline int equal_type_int(long long first, long long second) { return first == second; }
static inline int equal_type_double(double first, double second) {
    return module_value(first - second) < MATRIX_EPSILON;
}
#define equal_values(first, second) \
    _Generic(((first) + 0), double: equal_type_double, default: equal_type_int)((first), (second))

#ifdef __cplusplus
}
#endif

#endif  // MATRIX_COMMON_H

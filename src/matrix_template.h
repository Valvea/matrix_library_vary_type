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
#define MATRIX_CAT_(a, b) a##_##b
#define function_(name) MATRIX_CAT_(name, MATRIX_TAG)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

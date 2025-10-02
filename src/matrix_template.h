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

#define ROW_DELIMETER ' '
#define NEW_LINE '\n'

static int function_(input)(MATRIX_TYPE *row, int cols) {
  MATRIX_TYPE temp = 0;
  int i = 0;
  char del = ' ';
  while (i < cols && scanf(MATRIX_SCANF "%c", &temp, &del) == 2 &&
         (del == ROW_DELIMETER || del == NEW_LINE)) {
    row[i++] = temp;
  }
  return i;
}

MATRIX_TYPE **function_(create_matrix)(int rows, int cols) {
  // единый блок: [rows указателей][rows*cols элементов]
  MATRIX_TYPE **matrix =
      calloc(1, rows * sizeof *matrix + rows * cols * sizeof **matrix);
  if (!matrix)
    return NULL;
  MATRIX_TYPE *row0 = (MATRIX_TYPE *)(matrix + rows);
  for (int row = 0; row < rows; row++)
    matrix[row] = row0 + row * cols;
  return matrix;
}

int function_(fillmatrix)(MATRIX_TYPE **matrix, int rows, int cols) {
  for (int r = 0; r < rows; r++)
    if (function_(input)(matrix[r], cols) != cols)
      return 0;
  return 1;
}

static void function_(output)(const MATRIX_TYPE *row, int cols) {
  for (int col = 0; col < cols - 1; col++)
    printf(MATRIX_PRINTF "%c", row[col], ROW_DELIMETER);
  printf(MATRIX_PRINTF, row[cols - 1]);
}

void function_(printmatrix)(const MATRIX_TYPE *const *matrix, int rows,
                            int cols) {
  for (int row = 0; row < rows; row++) {
    function_(output)(matrix[row], cols);
    printf("%c", NEW_LINE);
  }
}

MATRIX_TYPE function_(sumarray)(const MATRIX_TYPE *array, int length) {
  if (!array || length <= 0)
    return 0;
  MATRIX_TYPE sum = 0;
  for (int index = 0; index < length; index++)
    sum += array[index];
  return sum;
}

MATRIX_TYPE function_(minarray)(const MATRIX_TYPE *array, int length) {
  if (!array || length <= 0)
    return 0;
  MATRIX_TYPE min = array[0];
  for (int index = 1; index < length; index++)
    if (array[i] < min)
      min = array[index];
  return min;
}

MATRIX_TYPE function_(maxarray)(const MATRIX_TYPE *array, int length) {
  if (!array || length <= 0)
    return 0;
  MATRIX_TYPE max = array[0];
  for (int index = 1; index < length; i++)
    if (array[index] > max)
      max = array[index];
  return max;
}

MATRIX_TYPE **function_(sum_matrix)(const MATRIX_TYPE *const *matrix_A,
                                    const MATRIX_TYPE *const *matrix_B,
                                    int rows, int cols) {

  MATRIX_TYPE **result_matrix = function_(create_matrix)(rows, cols);
  if (!result_matrix)
    return NULL;

  for (int row = 0; row < rows; row++) {

    for (int col = 0; col < cols; col++) {

      result_matrix[row][col] = matrix_A[row][col] + matrix_B[row][col];
    }
  }

  return result_matrix;
}




MATRIX_TYPE *function_(col_to_array)(const MATRIX_TYPE * const *matrix_origin, MATRIX_TYPE array[], int rows, int col_idx) {

  if (!array || !matrix_origin)
    return NULL;

  int row = 0;

  for (; row < rows; row++) {

    array[row] = matrix_origin[row][col_idx];
  }

  return array;
}


int *row_to_array(int **matrix_origin, int array[], int row_idx, int cols) {
  if (!array || !matrix_origin)
    return NULL;

  int col = 0;

  for (; col < cols; col++) {

    array[col] = matrix_origin[row_idx][col];
  }

  return array;
}





MATRIX_TYPE **function_(T_matrix)(const MATRIX_TYPE *const *matrix_origin,
                                  int rows, int cols)
{

  int rowsT = cols, colsT = rows;

  MATRIX_TYPE *array = calloc(rows, sizeof *array);
  if (!array)
    return NULL;
  MATRIX_TYPE **T_matrix = function_(create_matrix)(rowsT, colsT);
  if (!T_matrix)

  {
    free(array);
    return NULL;
  }

  for (int rowT = 0; rowT < rowsT; rowT++) {

    memcpy(T_matrix[rowT], function_(col_to_array)(matrix_origin, array, rows, rowT),
           rows * sizeof *T_matrix[rowT]);
  }

  free(array);
  return T_matrix;
}



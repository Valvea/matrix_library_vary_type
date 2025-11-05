#include "matrix_common.h"

#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Символьные константы десятичных цифр ASCII. */
typedef enum {
    NUM_CHAR_ZERO = '0',
    NUM_CHAR_ONE = '1',
    NUM_CHAR_TWO = '2',
    NUM_CHAR_THREE = '3',
    NUM_CHAR_FOUR = '4',
    NUM_CHAR_FIVE = '5',
    NUM_CHAR_SIX = '6',
    NUM_CHAR_SEVEN = '7',
    NUM_CHAR_EIGHT = '8',
    NUM_CHAR_NINE = '9',
} NumberSymbols;

/* Разрешённые цифровые символы для валидации строки. */
static const char ALLOWED_NUMBER_SYMBOLS[] = {NUM_CHAR_ZERO,  NUM_CHAR_ONE,  NUM_CHAR_TWO, NUM_CHAR_THREE,
                                              NUM_CHAR_FOUR,  NUM_CHAR_FIVE, NUM_CHAR_SIX, NUM_CHAR_SEVEN,
                                              NUM_CHAR_EIGHT, NUM_CHAR_NINE, '\0'};

/* Набор разделителей, которыми могут отделяться числа. */
const char ROW_DELIMITERS[] = {
    ROW_DELIM_SPACE, ROW_DELIM_TAB, ROW_DELIM_NEWLINE, ROW_DELIM_COMMA, ROW_DELIM_SEMICOLON,
    ROW_DELIM_PIPE,  '\0'};

/* Допустимые спецификаторы числового формата при сериализации. */
const char SPECIFIERS[] = "difFeEgG";
/* Текущий символ-разделитель внутри строки. */
char NUMBER_DELIMETER[] = {ROW_DELIM_SPACE, '\0'};
/* Текущий разделитель строк. */
char ROW_DELIMETER[] = {ROW_DELIM_NEWLINE, '\0'};

/* Проверяет, содержит ли строка допустимые символы числовой записи или разделители. */
static int valid_row(const char *str) {
    size_t length = 0;
    if (!str || !(length = strlen(str))) return length;
    if (length == 1) {
        return (strcspn(str, ALLOWED_NUMBER_SYMBOLS) != length || strcspn(str, ROW_DELIMITERS) != length)
                   ? length
                   : 0;
    }
    return (strcspn(str, ALLOWED_NUMBER_SYMBOLS) != length && strcspn(str, ROW_DELIMITERS) != length) ? length
                                                                                                      : 0;
}

/* Локальный аналог strtok, безопасный для вложенных разборов через отдельный save-указатель. */
static char *strtok_portable(char *str_src, const char *delim, char **save_str) {
    if (!delim || *delim == '\0') return NULL;

    char *str = str_src ? str_src : *save_str;

    if (!str || *str == '\0') return NULL;

    while (strchr(delim, *str) && *str != '\0') str++;

    size_t idx_delim = strcspn(str, delim);

    while (str[idx_delim] != '\0' && strchr(delim, str[idx_delim])) {
        str[idx_delim++] = '\0';
    }
    *save_str = str[idx_delim] == '\0' ? NULL : &str[idx_delim];

    return *str ? str : NULL;
}

/* Превращает текст с числами в динамически расширяемый буфер double, отмечая ошибки NaN. */
static int str_to_dnumber(char *str, double **buffer, size_t *buff_cup) {
    int elements = 0;
    char *next_double_ptr = NULL;
    errno = 0;
    char *save_row;
    char *row = strtok_portable(str, ROW_DELIMETER, &save_row);
    double value = NAN;

    while (row) {
        /* Обрабатываем только строки, содержащие числа. */
        if (valid_row(row)) {
            char *save_sub_row = NULL;
            char *sub_row = strtok_portable(row, ROW_DELIMITERS, &save_sub_row);
            while (sub_row) {
                errno = 0;
                value = strtod(sub_row, &next_double_ptr);
                if ((sub_row == next_double_ptr) || (*next_double_ptr != '\0') || errno == ERANGE) {
                    (*buffer)[elements++] = NAN;
                } else {
                    (*buffer)[elements++] = value;
                }
                /* При необходимости растягиваем буфер вдвое. */
                if ((size_t)elements >= *buff_cup) {
                    double *temp = realloc(*buffer, 2 * (*buff_cup) * sizeof(**buffer));
                    if (!temp) return elements;
                    (*buff_cup) *= 2;
                    *buffer = temp;
                }
                sub_row = strtok_portable(NULL, ROW_DELIMITERS, &save_sub_row);
            }
        }

        row = strtok_portable(NULL, ROW_DELIMETER, &save_row);
    }

    return elements;
}

/* При необходимости растягиваем буфер вдвое. */
static void *resize_buffer(void *buff_ptr, size_t *old_size, size_t add) {
    void *temp = realloc(buff_ptr, 2 * ( *old_size ? *old_size: 256)+add);
    if (!temp) return NULL;
    buff_ptr = temp;
    (*old_size) *= 2;

    return buff_ptr;
}

/* Превращает текст с числами в динамически расширяемый буфер */
static size_t read_text(char *str, unsigned char **buffer, size_t *buff_cup, Readtype scan_type) {
    size_t scaned_elems = 0;
    size_t elem_size = 0;
    switch (scan_type) {
        case Read_char:
            elem_size = sizeof(char);
            break;
        case Read_double:
            elem_size = sizeof(double);
            break;
        case Read_long:
            elem_size = sizeof(long);
            break;
        case Read_int:
            elem_size = sizeof(int);
            break;
        default:
            break;
    }
    if (!elem_size) return scaned_elems;
    char *next_double_ptr = NULL;
    errno = 0;
    char *save_row;
    char *row = strtok_portable(str, ROW_DELIMETER, &save_row);

    while (row) {
        if (scan_type == Read_char) {
            size_t new_row = strlen(row) + 1;
            if (*buff_cup <= (new_row + scaned_elems) * elem_size) {
                *buffer = resize_buffer(*buffer, buff_cup,new_row);
                if (!*buffer) return scaned_elems;
            }
            unsigned char *ptr_buffer = *buffer;
            memcpy(ptr_buffer + scaned_elems * elem_size, row, new_row);
            scaned_elems+=new_row;

        } else {
            /* Обрабатываем только строки, содержащие числа. */
            if (valid_row(row)) {
                char *save_sub_row = NULL;
                char *sub_row = strtok_portable(row, ROW_DELIMITERS, &save_sub_row);
                while (sub_row) {
                    errno = 0;
                    if (*buff_cup <= (scaned_elems+1) * elem_size) {
                        *buffer = resize_buffer(*buffer, buff_cup,elem_size);
                        if (!*buffer) return scaned_elems;
                    }
                    unsigned char *ptr_buffer = *buffer;
                    switch (scan_type) {
                        case Read_double: {
                            double value_temp = strtod(sub_row, &next_double_ptr);
                            if ((sub_row == next_double_ptr) || (*next_double_ptr != '\0') ||
                                errno == ERANGE) {
                                value_temp = NAN;
                            }
                            memcpy(ptr_buffer + (scaned_elems++) * elem_size, &value_temp, elem_size);
                        } break;
                        case Read_long: {
                            long value_temp = strtol(sub_row, NULL, 10);
                            memcpy(ptr_buffer + (scaned_elems++) * elem_size, &value_temp, elem_size);
                        } break;
                        case Read_int: {
                            int value_temp = (int)strtol(sub_row, NULL, 10);
                            memcpy(ptr_buffer + (scaned_elems++) * elem_size, &value_temp, elem_size);
                        } break;
                        default:
                            break;
                    }
                    
                    sub_row = strtok_portable(NULL, ROW_DELIMITERS, &save_sub_row);
                }
            }
        }

        row = strtok_portable(NULL, ROW_DELIMETER, &save_row);
    }

    return scaned_elems;
}

/* Считывает файл целиком и конвертирует найденные числа в плоский буфер double */
read_data read_matrix_from_file(const char *filename, Readtype scan_input) {
    /* Инициализируем контейнер результатом по умолчанию. */
    read_data container = {.elems_fl_data_count = 0, .flat_data = NULL, .status = NORMAL, .capacity = 0,.type=scan_input};

    FILE *file = fopen(filename, "rb");
    if (!file) {
        container.status = ERROR_FILE_READ;
        return container;
    }

    /* Вычисляем размер файла, чтобы подготовить буферы. */
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        container.status = ERROR_FILE_READ;
        return container;
    }
    long file_size = ftell(file);
    if (file_size <= 0) {
        container.status = FILE_EMPTY;
        fclose(file);
        return container;
    }
    fseek(file, 0, SEEK_SET);

    /* Буфер для исходного текстового содержимого. */
    char *dinamic_buffer = calloc(file_size+1, sizeof *dinamic_buffer);
    if (!dinamic_buffer) {
        container.status = ERROR_MEM_ALLOC;
        return container;
    }

    /* Буфер для результатов преобразования в числа. */
    size_t res_buf_cup = file_size;
    unsigned char *res_buffer = calloc(res_buf_cup, sizeof *res_buffer);
    if (!res_buffer) {
        free(dinamic_buffer);
        container.status = ERROR_MEM_ALLOC;
        return container;
    }
    container.flat_data = res_buffer;
    container.capacity = res_buf_cup;

    /* Читаем файл целиком. */
    size_t read = fread(dinamic_buffer, 1, (size_t)file_size, file);
    fclose(file);

    if (read != (size_t)file_size) {
        free(dinamic_buffer);
        free(res_buffer);
        container.status = ERROR_FILE_READ;
        return container;
    }

    /* Преобразуем текстовую матрицу в числовой буфер, допускающий расширение. */
    container.elems_fl_data_count = read_text(dinamic_buffer, &res_buffer, &res_buf_cup,scan_input);
    container.capacity = res_buf_cup;
    container.flat_data = res_buffer;

    free(dinamic_buffer);

    return container;
}

/* Печатает матрицу в текстовый файл, проверяя допустимость переданного формата */
ProcssState write_matrix_to_file(const char *filename, const char *format, void *const matrix_data, int rows,
                                 int cols) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("fopen");
        return ERROR_FILE_READ;
    }

    /* Находим позицию первого поддерживаемого спецификатора в строке формата. */
    int specif_idx = strcspn(SPECIFIERS, format);

    if (specif_idx == (int)strlen(SPECIFIERS)) {
        fclose(file);
        return INVALID_SPECIEF;
    }

    /* Общий буфер для snprintf. */
    char value[20] = {0};
    int **as_int = NULL;
    double **as_double = NULL;

    double compare_value_from_data = -1;
    char *end_ptr = NULL;

    /* Определяем формат обрабатываемых данных. */
    switch (SPECIFIERS[specif_idx]) {
        case 'd':
        case 'i': {
            as_int = (int **)matrix_data;
            compare_value_from_data = (double)as_int[0][0];
            break;
        }
        case 'f':
        case 'e':
        case 'g':
        case 'a': {
            as_double = (double **)matrix_data;
            compare_value_from_data = as_double[0][0];
            break;
        }
        default:
            fclose(file);
            return INVALID_SPECIEF;
    }

    /* Проверяем, что формат корректно отображает хотя бы одно значение. */
    snprintf(value, 20, format, compare_value_from_data);
    double compare_value_from_str = strtod(value, &end_ptr);
    if (compare_value_from_data != compare_value_from_str || *end_ptr != '\0' || end_ptr == value) {
        fclose(file);
        return INVALID_SPECIEF;
    }

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int scaned = 0;
            if (as_int) {
                scaned = snprintf(value, 20, format, as_int[row][col]);
            }
            if (as_double) {
                scaned = snprintf(value, 20, format, as_double[row][col]);
            }
            if (scaned <= 0) {
                fclose(file);
                return INVALID_SPECIEF;
            } else {
                /* Печатаем элемент и разделитель внутри строки. */
                int write = fprintf(file, "%s%s", value, NUMBER_DELIMETER);
                if (write != scaned + 1) {
                    fclose(file);
                    return ERROR_FILE_WRITE;
                }
            }
        }
        /* Завершаем строку матрицы. */
        fprintf(file, "%s", ROW_DELIMETER);
    }

    if (fclose(file) != 0) {
        return ERROR_FILE_WRITE;
    }

    return NORMAL;
}

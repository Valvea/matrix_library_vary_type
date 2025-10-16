// matrix_core.h
#ifndef MATRIX_CORE_H
#define MATRIX_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Тип хранимых элементов в структуре Matrix */
typedef enum {
  MAT_INVALID = 0, /* Неверное/неинициализированное состояние */
  MAT_INT32   = 1, /* Матрица с элементами типа int */
  MAT_F64     = 2, /* Матрица с элементами типа double */
} MatType;

/* Статусы ошибок базового слоя матриц */
typedef enum {
  ME_OK = 0,             /* Операция прошла успешно */
  ME_BAD_ARGS = -1,      /* Переданы некорректные аргументы */
  ME_OOM = -2,           /* Не хватило памяти */
  ME_TYPE_MISMATCH = -3, /* Типы матриц не совпали */
} MatErr;

// Базовая структура матрицы (единый буфер: [row_ptrs][flat_data])
typedef struct {
  int     rows;          /* Количество строк */
  int     cols;          /* Количество столбцов */
  int     owns;          // 1 если этот объект владеет памятью (надо free), 0 если «вид» (view)
  MatType type;          /* Тип хранимых данных */
  void   *block;         // адрес единого блока (для free)
  void   *flat_data;     // начало плоских данных (double* или int*)
  union {
    double **d;          // указатели на строки (для double)
    int    **i;          // указатели на строки (для int32)
    void   **vp;         // небезопасный общий вид
  } row;
} Matrix;

// ── Создание/освобождение ─────────────────────────────────────────────────────
/* Выделяет матрицу заданного типа и размера, плоский буфер инициализируется нулями */
Matrix *mat_new(MatType mat_type, int rows, int cols);  // аллокация + нули
/* Освобождает структуру Matrix, при owns=1 дополнительно освобождает общий блок данных */
void    mat_free(Matrix *matrix);                       // освобождение (если owns=1)

// Клоны/копии (тип должен совпадать)
/* Создаёт полную копию матрицы с собственным буфером данных */
Matrix *mat_clone(const Matrix *source);                // глубокая копия
/* Переносит данные одной матрицы в другую; типы и размеры должны совпадать */
MatErr  mat_copy(const Matrix *source, Matrix *dest);   // dest того же размера и типа

// Обёртки над уже существующими буферами (без копии)
// flat_data_row_major — плоский буфер row-major; если make_row_ptrs=1, создадим row-пойнтеры поверх flat_data
/* Формирует Matrix как обёртку над внешним буфером данных (без копирования) */
Matrix *mat_wrap_buffer(MatType mat_type, void *flat_data_row_major,
                        int rows, int cols, int make_row_ptrs);

// Доступ к плоскому буферу/строкам как к правильному типу (вернут NULL, если тип не тот)
/* Возвращает небезопасный указатель на плоский буфер double (NULL при несовпадении типа) */
double       *mat_data_f64(Matrix *matrix);
/* Возвращает const-указатель на плоский буфер double (NULL при несовпадении типа) */
const double *mat_cdata_f64(const Matrix *matrix);
/* Возвращает небезопасный указатель на плоский буфер int (NULL при несовпадении типа) */
int          *mat_data_i32(Matrix *matrix);
/* Возвращает const-указатель на плоский буфер int (NULL при несовпадении типа) */
const int    *mat_cdata_i32(const Matrix *matrix);

// ── Утилиты формы ─────────────────────────────────────────────────────────────
/* Проверяет, является ли матрица квадратной */
static inline int mat_is_square(const Matrix *matrix) { return matrix && matrix->rows == matrix->cols; }

// ── Адаптеры к твоим существующим функциям (ожидающим T**)
// ВАЖНО: Эти указатели валидны, пока жива Matrix и форма не менялась.
/* Возвращает массив указателей на строки для матрицы double, при необходимости создаёт его */
double **mat_rows_f64(Matrix *matrix);
/* Возвращает массив указателей на строки для матрицы int, при необходимости создаёт его */
int    **mat_rows_i32(Matrix *matrix);

#ifdef __cplusplus
}
#endif

#endif

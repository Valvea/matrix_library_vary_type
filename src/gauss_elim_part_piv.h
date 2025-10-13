#ifndef GAUSS_ELIM_PART_PIV_H
#define GAUSS_ELIM_PART_PIV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int status;
    double determinant;
    double **invers_matrix;
    int swaps;

} result_solve;

typedef enum { DET, INVERSE } solve_mode;

result_solve matrix_det_lu_double(solve_mode mode, const double * const*matrix_origin, int rows, int cols);

#ifdef __cplusplus
}
#endif

#endif
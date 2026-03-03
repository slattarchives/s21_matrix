#include "s21_matrix.h"

int isWrongMatrix(matrix_t *matrix) {
    return (matrix == NULL || matrix->matrix == NULL || matrix->rows <= 0 || matrix->columns <= 0);
}

int s21_create_matrix(int rows, int columns, matrix_t *result) {
    int flag = OK;
    if (rows <= 0 || columns <= 0 || result == NULL) {
        flag = INCORRECT;
    } else {
        result->matrix = (double**)calloc(rows, sizeof(double*));
        if (result->matrix == NULL) {
            flag = INCORRECT;
        } else {
            result->rows = rows;
            result->columns = columns;
            for (int i = 0; i < rows && flag == OK; i++) {
                result->matrix[i] = (double*)calloc(columns, sizeof(double));
                if (result->matrix[i] == NULL) {
                    flag = INCORRECT;
                    s21_remove_matrix(result);
                }
            }
        }
    }
    
    return flag;
}

void s21_remove_matrix(matrix_t *A) {
    if (!isWrongMatrix(A)) { 
        for (int i = 0; i < A->rows; i++) {
            if (A->matrix[i] != NULL) {
                free(A->matrix[i]);
            }
        }
        free(A->matrix);
        A->matrix = NULL;
        A->rows = 0;
        A->columns = 0;
    }
}

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
    int flag = SUCCESS;
    
    if (isWrongMatrix(A) || isWrongMatrix(B)) {
        flag = FAILURE;
    } else if (A->rows != B->rows || A->columns != B->columns) {
        flag = FAILURE;
    } else {
        for (int i = 0; i < A->rows && flag == SUCCESS; i++) {
            for (int j = 0; j < A->columns && flag == SUCCESS; j++) {
                if (fabs(A->matrix[i][j] - B->matrix[i][j]) > 1e-6) {
                    flag = FAILURE;
                }
            }
        }
    }
    
    return flag;
}

int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
    int flag = OK;
    if (isWrongMatrix(A) || isWrongMatrix(B) || result == NULL) {
        flag = INCORRECT;
    } else if ((A->rows != B->rows) || (A->columns != B->columns)) {
        flag = CALC_ERROR;
    } else {
        flag = s21_create_matrix(A->rows, A->columns, result);
        if (flag == OK) {
            for (int i = 0; i < A->rows; i++) {
                for (int j = 0; j < A->columns; j++) {
                    result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
                }
            }
        }
    }
    return flag;
}
int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
    int flag = OK;
    if (isWrongMatrix(A) || isWrongMatrix(B) || result == NULL) {
        flag = INCORRECT;
    } else if ((A->rows != B->rows) || (A->columns != B->columns)) {
        flag = CALC_ERROR;
    } else {
        flag = s21_create_matrix(A->rows, A->columns, result);
        if (flag == OK) {
            for (int i = 0; i < A->rows; i++) {
                for (int j = 0; j < A->columns; j++) {
                    result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
                }
            }
        }
    }
    return flag;
}
int s21_mult_number(matrix_t *A, double number, matrix_t *result) {
    int flag = OK;
    if (isWrongMatrix(A) || result == NULL) {
        flag = INCORRECT;
    } else {
        flag = s21_create_matrix(A->rows, A->columns, result);
        if (flag == OK) {
            for (int i = 0; i < A->rows; i++) {
                for (int j = 0; j < A->columns; j++) {
                    result->matrix[i][j] = A->matrix[i][j] * number;
                }
            }
        }
    }
    return flag;
}
int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
    int flag = OK;
    if (isWrongMatrix(A) || isWrongMatrix(B) || result == NULL) {
        flag = INCORRECT;
    } else if (A->columns != B->rows) {
        flag = CALC_ERROR;
    } else {
        flag = s21_create_matrix(A->rows, B->columns, result);
        if (flag == OK) {
            for (int i = 0; i < A->rows; i++) {
                for (int j = 0; j < B->columns; j++) {
                    result->matrix[i][j] = 0;
                    for (int counter = 0; counter < B->rows; counter++){
                        result->matrix[i][j] += A->matrix[i][counter] * B->matrix[counter][j];
                    }
                }
            }
        }
    }
    
    return flag;
}
int s21_transpose(matrix_t *A, matrix_t *result) {
    int flag = OK;
    if (isWrongMatrix(A) || result == NULL) {
        flag = INCORRECT;
    } else {
        flag = s21_create_matrix(A->columns, A->rows, result);
        if (flag == OK) {
            for (int i = 0; i < A->rows; i++) {
                for (int j = 0; j < A->columns; j++) {
                    result->matrix[j][i] = A->matrix[i][j];
                }
            }
        }
    }
    return flag;
}

void newMatrixLessSize(matrix_t *old, matrix_t *new, int exclusionRow, int exclusionColumn) {
    int indexRow = 0; 
    for (int i = 0; i < old->rows; i++) {
        if (i != exclusionRow) {
            int indexCol = 0;
            for (int j = 0; j < old->columns; j++) {
                if (j != exclusionColumn) {
                    new->matrix[indexRow][indexCol] = old->matrix[i][j];
                    indexCol++;
                }
            }
            indexRow++;
        }
    }
}

int s21_calc_complements(matrix_t *A, matrix_t *result) {
    int flag = OK;
    matrix_t minor = {0};
    double det_minor = 0.0;

    if (isWrongMatrix(A) || result == NULL) {
        flag = INCORRECT;
    } else if (A->rows != A->columns) {
        flag = CALC_ERROR;
    } else {
        flag = s21_create_matrix(A->rows, A->columns, result);
        if (flag == OK && A->rows == 1) {
            result->matrix[0][0] = 1.0;
        } else if (flag == OK) {
            for (int i = 0; i < A->rows ; i++) {
                for (int j = 0; j < A->columns; j++) {
                    flag = s21_create_matrix(A->rows - 1, A->columns - 1, &minor);
                    if (flag == OK) {
                        newMatrixLessSize(A, &minor, i, j);
                        flag = s21_determinant(&minor, &det_minor);
                        if (flag == OK) {
                            int sign = ((i + j) % 2 == 0) ? 1 : -1;
                            result->matrix[i][j] = sign * det_minor;
                        }
                        s21_remove_matrix(&minor);
                    }
                }
            }
        }
    }
    return flag;
}


int s21_determinant(matrix_t *A, double *result) {
    int flag = OK;
    double det = 0.0;

    if (isWrongMatrix(A) || result == NULL) {
        flag = INCORRECT;
    } else if (A->rows != A->columns) {
        flag = CALC_ERROR;
    } else if (A->rows == 1) {
        det = A->matrix[0][0];
    } else if (A->rows == 2) {
        det = A->matrix[0][0] * A->matrix[1][1] - A->matrix[0][1] * A->matrix[1][0];
    } else { 
        for (int j = 0; j < A->columns; j++) {
            double minor_det = 0.0;
            matrix_t minor = {0};

            flag = s21_create_matrix(A->rows - 1, A->columns - 1, &minor);
            if (flag == OK) {
                newMatrixLessSize(A, &minor, 0, j);
                flag = s21_determinant(&minor, &minor_det);
                if (flag == OK) {
                    int sign = (j % 2 == 0) ? 1 : -1;
                    det += sign * A->matrix[0][j] * minor_det;
                }
                s21_remove_matrix(&minor); 
            }

        }
    } 
    if (flag == OK) *result = det;
    
    return flag;
}


int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
    int flag = OK;
    matrix_t complements = {0};
    matrix_t transposed = {0};
    double det = 0.0;

    if (isWrongMatrix(A) || result == NULL) {
        flag = INCORRECT;
    } else if ((A->rows != A->columns)) {
            flag = CALC_ERROR;
    } else {
        flag = s21_determinant(A, &det);
        if (flag == OK && fabs(det) < 1e-6) {
            flag = CALC_ERROR;
        }
        if (flag == OK) flag = s21_calc_complements(A, &complements);
        if (flag == OK) flag = s21_transpose(&complements, &transposed);
        if (flag == OK) flag = s21_mult_number(&transposed, 1.0 / det, result);

        if (complements.matrix != NULL) s21_remove_matrix(&complements);
        if (transposed.matrix != NULL) s21_remove_matrix(&transposed);
            
    }
    return flag;
}


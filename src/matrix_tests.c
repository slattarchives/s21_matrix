// tests/matrix_tests.c
#include <check.h>
#include <math.h>
#include <stdlib.h>
#include "s21_matrix.h"
#include <stdbool.h>

#define EPSILON 1e-6

/* ========== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ========== */

// Создать матрицу и заполнить значениями
matrix_t create_test_matrix(int rows, int cols, double *values) {
    matrix_t m;
    s21_create_matrix(rows, cols, &m);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m.matrix[i][j] = values[i * cols + j];
        }
    }
    return m;
}

// Сравнить две матрицы с эпсилоном
bool matrices_equal(matrix_t *A, matrix_t *B) {
    if (A->rows != B->rows || A->columns != B->columns) return false;
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
            if (fabs(A->matrix[i][j] - B->matrix[i][j]) > EPSILON) {
                return false;
            }
        }
    }
    return true;
}

// ✅ НОВАЯ: Проверка матрицы против массива expected (замена memcmp)
void check_matrix_values(matrix_t *M, double *expected, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            ck_assert_float_eq_tol(M->matrix[i][j], expected[i * cols + j], EPSILON);
        }
    }
}

// Освободить тестовую матрицу (если не использовалась remove_matrix)
void free_test_matrix(matrix_t *m) {
    if (m && m->matrix) {
        for (int i = 0; i < m->rows; i++) free(m->matrix[i]);
        free(m->matrix);
        m->matrix = NULL;
    }
}

/* ========== ТЕСТЫ: s21_create_matrix ========== */
START_TEST(test_create_matrix_valid_1x1) {
    matrix_t m;
    int res = s21_create_matrix(1, 1, &m);
    ck_assert_int_eq(res, OK);
    ck_assert_ptr_nonnull(m.matrix);
    ck_assert_int_eq(m.rows, 1);
    ck_assert_int_eq(m.columns, 1);
    s21_remove_matrix(&m);
}
END_TEST

START_TEST(test_create_matrix_valid_3x4) {
    matrix_t m;
    int res = s21_create_matrix(3, 4, &m);
    ck_assert_int_eq(res, OK);
    ck_assert_ptr_nonnull(m.matrix);
    ck_assert_int_eq(m.rows, 3);
    ck_assert_int_eq(m.columns, 4);
    s21_remove_matrix(&m);
}
END_TEST

START_TEST(test_create_matrix_valid_10x10) {
    matrix_t m;
    int res = s21_create_matrix(10, 10, &m);
    ck_assert_int_eq(res, OK);
    ck_assert_int_eq(m.rows, 10);
    ck_assert_int_eq(m.columns, 10);
    s21_remove_matrix(&m);
}
END_TEST

START_TEST(test_create_matrix_zero_rows) {
    matrix_t m;
    int res = s21_create_matrix(0, 5, &m);
    ck_assert_int_eq(res, INCORRECT);
}
END_TEST

START_TEST(test_create_matrix_zero_cols) {
    matrix_t m;
    int res = s21_create_matrix(5, 0, &m);
    ck_assert_int_eq(res, INCORRECT);
}
END_TEST

START_TEST(test_create_matrix_negative_rows) {
    matrix_t m;
    int res = s21_create_matrix(-3, 4, &m);
    ck_assert_int_eq(res, INCORRECT);
}
END_TEST

START_TEST(test_create_matrix_negative_cols) {
    matrix_t m;
    int res = s21_create_matrix(4, -2, &m);
    ck_assert_int_eq(res, INCORRECT);
}
END_TEST

START_TEST(test_create_matrix_null_result) {
    int res = s21_create_matrix(3, 3, NULL);
    ck_assert_int_eq(res, INCORRECT);
}
END_TEST

START_TEST(test_create_matrix_all_zeros) {
    matrix_t m;
    s21_create_matrix(2, 3, &m);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            ck_assert_float_eq(m.matrix[i][j], 0.0);
        }
    }
    s21_remove_matrix(&m);
}
END_TEST

/* ========== ТЕСТЫ: s21_remove_matrix ========== */
START_TEST(test_remove_matrix_valid) {
    matrix_t m;
    s21_create_matrix(3, 3, &m);
    s21_remove_matrix(&m);
    ck_assert_ptr_null(m.matrix);
    ck_assert_int_eq(m.rows, 0);
    ck_assert_int_eq(m.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_null_ptr) {
    s21_remove_matrix(NULL);
    ck_assert_int_eq(1, 1);
}
END_TEST

START_TEST(test_remove_matrix_already_null) {
    matrix_t m = {0};
    s21_remove_matrix(&m);
    ck_assert_int_eq(1, 1);
}
END_TEST

START_TEST(test_remove_matrix_twice) {
    matrix_t m;
    s21_create_matrix(2, 2, &m);
    s21_remove_matrix(&m);
    s21_remove_matrix(&m);
    ck_assert_int_eq(1, 1);
}
END_TEST

/* ========== ТЕСТЫ: s21_eq_matrix ========== */
START_TEST(test_eq_matrix_identical) {
    double vals[] = {1, 2, 3, 4};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t B = create_test_matrix(2, 2, vals);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), SUCCESS);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_different_values) {
    double valsA[] = {1, 2, 3, 4};
    double valsB[] = {1, 2, 3, 5};
    matrix_t A = create_test_matrix(2, 2, valsA);
    matrix_t B = create_test_matrix(2, 2, valsB);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), FAILURE);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_different_sizes) {
    double vals[] = {1, 2, 3, 4};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t B = create_test_matrix(2, 3, vals);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), FAILURE);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_epsilon_equal) {
    double valsA[] = {1.0, 2.0000001};
    double valsB[] = {1.0, 2.0000002};
    matrix_t A = create_test_matrix(1, 2, valsA);
    matrix_t B = create_test_matrix(1, 2, valsB);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), SUCCESS);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_epsilon_different) {
    double valsA[] = {1.0, 2.0};
    double valsB[] = {1.0, 2.00001};
    matrix_t A = create_test_matrix(1, 2, valsA);
    matrix_t B = create_test_matrix(1, 2, valsB);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), FAILURE);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_null_a) {
    matrix_t B;
    s21_create_matrix(2, 2, &B);
    ck_assert_int_eq(s21_eq_matrix(NULL, &B), FAILURE);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_null_b) {
    matrix_t A;
    s21_create_matrix(2, 2, &A);
    ck_assert_int_eq(s21_eq_matrix(&A, NULL), FAILURE);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_eq_matrix_both_null) {
    ck_assert_int_eq(s21_eq_matrix(NULL, NULL), FAILURE);
}
END_TEST

START_TEST(test_eq_matrix_negative_values) {
    double vals[] = {-1.5, 2.3, -0.0, 4.7};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t B = create_test_matrix(2, 2, vals);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), SUCCESS);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_large_values) {
    double vals[] = {1e10, -1e10, 1e-10, -1e-10};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t B = create_test_matrix(2, 2, vals);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), SUCCESS);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

/* ========== ТЕСТЫ: s21_sum_matrix ========== */
START_TEST(test_sum_matrix_basic) {
    double valsA[] = {1, 2, 3, 4};
    double valsB[] = {5, 6, 7, 8};
    double expected[] = {6, 8, 10, 12};
    matrix_t A = create_test_matrix(2, 2, valsA);
    matrix_t B = create_test_matrix(2, 2, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_sum_matrix(&A, &B, &result), OK);
    check_matrix_values(&result, expected, 2, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_negative) {
    double valsA[] = {-1, -2, -3, -4};
    double valsB[] = {1, 2, 3, 4};
    double expected[] = {0, 0, 0, 0};
    matrix_t A = create_test_matrix(2, 2, valsA);
    matrix_t B = create_test_matrix(2, 2, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_sum_matrix(&A, &B, &result), OK);
    check_matrix_values(&result, expected, 2, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_different_sizes) {
    double vals[] = {1, 2, 3, 4};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t B = create_test_matrix(2, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_sum_matrix(&A, &B, &result), CALC_ERROR);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_null_a) {
    matrix_t B, result;
    s21_create_matrix(2, 2, &B);
    ck_assert_int_eq(s21_sum_matrix(NULL, &B, &result), INCORRECT);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_float_precision) {
    double valsA[] = {0.1, 0.2};
    double valsB[] = {0.3, 0.4};
    double expected[] = {0.4, 0.6};
    matrix_t A = create_test_matrix(1, 2, valsA);
    matrix_t B = create_test_matrix(1, 2, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_sum_matrix(&A, &B, &result), OK);
    for (int i = 0; i < 2; i++) {
        ck_assert_float_eq_tol(result.matrix[0][i], expected[i], EPSILON);
    }
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

/* ========== ТЕСТЫ: s21_sub_matrix ========== */
START_TEST(test_sub_matrix_basic) {
    double valsA[] = {10, 20, 30, 40};
    double valsB[] = {1, 2, 3, 4};
    double expected[] = {9, 18, 27, 36};
    matrix_t A = create_test_matrix(2, 2, valsA);
    matrix_t B = create_test_matrix(2, 2, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_sub_matrix(&A, &B, &result), OK);
    check_matrix_values(&result, expected, 2, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_negative_result) {
    double valsA[] = {1, 2, 3, 4};
    double valsB[] = {10, 20, 30, 40};
    double expected[] = {-9, -18, -27, -36};
    matrix_t A = create_test_matrix(2, 2, valsA);
    matrix_t B = create_test_matrix(2, 2, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_sub_matrix(&A, &B, &result), OK);
    check_matrix_values(&result, expected, 2, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_same_matrix) {
    double vals[] = {5, 10, 15, 20};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t B = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_sub_matrix(&A, &B, &result), OK);
    for (int i = 0; i < 4; i++) {
        ck_assert_float_eq(result.matrix[i/2][i%2], 0.0);
    }
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

/* ========== ТЕСТЫ: s21_mult_number ========== */
START_TEST(test_mult_number_basic) {
    double vals[] = {1, 2, 3, 4};
    double expected[] = {3, 6, 9, 12};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_mult_number(&A, 3.0, &result), OK);
    check_matrix_values(&result, expected, 2, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_zero) {
    double vals[] = {10, 20, 30, 40};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_mult_number(&A, 0.0, &result), OK);
    for (int i = 0; i < 4; i++) {
        ck_assert_float_eq(result.matrix[i/2][i%2], 0.0);
    }
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_negative) {
    double vals[] = {1, -2, 3, -4};
    double expected[] = {-2, 4, -6, 8};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_mult_number(&A, -2.0, &result), OK);
    check_matrix_values(&result, expected, 2, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_float) {
    double vals[] = {2, 4, 6, 8};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_mult_number(&A, 0.5, &result), OK);
    ck_assert_float_eq(result.matrix[0][0], 1.0);
    ck_assert_float_eq(result.matrix[1][1], 4.0);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

/* ========== ТЕСТЫ: s21_mult_matrix ========== */
START_TEST(test_mult_matrix_2x2_basic) {
    double valsA[] = {1, 2, 3, 4};
    double valsB[] = {5, 6, 7, 8};
    double expected[] = {19, 22, 43, 50};
    matrix_t A = create_test_matrix(2, 2, valsA);
    matrix_t B = create_test_matrix(2, 2, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_mult_matrix(&A, &B, &result), OK);
    check_matrix_values(&result, expected, 2, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_identity) {
    double valsA[] = {1, 2, 3, 4};
    double valsI[] = {1, 0, 0, 1};
    matrix_t A = create_test_matrix(2, 2, valsA);
    matrix_t I = create_test_matrix(2, 2, valsI);
    matrix_t result;
    ck_assert_int_eq(s21_mult_matrix(&A, &I, &result), OK);
    ck_assert(matrices_equal(&A, &result));
    s21_remove_matrix(&A);
    s21_remove_matrix(&I);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_incompatible) {
    double vals[] = {1, 2, 3, 4};
    matrix_t A = create_test_matrix(2, 3, vals);
    matrix_t B = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_mult_matrix(&A, &B, &result), CALC_ERROR);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_3x3) {
    double valsA[] = {1,2,3, 4,5,6, 7,8,9};
    double valsB[] = {9,8,7, 6,5,4, 3,2,1};
    double expected[] = {30,24,18, 84,69,54, 138,114,90};
    matrix_t A = create_test_matrix(3, 3, valsA);
    matrix_t B = create_test_matrix(3, 3, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_mult_matrix(&A, &B, &result), OK);
    check_matrix_values(&result, expected, 3, 3);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_with_zero_row) {
    double valsA[] = {0,0,0, 1,2,3};
    double valsB[] = {1,2, 3,4, 5,6};
    matrix_t A = create_test_matrix(2, 3, valsA);
    matrix_t B = create_test_matrix(3, 2, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_mult_matrix(&A, &B, &result), OK);
    ck_assert_float_eq(result.matrix[0][0], 0.0);
    ck_assert_float_eq(result.matrix[0][1], 0.0);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

/* ========== ТЕСТЫ: s21_transpose ========== */
START_TEST(test_transpose_2x3) {
    double vals[] = {1,2,3, 4,5,6};
    double expected[] = {1,4, 2,5, 3,6};
    matrix_t A = create_test_matrix(2, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_transpose(&A, &result), OK);
    ck_assert_int_eq(result.rows, 3);
    ck_assert_int_eq(result.columns, 2);
    check_matrix_values(&result, expected, 3, 2);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_square) {
    double vals[] = {1,2,3, 4,5,6, 7,8,9};
    double expected[] = {1,4,7, 2,5,8, 3,6,9};
    matrix_t A = create_test_matrix(3, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_transpose(&A, &result), OK);
    check_matrix_values(&result, expected, 3, 3);  // ✅ Исправлено
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_1x1) {
    double vals[] = {42};
    matrix_t A = create_test_matrix(1, 1, vals);
    matrix_t result;
    ck_assert_int_eq(s21_transpose(&A, &result), OK);
    ck_assert_float_eq(result.matrix[0][0], 42);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_row_to_column) {
    double vals[] = {1,2,3,4,5};
    matrix_t A = create_test_matrix(1, 5, vals);
    matrix_t result;
    ck_assert_int_eq(s21_transpose(&A, &result), OK);
    ck_assert_int_eq(result.rows, 5);
    ck_assert_int_eq(result.columns, 1);
    for (int i = 0; i < 5; i++) {
        ck_assert_float_eq(result.matrix[i][0], vals[i]);
    }
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

/* ========== ТЕСТЫ: s21_determinant ========== */
START_TEST(test_determinant_1x1) {
    double vals[] = {5};
    matrix_t A = create_test_matrix(1, 1, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq(det, 5.0);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_2x2_basic) {
    double vals[] = {1,2, 3,4};
    matrix_t A = create_test_matrix(2, 2, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq(det, -2.0);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_3x3_example) {
    double vals[] = {1,2,3, 4,5,6, 7,8,9};
    matrix_t A = create_test_matrix(3, 3, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq(det, 0.0);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_3x3_nonzero) {
    double vals[] = {2,5,7, 6,3,4, 5,-2,-3};
    matrix_t A = create_test_matrix(3, 3, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq(det, -1.0);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_not_square) {
    double vals[] = {1,2,3,4};
    matrix_t A = create_test_matrix(2, 3, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), CALC_ERROR);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_identity_3x3) {
    double vals[] = {1,0,0, 0,1,0, 0,0,1};
    matrix_t A = create_test_matrix(3, 3, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq(det, 1.0);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_diagonal) {
    double vals[] = {2,0,0, 0,3,0, 0,0,4};
    matrix_t A = create_test_matrix(3, 3, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq(det, 24.0);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_negative) {
    double vals[] = {-1,2, 3,-4};
    matrix_t A = create_test_matrix(2, 2, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq(det, -2.0);
    s21_remove_matrix(&A);
}
END_TEST

/* ========== ТЕСТЫ: s21_calc_complements ========== */
START_TEST(test_calc_complements_2x2) {
    double vals[] = {1,2, 3,4};
    double expected[] = {4, -3, -2, 1};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_calc_complements(&A, &result), OK);
    check_matrix_values(&result, expected, 2, 2);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_calc_complements_3x3_example) {
    double vals[] = {1,2,3, 0,4,2, 5,2,1};
    double expected[] = {0, 10, -20, 4, -14, 8, -8, -2, 4};
    matrix_t A = create_test_matrix(3, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_calc_complements(&A, &result), OK);
    check_matrix_values(&result, expected, 3, 3);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_calc_complements_not_square) {
    double vals[] = {1,2,3,4};
    matrix_t A = create_test_matrix(2, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_calc_complements(&A, &result), CALC_ERROR);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_calc_complements_1x1) {
    double vals[] = {7};
    matrix_t A = create_test_matrix(1, 1, vals);
    matrix_t result;
    ck_assert_int_eq(s21_calc_complements(&A, &result), OK);
    ck_assert_float_eq(result.matrix[0][0], 1.0);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

/* ========== ТЕСТЫ: s21_inverse_matrix ========== */
START_TEST(test_inverse_matrix_2x2) {
    double vals[] = {4,7, 2,6};
    double expected[] = {0.6, -0.7, -0.2, 0.4};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_inverse_matrix(&A, &result), OK);
    check_matrix_values(&result, expected, 2, 2);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_3x3_example) {
    double vals[] = {2,5,7, 6,3,4, 5,-2,-3};
    matrix_t A = create_test_matrix(3, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_inverse_matrix(&A, &result), OK);
    matrix_t identity;
    s21_mult_matrix(&A, &result, &identity);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) ck_assert_float_eq_tol(identity.matrix[i][j], 1.0, 1e-4);
            else ck_assert_float_eq_tol(identity.matrix[i][j], 0.0, 1e-4);
        }
    }
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
    s21_remove_matrix(&identity);
}
END_TEST

START_TEST(test_inverse_matrix_singular) {
    double vals[] = {1,2,3, 4,5,6, 7,8,9};
    matrix_t A = create_test_matrix(3, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_inverse_matrix(&A, &result), CALC_ERROR);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_not_square) {
    double vals[] = {1,2,3,4};
    matrix_t A = create_test_matrix(2, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_inverse_matrix(&A, &result), CALC_ERROR);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_identity) {
    double vals[] = {1,0,0, 0,1,0, 0,0,1};
    matrix_t A = create_test_matrix(3, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_inverse_matrix(&A, &result), OK);
    ck_assert(matrices_equal(&A, &result));
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_diagonal) {
    double vals[] = {2,0,0, 0,3,0, 0,0,4};
    double expected[] = {0.5,0,0, 0,0.333333,0, 0,0,0.25};
    matrix_t A = create_test_matrix(3, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_inverse_matrix(&A, &result), OK);
    check_matrix_values(&result, expected, 3, 3);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

/* ========== ДОПОЛНИТЕЛЬНЫЕ ГРАНИЧНЫЕ ТЕСТЫ ========== */
START_TEST(test_create_matrix_very_large) {
    matrix_t m;
    int res = s21_create_matrix(100, 100, &m);
    ck_assert_int_eq(res, OK);
    ck_assert_int_eq(m.rows, 100);
    s21_remove_matrix(&m);
}
END_TEST

START_TEST(test_mult_matrix_1x1) {
    double valsA[] = {3};
    double valsB[] = {4};
    matrix_t A = create_test_matrix(1, 1, valsA);
    matrix_t B = create_test_matrix(1, 1, valsB);
    matrix_t result;
    ck_assert_int_eq(s21_mult_matrix(&A, &B, &result), OK);
    ck_assert_float_eq(result.matrix[0][0], 12.0);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_determinant_large_matrix) {
    double vals[] = {1,0,2,-1, 3,0,0,5, 2,1,4,-3, 1,0,5,0};
    matrix_t A = create_test_matrix(4, 4, vals);
    double det;
    ck_assert_int_eq(s21_determinant(&A, &det), OK);
    ck_assert_float_eq_tol(det, 30.0, 1e-4);
    s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_eq_matrix_very_small_diff) {
    double valsA[] = {1.0, 1.0000000001};
    double valsB[] = {1.0, 1.0000000002};
    matrix_t A = create_test_matrix(1, 2, valsA);
    matrix_t B = create_test_matrix(1, 2, valsB);
    ck_assert_int_eq(s21_eq_matrix(&A, &B), SUCCESS);
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_1x100) {
    double vals[100];
    for (int i = 0; i < 100; i++) vals[i] = i;
    matrix_t A = create_test_matrix(1, 100, vals);
    matrix_t B = create_test_matrix(1, 100, vals);
    matrix_t result;
    ck_assert_int_eq(s21_sum_matrix(&A, &B, &result), OK);
    for (int i = 0; i < 100; i++) {
        ck_assert_float_eq(result.matrix[0][i], 2*i);
    }
    s21_remove_matrix(&A);
    s21_remove_matrix(&B);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_100x1) {
    double vals[100];
    for (int i = 0; i < 100; i++) vals[i] = i * 0.5;
    matrix_t A = create_test_matrix(100, 1, vals);
    matrix_t result;
    ck_assert_int_eq(s21_transpose(&A, &result), OK);
    ck_assert_int_eq(result.rows, 1);
    ck_assert_int_eq(result.columns, 100);
    for (int i = 0; i < 100; i++) {
        ck_assert_float_eq(result.matrix[0][i], vals[i]);
    }
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_very_large) {
    double vals[] = {1e10, 1e-10, -1e10, -1e-10};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t result;
    ck_assert_int_eq(s21_mult_number(&A, 1e-5, &result), OK);
    ck_assert_float_eq_tol(result.matrix[0][0], 1e5, 1e-1);
    ck_assert_float_eq_tol(result.matrix[0][1], 1e-15, 1e-20);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_calc_complements_identity) {
    double vals[] = {1,0,0, 0,1,0, 0,0,1};
    double expected[] = {1,0,0, 0,1,0, 0,0,1};
    matrix_t A = create_test_matrix(3, 3, vals);
    matrix_t result;
    ck_assert_int_eq(s21_calc_complements(&A, &result), OK);
    check_matrix_values(&result, expected, 3, 3);
    s21_remove_matrix(&A);
    s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_check_product) {
    double vals[] = {2,1, 1,2};
    matrix_t A = create_test_matrix(2, 2, vals);
    matrix_t inv, product;
    ck_assert_int_eq(s21_inverse_matrix(&A, &inv), OK);
    ck_assert_int_eq(s21_mult_matrix(&A, &inv, &product), OK);
    ck_assert_float_eq_tol(product.matrix[0][0], 1.0, 1e-5);
    ck_assert_float_eq_tol(product.matrix[1][1], 1.0, 1e-5);
    ck_assert_float_eq_tol(product.matrix[0][1], 0.0, 1e-5);
    ck_assert_float_eq_tol(product.matrix[1][0], 0.0, 1e-5);
    s21_remove_matrix(&A);
    s21_remove_matrix(&inv);
    s21_remove_matrix(&product);
}
END_TEST

/* ========== СУИТЫ ТЕСТОВ ========== */
// (все suite_* функции остаются без изменений)
Suite* suite_create_matrix(void) {
    Suite *s = suite_create("create_matrix");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_create_matrix_valid_1x1);
    tcase_add_test(tc, test_create_matrix_valid_3x4);
    tcase_add_test(tc, test_create_matrix_valid_10x10);
    tcase_add_test(tc, test_create_matrix_zero_rows);
    tcase_add_test(tc, test_create_matrix_zero_cols);
    tcase_add_test(tc, test_create_matrix_negative_rows);
    tcase_add_test(tc, test_create_matrix_negative_cols);
    tcase_add_test(tc, test_create_matrix_null_result);
    tcase_add_test(tc, test_create_matrix_all_zeros);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_remove_matrix(void) {
    Suite *s = suite_create("remove_matrix");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_remove_matrix_valid);
    tcase_add_test(tc, test_remove_matrix_null_ptr);
    tcase_add_test(tc, test_remove_matrix_already_null);
    tcase_add_test(tc, test_remove_matrix_twice);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_eq_matrix(void) {
    Suite *s = suite_create("eq_matrix");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_eq_matrix_identical);
    tcase_add_test(tc, test_eq_matrix_different_values);
    tcase_add_test(tc, test_eq_matrix_different_sizes);
    tcase_add_test(tc, test_eq_matrix_epsilon_equal);
    tcase_add_test(tc, test_eq_matrix_epsilon_different);
    tcase_add_test(tc, test_eq_matrix_null_a);
    tcase_add_test(tc, test_eq_matrix_null_b);
    tcase_add_test(tc, test_eq_matrix_both_null);
    tcase_add_test(tc, test_eq_matrix_negative_values);
    tcase_add_test(tc, test_eq_matrix_large_values);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_sum_matrix(void) {
    Suite *s = suite_create("sum_matrix");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_sum_matrix_basic);
    tcase_add_test(tc, test_sum_matrix_negative);
    tcase_add_test(tc, test_sum_matrix_different_sizes);
    tcase_add_test(tc, test_sum_matrix_null_a);
    tcase_add_test(tc, test_sum_matrix_float_precision);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_sub_matrix(void) {
    Suite *s = suite_create("sub_matrix");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_sub_matrix_basic);
    tcase_add_test(tc, test_sub_matrix_negative_result);
    tcase_add_test(tc, test_sub_matrix_same_matrix);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_mult_number(void) {
    Suite *s = suite_create("mult_number");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_mult_number_basic);
    tcase_add_test(tc, test_mult_number_zero);
    tcase_add_test(tc, test_mult_number_negative);
    tcase_add_test(tc, test_mult_number_float);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_mult_matrix(void) {
    Suite *s = suite_create("mult_matrix");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_mult_matrix_2x2_basic);
    tcase_add_test(tc, test_mult_matrix_identity);
    tcase_add_test(tc, test_mult_matrix_incompatible);
    tcase_add_test(tc, test_mult_matrix_3x3);
    tcase_add_test(tc, test_mult_matrix_with_zero_row);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_transpose(void) {
    Suite *s = suite_create("transpose");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_transpose_2x3);
    tcase_add_test(tc, test_transpose_square);
    tcase_add_test(tc, test_transpose_1x1);
    tcase_add_test(tc, test_transpose_row_to_column);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_determinant(void) {
    Suite *s = suite_create("determinant");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_determinant_1x1);
    tcase_add_test(tc, test_determinant_2x2_basic);
    tcase_add_test(tc, test_determinant_3x3_example);
    tcase_add_test(tc, test_determinant_3x3_nonzero);
    tcase_add_test(tc, test_determinant_not_square);
    tcase_add_test(tc, test_determinant_identity_3x3);
    tcase_add_test(tc, test_determinant_diagonal);
    tcase_add_test(tc, test_determinant_negative);
    tcase_add_test(tc, test_determinant_large_matrix);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_calc_complements(void) {
    Suite *s = suite_create("calc_complements");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_calc_complements_2x2);
    tcase_add_test(tc, test_calc_complements_3x3_example);
    tcase_add_test(tc, test_calc_complements_not_square);
    tcase_add_test(tc, test_calc_complements_1x1);
    tcase_add_test(tc, test_calc_complements_identity);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_inverse_matrix(void) {
    Suite *s = suite_create("inverse_matrix");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_inverse_matrix_2x2);
    tcase_add_test(tc, test_inverse_matrix_3x3_example);
    tcase_add_test(tc, test_inverse_matrix_singular);
    tcase_add_test(tc, test_inverse_matrix_not_square);
    tcase_add_test(tc, test_inverse_matrix_identity);
    tcase_add_test(tc, test_inverse_matrix_diagonal);
    tcase_add_test(tc, test_inverse_matrix_check_product);
    suite_add_tcase(s, tc);
    return s;
}

Suite* suite_edge_cases(void) {
    Suite *s = suite_create("edge_cases");
    TCase *tc = tcase_create("core");
    tcase_add_test(tc, test_create_matrix_very_large);
    tcase_add_test(tc, test_mult_matrix_1x1);
    tcase_add_test(tc, test_eq_matrix_very_small_diff);
    tcase_add_test(tc, test_sum_matrix_1x100);
    tcase_add_test(tc, test_transpose_100x1);
    tcase_add_test(tc, test_mult_number_very_large);
    suite_add_tcase(s, tc);
    return s;
}

/* ========== MAIN ========== */
int main(void) {
    int number_failed;
    Suite *suites[] = {
        suite_create_matrix(), suite_remove_matrix(), suite_eq_matrix(),
        suite_sum_matrix(), suite_sub_matrix(), suite_mult_number(),
        suite_mult_matrix(), suite_transpose(), suite_determinant(),
        suite_calc_complements(), suite_inverse_matrix(), suite_edge_cases(), NULL
    };
    
    SRunner *sr = srunner_create(suites[0]);
    for (int i = 1; suites[i] != NULL; i++) {
        srunner_add_suite(sr, suites[i]);
    }
    
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
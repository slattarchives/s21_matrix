CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -g
LDFLAGS = -lcheck -lm -lsubunit -lpthread
GCOV_FLAGS = -fprofile-arcs -ftest-coverage

LIB = s21_matrix.a
SRC = s21_matrix.c
OBJ = $(SRC:.c=.o)
GCOV_OBJ = $(SRC:.c=.gcov.o)
TEST_SRC = matrix_tests.c
TEST_BIN = matrix_test

.PHONY: all clean test s21_matrix.a gcov_report rebuild

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

$(OBJ): $(SRC) s21_matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

$(GCOV_OBJ): $(SRC) s21_matrix.h
	$(CC) $(CFLAGS) $(GCOV_FLAGS) -c $< -o $@

test: $(LIB)
	$(CC) $(CFLAGS) $(TEST_SRC) $(LIB) -o $(TEST_BIN) $(LDFLAGS)
	./$(TEST_BIN)

gcov_report: clean_test
	$(CC) $(CFLAGS) $(GCOV_FLAGS) -c $(SRC) -o $(GCOV_OBJ)
	ar rcs s21_matrix_gcov.a $(GCOV_OBJ)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(TEST_SRC) s21_matrix_gcov.a -o $(TEST_BIN) $(LDFLAGS)
	./$(TEST_BIN)
	lcov --capture --directory . --output-file test.info --no-external --base-directory $(PWD)
	lcov --remove test.info '*/matrix_tests.c' '/usr/*' --output-file test.info --ignore-errors empty,unused
	mkdir -p report
	genhtml test.info --output-directory report --ignore-errors empty,unused

clean:
	rm -rf $(TEST_BIN) $(LIB) $(OBJ) $(GCOV_OBJ) s21_matrix_gcov.a *.gcda *.gcno *.gcov *.info report/

clean_test:
	rm -rf $(TEST_BIN) $(GCOV_OBJ) s21_matrix_gcov.a *.gcda *.gcno *.gcov *.info report/

rebuild: clean all
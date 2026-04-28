#ifndef MATRIX_H
#define MATRIX_H

#include <string>

// Объявление функций
void reduce_sum(int p, double* a = nullptr, int n = 0);
void thread_function(double* matrix, double* block_matrix, int thread_num, int total_threads, int n, int m, double* vector, int step);
double* allocateMatrix(int size); //Выделение памяти под матрицу
void freeMatrix(double* matrix); //Освобождение памяти матрицы
void form_vector(double* a, double* b, int n);// Формирование вектора по матрице из файла
double* createMatrix(double* matrix, int s, int n);
double* readMatrixFromFile(const std::string& filename, double* matrix, int n); // Чтение матрицы из файла
void writeVectorToFile(double* matrix, int n, const std::string& filename); //Запись вектора решения в файл
void printSolveVector(double* vector, int rows); //Печать вектора(решение)
double* proud_of_matrix(double* matrix_1, double* matrix_2, double* matrix_3, int m); //Произведение матриц
void sub_matrix(double* matrix_1, double* matrix_2, int m);
void printMatrix(const double* matrix, int rows, int cols, int r); //Вывод матрицы в стандартный поток(для тестирования функций)
void matrix_with_one(double* matrix, int m); // Заполнение единичной матрицы
void transfer_maxElem(double* matrix,double* reverse_matrix, int* list_swap, int m, int step); //Передвижение максимального элемента в столбце в левый верхний угол
void step_gauss(double *matrix, double* reverse_matrix, int* list_swap, int m, int step);// Шаг метода Гаусса
int method_gauss(double *matrix, double* reverse_matrix, int* list_swap, int m, double norm); // Метод Гаусса
void reverse_method_gauss(double* matrix,double* reverse_matrix, int* list_swap, int m);//Обратный ход метода Гаусса
void transponeted(double* matrix, int m); // Транспонирование матрицы
void dayte_zetku(double* reverse_matrix, int** list_swap, int m); // Обратное преобразование матрицы
double norm(double* matrix, int m); // норма матрицы
void transform_block(double* matrix, double* blocks_matrix, double* matrix_block, int n, int m); // блочное хранение
double* get_block(double* matrix, double* matrix_block, int n, int m, int i, int j); // взятие блока
void put_block(double* matrix, double* matrix_block, int n, int m, int i, int j);// помещение блока в матрицу
void swap_block_matrix(double* matrix_1, double* matrix_2, int m); // Перестановка двух матриц
int thread_step_blocks_gauss(int total_thread, int thread_num, double *matrix, double *block_matrix, double *reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec, int n, int m, int step, double *vector, double* vector_result, double matrix_norm);
int thread_block_method_gauss(int total_thread, int thread_num, double *matrix, double *block_matrix, double *reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec,  int n, int m, double* vector, double* vector_result, double matrix_norm); // Блочный метод Гаусса
void reverse_block_gauss(double *matrix, double *block_matrix, double *reverse_matrix, double *matrix_result, int *list_swap, int* swap_vec, int n, int m, double* vector, double* vector_result, double matrix_norm);
void proud_vector(double* matrix, double* vector, double* vector_result, int m, int l); // умножение матрицы на вектор
double* get_vector(double* vector, double* block_vector, int m, int i);
void put_vector(double* vector, double* block_vector, int m, int i, int l);
void sub_vector(double* vector_1, double* vector_2, int m);
double measurement(double* vector_b, int n); // вычисление невязки
double solve_measurement(double* matrix,double* vector_b, double* vector_result, int n);
int thread_solve_function(int total_thread, double* matrix, double* block_matrix, double* reverse_matrix, double* matrix_result, 
    int* list_swap, int* swap_vec, int n, int m, double* vector, double* vector_result, double matrix_norm); // Главная функция решения
double row_norm(double* matrix, int n);
double norm_reverse(double* matrix_result, double* reverse_matrix, int* list_swap, int m, double norm);
void thread_min_norm_transfer(int total_thread, int thread_num, double* matrix, double* block_matrix, double* reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec, int n, int m, int step, double* vector, double norm);

#endif

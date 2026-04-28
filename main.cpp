#include <iostream>
#include <time.h>
#include "matrix.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 6) {
        cerr << "Неверное количество аргументов. Ожидается: n r s" << endl;
        return 1;
    }

    int n = stoi(argv[2]);  
    int m = stoi(argv[3]);    
    int r = stoi(argv[4]);
    int s = stoi(argv[5]);
    int p = stoi(argv[1]);
    double* matrix = allocateMatrix(n);
    if (s == 0) {
        string inputFilename = argv[5];
        matrix = readMatrixFromFile(inputFilename, matrix, n);
        if (matrix == nullptr) {
            return -1;
        }
    }
    if (s != 0) {
        matrix = createMatrix(matrix, s, n);
    }


    int* list_swap = new int [n];

    for (int i = 0; i < n; i += 1) {
        list_swap[i] = -1;
    }

    double* vector_b = new double [n];
    int* swap_vec;
    if (n % m == 0) {
        swap_vec = new int [n / m];
    } else {
        swap_vec = new int [n / m + 1];
    }

    double* vector_result = new double [m];

    double* vector_mes = new double [n];

    form_vector(matrix, vector_b, n);

    double* reverse_matrix = allocateMatrix(m);

    double* result_matrix = allocateMatrix(m);

    double* matrix_block = allocateMatrix(m);
    double t1 = 0;
    double t2 = 0;
    double r1 = -1;
    double r2 = -1;
    clock_t start;
    clock_t end;
    int task = 11;
    printf("StartMatrix \n");
    printMatrix(matrix, n, n, r);
    proud_of_matrix(matrix, matrix, result_matrix, n);
    proud_of_matrix(result_matrix, matrix, matrix_block, n);
    printMatrix(matrix, n, n, 25);
    printf("\n");
    double norm = row_norm(matrix, n);
    matrix_with_one(reverse_matrix, m);

    start = clock();
    int result = thread_solve_function(p, matrix, matrix_block, reverse_matrix, result_matrix, list_swap, swap_vec, n, m, vector_b, vector_result, norm);
    end = clock();
    t1 = (double)(end - start) / (double)CLOCKS_PER_SEC;
    printf("SolveVector \n");
    printMatrix(vector_b, n, 1, r);
    printf("\n");
    if (result != -1) {
        if (s == 0) {
            string inputFilename = argv[5];
            matrix = readMatrixFromFile(inputFilename, matrix, n);
        } else {
            matrix = createMatrix(matrix, s, n);
        }
        form_vector(matrix, vector_mes, n);
        start = clock();
        r1 =  measurement(vector_b, n);

        r2 =  solve_measurement(matrix, vector_b, vector_mes, n);
        end = clock();
        t2 = (double)(end - start) / (double)CLOCKS_PER_SEC;
    } else {
        r1 = -1;
        r2 = -1;
        t2 = 0;
    }

    printf (
        "%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d M = %d\n",
        argv[0], task, r1, r2, t1, t2, s, n, m);

    delete[] list_swap; 
    delete[] vector_b;
    delete[] swap_vec;
    delete[] vector_result;
    delete[] vector_mes;

    freeMatrix(matrix);
    freeMatrix(matrix_block);
    freeMatrix(reverse_matrix);
    freeMatrix(result_matrix);
    return 0;
}


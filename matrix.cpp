#include "matrix.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <vector>

using namespace std;

std::mutex mx;

//Выделение памяти
double* allocateMatrix(int size) {
    return new double[size * size];
}

//Освободение
void freeMatrix(double* matrix) {
    delete[] matrix;
}

//Формирования вектора
void form_vector(double* a, double* b, int n) {
    for (int i = 0; i < n; ++i) {
        b[i] = 0;
    }
    for (int i = 0; i < n; i+=1) {
        for (int k = 0; k < n; k = k + 2) {
            b[i] += a[i * n + k];
        }
    }
}
//Выбор формулы в зависимости от s
double f(int s, int n, int i, int j) {
    switch (s) {
        case 1: 
            return (double)(n - max(i, j) + 1);  // Формула для s = 1
        case 2: 
            return (double)max(i, j);          // Формула для s = 2
        case 3: 
            return (double)std::abs(i - j);         // Формула для s = 3
        case 4: 
            return 1/(double)(i + j - 1);                 // Формула для s = 4
        default:
            std::cerr << "Значение не выбрано" << std::endl;
            return 0;
    }
}


// Формирование матрицы, если s != 0, +1 там из-за того, что в матрице кэф. с 1 начинаются
double* createMatrix(double* matrix, int s, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i * n + j] = f(s, n, i + 1, j + 1); 
        }
    }
    return matrix;
}

// Чтение матрицы из файла
double* readMatrixFromFile(const std::string& filename, double* matrix, int n) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Ошибка при открытие файла " << filename << std::endl;
        return nullptr;
    }
    int count = 0;

    for (int i = 0; i < n * n; ++i) {
        file >> matrix[i];
        if (!file) {
            break;
        }
        count += 1;
    }
    if (count != n*n) {
        std::cerr << "Элементов неправильное количество" << std::endl;
        freeMatrix(matrix);
        return nullptr;
    }
    file.close();
    return matrix;
}

// Запись вектора в файл
void writeVectorToFile(double* vector, int n, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    
    for (int i = 0; i < n; ++i) {
        file << vector[i] << std::endl;
    }
    
    file.close();
}

// Печать вектора решения на экран
void printSolveVector(double* vector, int rows) {
    
    for (int i = 0; i < rows; ++i) {
        cout << vector[i] << endl;
    }
}

//Печать матрицы в стандартный вывод(для проверок функций)
// void printMatrix(double* matrix, int n) {
//     for (int i = 0; i < n; i+=1) {
//         for (int j = 0; j < n; j+=1) {
//             if (j + 1 == n) {
//                 std::cout << matrix[i*n + j] << std::endl;
//             } else {
//                 std::cout << matrix[i*n + j] << ' ';
//             }
//         }
//     }
// }

void printMatrix(const double* matrix, int rows, int cols, int r) {
    int maxRows = min(rows, r);
    int maxCols = min(cols, r);
    for (int i = 0; i < maxRows; ++i) {
        for (int j = 0; j < maxCols; ++j) {
            cout << setw(10) << scientific << setprecision(3) 
                    << matrix[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
}

//Умножение матриц
double* proud_of_matrix(double* matrix_1, double* matrix_2, double* matrix_3, int m) {
    for (int i = 0; i < m; i += 1) {
        for (int j = 0; j < m; j += 1) {
            matrix_3[i*m + j] = 0;
        }
    }
    for (int k = 0; k < m; k += 1) {
        for (int i = 2; i < m; i += 3) {
            double temp = matrix_1[i*m + k];
            double temp_1 = matrix_1[(i - 1)*m + k];
            double temp_2 = matrix_1[(i - 2)*m + k];
            for (int j = 2; j < m; j += 3) {
                matrix_3[i*m + j] += matrix_2[k*m + j]*temp;
                matrix_3[i*m + j - 1] += matrix_2[k*m + j - 1]*temp;
                matrix_3[i*m + j - 2] += matrix_2[k*m + j - 2]*temp;
                matrix_3[(i - 1)*m + j] += matrix_2[k*m + j]*temp_1;
                matrix_3[(i - 1)*m + j - 1] += matrix_2[k*m + j - 1]*temp_1;
                matrix_3[(i - 1)*m + j - 2] += matrix_2[k*m + j - 2]*temp_1;
                matrix_3[(i - 2)*m + j] += matrix_2[k*m + j]*temp_2;
                matrix_3[(i - 2)*m + j - 1] += matrix_2[k*m + j - 1]*temp_2;
                matrix_3[(i - 2)*m + j - 2] += matrix_2[k*m + j - 2]*temp_2;
            }
            for (int j = m / 3 * 3; j < m; j += 1) {
                matrix_3[i*m + j] += matrix_2[k*m + j]*temp;
                matrix_3[(i - 1)*m + j] += matrix_2[k*m + j]*temp_1;
                matrix_3[(i - 2)*m + j] += matrix_2[k*m + j]*temp_2;
                
            }
        }
        for(int i = m / 3 * 3; i < m; i += 1) {
            double temp = matrix_1[i*m + k];
            for (int j = 2; j < m; j += 3) {
                matrix_3[i*m + j] += matrix_2[k*m + j]*temp;
                matrix_3[i*m + j - 1] += matrix_2[k*m + j - 1]*temp;
                matrix_3[i*m + j - 2] += matrix_2[k*m + j - 2]*temp;
            }
            for (int j = m / 3 * 3; j < m; j += 1) {
                matrix_3[i*m + j] += matrix_2[k*m + j]*temp;
            }
        }
    }
    return matrix_3;
} //Функция работает корректно, так что с ней все нормально

// Заполнение единичной матрицы
void matrix_with_one(double* matrix, int m) {
    for (int i = 0; i < m; i += 1) {
        for (int j = 0; j < m; j += 1) {
            if (i == j) {
                matrix[i*m + j] = 1.0;
            } else {
                matrix[i*m + j] = 0;
            }
        }
    }
}

//Пердвижение максимального элемента(не блока) на позицию a_{1}{1} + вычисления для обратной
void transfer_maxElem(double* matrix,double* reverse_matrix,int* list_swap, int m,int step) {
    double max_elem = 0;
    int index_max[1] = {step}; // заменял step на 0, на случай, когда все элементы околонулевые
    for (int j = step; j < m; j += 1) {
        index_max[0] = (std::fabs(matrix[j * m + step]) > std::fabs(max_elem)) ? j : index_max[0];
        max_elem = (std::fabs(matrix[j * m + step]) > std::fabs(max_elem)) ? matrix[j * m + step] : max_elem;
    }
    
     for (int i = step; i < m; i += 1) {
        if (i == index_max[0]) {
            double temp = 0;
            for(int k = 0; k < m; k += 1) {
                temp = matrix[step*m + k];
                matrix[step*m + k] = matrix[i*m + k];
                matrix[i*m + k] = temp;
                // Для обратной
                double temp_reverse = reverse_matrix[step*m + k];
                reverse_matrix[step*m + k] = reverse_matrix[i*m + k];
                reverse_matrix[i*m + k] = temp_reverse;

            }
            break;
        }
    }
    list_swap[step] = index_max[0];
    //Список декодируется так, step = строка столбец с которой 
    //меняем указанный строка столдец во второй индексации, изначально будет заполнен -1
    

} // Этот метод также был проверен, он верный + у единичной тоже верно переставляет

// Ход Гаусса, step здесь это какой это шаг, 0, ..., m -1 + опеации над обратной
int step_gauss(double *matrix, double *reverse_matrix,int* list_swap,int m, int step, double matrix_norm) {
    transfer_maxElem(matrix, reverse_matrix, list_swap, m, step);
    double temp = matrix[step*m + step];
    double epsi = matrix_norm > 5000 ? 1e-9 : 1e-11;
    if (fabs(temp) < epsi * matrix_norm) {
        return -1;
    } 
    for (int k = 0; k < m; k += 1) {   // Исправление было k = step - видимо верное
        matrix[step*m + k] = matrix[step*m + k] / temp;
        reverse_matrix[step*m + k] = reverse_matrix[step*m + k] / temp;
    }
    for (int i = step + 1; i < m; i += 1) {
        temp = matrix[i*m + step];
        for (int j = 0; j < m; j += 1) {   // Аналогичное исправление
            matrix[i*m + j] = matrix[i*m + j] - temp * matrix[step*m + j];
            reverse_matrix[i*m + j] = reverse_matrix[i*m + j] - temp * reverse_matrix[step*m + j];
        }
    }
    return 0;
    // printMatrix(matrix, m, m, m);
} // Ход Гаусса работает корректно, судя по тесту + вроде и для обратной корректно, потом проверю

//Полный метод Гаусса, в одну сторону
int method_gauss(double* matrix,double* reverse_matrix,int* list_swap, int m, double matrix_norm) {
    int result = 0;
    for (int i = m; i > 0; i -= 1) {
        int j = m - i;
        result = step_gauss(matrix,reverse_matrix, list_swap, m, j, matrix_norm);
        if (result == - 1) {
            return -1;
        }
    }
    return result;
}// Судя по всему работает верно( + прикрут обратной тоже верный - пока не факт), но обычный Гаусс работает верно!
//Обратный ход метода Гаусса
void reverse_method_gauss(double* matrix,double* reverse_matrix, int* list_swap, int m) {
    int j;
    for (int k = m - 1; k >= 0; k -= 1) {
        for (int i = k - 1; i >= 0; i -= 1) {
            double temp = matrix[i*m + k]; // коэффициент домножения
            for (j = m - 1; j >= 0; j -= 1) {
                // matrix[i*m + j] =   matrix[i*m + j] - matrix[k*m + j]*temp;
                reverse_matrix[i*m + j] = reverse_matrix[i*m + j] - reverse_matrix[k*m + j]*temp;
            }
        }
    }
    list_swap[0] = 0;
} // Если будет ошибка то думаем над этим

void transponeted(double* matrix, int m) {
    double* temp = allocateMatrix(m);
    for (int i = 0; i < m; i += 1) {
        for(int j = 0; j < m; j += 1) {
            temp[i * m + j] = matrix[i* m + j];
        }
    }
    for (int i = 0; i < m; i += 1) {
        for(int j = 0; j < m; j += 1) {
            matrix[i * m + j] = temp[j* m + i];
        }
    }
    freeMatrix(temp);
}

double norm(double* matrix, int m) {
    double max = 0;
    for (int j = 0; j < m; j += 1) {
        double sum_0 = 0;
        // double sum_1 = 0;
        // double sum_2 = 0;
        for (int i = 0; i < m; i += 1) {
            sum_0 += fabs(matrix[i*m + j]);// + fabs(matrix[(i + 1)*m + j]);
            // sum_1 += fabs(matrix[i*m + j + 1]) + fabs(matrix[(i + 1)*m + j + 1]);
            // sum_2 += fabs(matrix[i*m + j + 2]) + fabs(matrix[(i + 1)*m + j + 2]);
        }
        // for (int i = m / 2 * 2; i < m; i += 1) {
        //     sum_0 += fabs(matrix[i*m + j]);
        //     sum_1 += fabs(matrix[i*m + j + 1]);
        //     sum_2 += fabs(matrix[i*m + j + 2]);
        // }
        // sum_1 = sum_1 > sum_0 ? sum_1 : sum_0;
        // sum_2 = sum_2 > sum_1 ? sum_2 : sum_1;
        max = max > sum_0 ? max : sum_0;
    }
    // for (int j = m / 3 * 3; j < m; j += 1) {
    //     double sum_0 = 0;
    //     for (int i = 0; i < m; i += 1) {
    //         sum_0 += fabs(matrix[i*m + j]);
    //     }
    //     max = max > sum_0 ? max : sum_0;
    // }
    return max;
}
double* get_block(double* matrix, double* matrix_block, int n, int m, int i, int j) {
    int k = n / m;
    int l = n - k*m;
    int row = (i < k) ? m : l;
    int col = (j < k) ? m: l;
    double* ptr_block = matrix + i*n*m + j*m;
    for (int r = 0; r < m; r += 1) {
        for (int c = 0; c < m; c+= 1) {
            matrix_block[r*m + c] = 0;
        }
    }
    for (int r = 0; r < row; r += 1){
        for (int c = 0; c < col; c += 1) {
            matrix_block[r*m + c] = ptr_block[r*n + c];
        }
    }
    return matrix_block;
}
void put_block(double* matrix, double* matrix_block, int n, int m, int i, int j) {
    int k = n / m;
    int l = n - k*m;
    int row = (i < k) ? m : l;
    int col = (j < k) ? m : l;
    double* ptr_block = matrix + i*n*m + j*m;
    for (int r = 0; r < row; r += 1) {
        for (int c = 0; c < col; c += 1) {
            ptr_block[r * n + c] = matrix_block[r*m + c];
        }
    }
}
void transform_block(double* matrix, double* blocks_matrix, double* matrix_block, int n, int m) {
    int k = n / m;
    for (int row = 0; row < k + 1; row += 1) {
        for (int col = 0; col < k + 1; col += 1) {
            matrix_block = get_block(matrix, matrix_block, n, m, row, col);
            put_block(blocks_matrix, matrix_block, n, m, row, col);
        }
    }    
}
void sub_matrix(double* matrix_1, double* matrix_2, int m){
    int k = m / 3;
    for (int i = 2; i < m; i += 3) {
        for (int j = 2; j < m; j += 3) {
            matrix_1[i*m + j] = matrix_1[i*m + j] - matrix_2[i*m + j];
            matrix_1[i*m + j - 1] = matrix_1[i*m + j - 1] - matrix_2[i*m + j - 1];
            matrix_1[i*m + j - 2] = matrix_1[i*m + j - 2] - matrix_2[i*m + j - 2];
            matrix_1[(i - 1)*m + j] = matrix_1[(i - 1)*m + j] - matrix_2[(i - 1)*m + j];
            matrix_1[(i - 1)*m + j - 1] = matrix_1[(i - 1)*m + j - 1] - matrix_2[(i - 1)*m + j - 1];
            matrix_1[(i - 1)*m + j - 2] = matrix_1[(i - 1)*m + j - 2] - matrix_2[(i - 1)*m + j - 2];
            matrix_1[(i - 2)*m + j] = matrix_1[(i - 2)*m + j] - matrix_2[(i - 2)*m + j];
            matrix_1[(i - 2)*m + j - 1] = matrix_1[(i - 2)*m + j - 1] - matrix_2[(i - 2)*m + j - 1];
            matrix_1[(i - 2)*m + j - 2] = matrix_1[(i - 2)*m + j - 2] - matrix_2[(i - 2)*m + j - 2];
        }
        for (int j = k * 3; j < m; j += 1) {
            matrix_1[i*m + j] = matrix_1[i*m + j] - matrix_2[i*m + j];
        }
    }
    for (int i = k * 3; i < m; i += 1) {
        for (int j = 2; j < m; j += 3) {
            matrix_1[i*m + j] = matrix_1[i*m + j] - matrix_2[i*m + j];
            matrix_1[i*m + j - 1] = matrix_1[i*m + j - 1] - matrix_2[i*m + j - 1];
            matrix_1[i*m + j - 2] = matrix_1[i*m + j - 2] - matrix_2[i*m + j - 2];
        }
        for (int j = k * 3; j < m; j += 1) {
            matrix_1[i*m + j] = matrix_1[i*m + j] - matrix_2[i*m + j];
        }
    }
}
void swap_block_matrix(double* matrix_1, double* matrix_2, int m) {
    for (int i = 0; i < m; i += 1) {
        for (int j = 0; j < m; j += 1) {
            double temp = matrix_1[i*m + j];
            matrix_1[i*m + j] = matrix_2[i*m + j];
            matrix_2[i*m + j] = temp;
        }
    }
}
double norm_reverse(double* matrix, double* reverse_matrix,int* list_swap, int m, double matrix_norm) {
    int result = method_gauss(matrix, reverse_matrix, list_swap, m, matrix_norm);
    if (result == -1) {
        return 1e+41;
    }
    reverse_method_gauss(matrix, reverse_matrix, list_swap, m);
    return norm(reverse_matrix, m);
}
void min_norm_transfer(double* matrix, double* block_matrix, double* reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec, int n, int m, int step, double* vector, double matrix_norm){
    int k = n / m;
    double min_norm = 1e+40;
    double temp;
    int index[2] = {0, 0};
    for(int i = step; i < k; i += 1) {
        for (int j = step; j < k; j += 1) { // замена 0 на step для ускорения и избежания ошибок
            block_matrix = get_block(matrix, block_matrix, n, m, i, j);
            for (int a = 0; a < m; a += 1) {
                for (int b = 0; b < m; b += 1) {
                    matrix_result[a*m + b] = block_matrix[a*m + b];
                }
            }
            matrix_with_one(reverse_matrix, m);
            temp = norm_reverse(matrix_result, reverse_matrix, list_swap, m, matrix_norm);
            if (temp < min_norm) {
                min_norm = temp;
                index[0] = i;
                index[1] = j;
            }
        }
    }
    if (n%m != 0) {
        k += 1;
    }
    for (int i = step; i < k; i += 1){
        if (i == index[0]) {
            for (int j = 0; j < k; j += 1) {
                block_matrix = get_block(matrix, block_matrix, n, m, i, j);
                for (int a = 0; a < m; a += 1) {
                    for (int b = 0; b < m; b += 1) {
                        matrix_result[a*m + b] = block_matrix[a*m + b];
                    }
                }
                put_block(matrix,get_block(matrix, block_matrix, n, m, step, j), n, m, i, j);
                put_block(matrix, matrix_result, n, m, step, j);           
            }
            for (int c =  0; c < m; c += 1) {
                temp = vector[i*m + c];
                vector[i*m + c] = vector[step*m + c];
                vector[step*m + c] = temp;
            }
            break;
        }
    }
    for (int j = step; j < k; j += 1){ // Замена 0 на step
        if (j == index[1]) {
            for (int i = 0; i < k; i += 1) {
                block_matrix = get_block(matrix, block_matrix, n, m, i, j);
                for (int a = 0; a < m; a += 1) {
                    for (int b = 0; b < m; b += 1) {
                        matrix_result[a*m + b] = block_matrix[a*m + b];
                    }
                }
                put_block(matrix, get_block(matrix, block_matrix, n, m, i, step), n, m, i, j);
                put_block(matrix, matrix_result, n, m, i, step);           
            }
            swap_vec[step] = index[1];
            break;
        }
    }
}
void proud_vector(double* matrix, double* vector, double* vector_result, int m, int l) {
    for (int i = 0; i < m; i += 1) {
        double sum = 0;
        for (int j = 0; j < l; j += 1) {
            sum += matrix[i*m + j]*vector[j]; 
        }
        vector_result[i] = sum;
    } // Разворот этих циклов немного улучшил невязку вектора ответа, если что потом откатить
    // так как по идее ничего меняться не должно было
}

void sub_vector(double* vector_1, double* vector_2, int m) {
    for (int i = 0; i < m; i += 1) {
        vector_1[i] = vector_1[i] - vector_2[i];
    }
}

double* get_vector(double* vector, double* block_vector, int m, int i) {
    for (int row = 0; row < m; row += 1) {
        block_vector[row] = vector[i*m + row];
    }
    return block_vector;
}

void put_vector(double* vector, double* block_vector, int m, int i, int l) {
    for (int row = 0; row < l; row += 1) {
        vector[i*m + row] = block_vector[row];
    }
}  // добавил исправление чтобы не падала память, регулируем кол-во put элементов

int step_blocks_gauss(double *matrix, double *block_matrix, double *reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec, int n, int m, int step, double *vector, double* vector_result, double matrix_norm) {
    int k = n / m;
    int l = n - k*m;
    double* temp;
    min_norm_transfer(matrix, block_matrix, reverse_matrix, matrix_result, list_swap, swap_vec, n, m, step, vector, matrix_norm);
    matrix_with_one(reverse_matrix, m);
    block_matrix = get_block(matrix, block_matrix, n, m, step, step);
    int result = method_gauss(block_matrix, reverse_matrix, list_swap, m, matrix_norm);
    reverse_method_gauss(block_matrix, reverse_matrix, list_swap, m);
    proud_vector(reverse_matrix, &vector[step*m], vector_result, m, m);
    put_vector(vector, vector_result, m, step, m);

    if (n%m != 0) {
        k = k + 1;
    } else {
        l = m;
    } // добавленое изменение, так как остаточные блоки забываются
    for (int j = step; j < k; j += 1) {
        block_matrix = get_block(matrix, block_matrix, n, m, step, j);
        matrix_result = proud_of_matrix(reverse_matrix, block_matrix, matrix_result, m);
        put_block(matrix, matrix_result, n, m, step, j);
    }  // предыдущая часть является последовательной программой, она будет идти перед потоковыми функциями
    for (int i = step + 1; i < k; i += 1) {
        block_matrix = get_block(matrix, block_matrix, n, m, i, step);
        for (int row = 0; row < m ; row += 1) {
            for (int col = 0; col < m; col += 1) {
                reverse_matrix[row*m + col] = block_matrix[row*m + col];
            }
        }
        int greater = matrix_norm < 1e+39 ? 1 : 2;
        for (int v = 0; v < m; v += 1) {
            double sum = 0;
            for (int d = 0; d < m; d += greater) {
                sum += reverse_matrix[v*m + d]*vector[step*m + d];

            }
            vector_result[v] = sum;
        }
        // printMatrix(vector_result, m, 1, m);
        //proud_vector(reverse_matrix, &vector[step*m], vector_result, m);

        // put_vector(vector, vector_result, m, step);
        if (i == k - 1) {
            sub_vector(&vector[i*m], vector_result, l);
        } else {
            sub_vector(&vector[i*m], vector_result, m);
        }
        for (int j = step; j < k; j += 1) {
            if (j == step) {
                matrix_with_one(block_matrix, m);
                temp = block_matrix;
            } else {
                temp = get_block(matrix, block_matrix, n, m, step, j);
            }
            // из-за того что на e^+40 0 = e^-14 при умножении будет не 0, а довольно большое число надо написать умножение с учетом нормы
            matrix_result = proud_of_matrix(reverse_matrix, temp, matrix_result, m);
            sub_matrix(get_block(matrix, block_matrix, n, m, i, j), matrix_result, m);
            put_block(matrix, block_matrix, n, m, i, j); // используем reverse, чтобы лишний раз не копировать матрицу в proud
            // так как reverse в этой части функции уже не используется
        }
    }
    // printMatrix(matrix, n, n, n);
    // printMatrix(vector, n, 1, n);
    return result;
}


int block_method_gauss(double *matrix, double *block_matrix, double *reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec,  int n, int m, double* vector, double* vector_result, double matrix_norm){
    int k = n / m;
    int result = 0;
    int l = n - k*m;
    for (int i = k; i > 0; i -= 1) {
        int j = k - i;
        result = step_blocks_gauss(matrix, block_matrix, reverse_matrix, matrix_result, list_swap, swap_vec, n, m, j, vector, vector_result, matrix_norm);
    }
    if (result == -1) {
        return -1;
    }
    if (n % m != 0) {
        matrix_with_one(reverse_matrix, m);
        block_matrix = get_block(matrix, block_matrix, n, m, k, k);
        for (int i = n - k *m; i > 0; i -= 1) {
            int j = n - k*m - i;
            result = step_gauss(block_matrix, reverse_matrix, list_swap, m, j, matrix_norm);
        }
        reverse_method_gauss(block_matrix, reverse_matrix, list_swap, m);
        // printMatrix(reverse_matrix, m, m, m);
        proud_vector(reverse_matrix, &vector[k*m], vector_result, m, l);
        put_vector(vector, vector_result, m, k, l); // заменял l на m в индексе строки
        proud_of_matrix(get_block(matrix, block_matrix, n, m, k, k), reverse_matrix, matrix_result, m); // добавил так как нет обратного хода и это важно
        put_block(matrix, matrix_result, n, m, k, k);  // было block_matrix
    }
    return result;
}

void reverse_block_gauss(double *matrix, double *block_matrix, double *reverse_matrix, double *matrix_result, int *list_swap, int* swap_vec, int n, int m, double* vector, double* vector_result, double matrix_norm){
    int k = n / m;  // При целочисленном делении пока ошибка, надо как-то учесть этот случай
    int even = k;
    if (n%m == 0) {
        even = k - 1;
    }
    int l = n - m*k;
    // if (l == 0) {
    //     l = m;
    //     k = k - 1;
    // }
    // block_matrix = get_block(matrix, block_matrix, n, l, k, k);
    // method_gauss(block_matrix, reverse_matrix, list_swap, l);
    // reverse_method_gauss(block_matrix, reverse_matrix, list_swap, l);
    // put_block(matrix, block_matrix, n, l , k, k);
    // proud_vector(reverse_matrix, &vector[k*m], vector_result, l);
    // put_vector(vector, vector_result, l, k);
    // printSolveVector(vector, n);
    matrix_norm = matrix_norm * 1;
    list_swap = list_swap;
    reverse_matrix = get_block(matrix, reverse_matrix, n, m, even, even);
    for (int i = 0; i < m; i += 1){
        vector_result[i] = 0;
    }
    for (int i = even - 1; i >= 0; i -= 1) {
        block_matrix = get_block(matrix, block_matrix, n, m, i, even);
        proud_vector(block_matrix, &vector[even*m], vector_result, m, l);
        sub_vector(&vector[i*m], vector_result, m); // здесь точно m
        matrix_result = proud_of_matrix(block_matrix, reverse_matrix, matrix_result, m);
        sub_matrix(block_matrix, matrix_result, l); // не уверен, что дело в l
        put_block(matrix, block_matrix, n, m, i, even); 
        // здесь тоже надо сделать обнуление, и умножение делать только на присоединенный вектор
    }
    // printSolveVector(vector, n);
    for (int j = k - 1; j >= 0; j -= 1) {
        // block_matrix = get_block(matrix, block_matrix, n, m, j, j);

        // matrix_with_one(reverse_matrix, m);
        // method_gauss(block_matrix, reverse_matrix, list_swap, m, matrix_norm);
        // reverse_method_gauss(block_matrix, reverse_matrix, list_swap, m);

        // proud_vector(reverse_matrix, &vector[j*m], vector_result, m);
        // put_vector(vector, vector_result, m, j);
        // put_block(matrix, block_matrix, n, m , j, j);
        matrix_with_one(reverse_matrix, m);
        for (int i = j; i >= 1; i -= 1) { // возможно там i = k - 1
            if (j == 0) {
                break;
            }
            block_matrix = get_block(matrix, block_matrix, n, m, i - 1, j);
            proud_vector(block_matrix, &vector[j*m], vector_result, m, m);
            sub_vector(&vector[(i - 1)*m], vector_result, m);
            // идея обнуления блоков сверху и домножение просто вектора присоединенного, домножение на единичный блок в обратном
            //ходе особо смысла не имеет, так что мы сможем серьезно усилить скорость работы за счет этого действия
            //позже я добавлю здесь умножение присоединенного вектора
            // sub_matrix(block_matrix, block_matrix, m);
            // put_block(matrix, block_matrix, n, m, i  - 1, j);

        }
    }
    for (int i = even - 1; i >= 0; i -= 1) {
        for (int c = 0; c < m; c += 1) {
            double temp = vector[swap_vec[i]*m + c];
                vector[swap_vec[i]*m + c] = vector[i*m + c];
                vector[i*m + c] = temp;
        }

    }
}

double row_norm(double* matrix, int n) {
    double max = 0;
    // if (n > 1000) {
    //     return 1e+7;
    // }
    for (int i = 0; i < n; i += 1) {
        double sum = 0;
        for (int j = 0; j < n; j += 1) {
            sum += fabs(matrix[i*n + j]);
        }
        max = max > sum ? max : sum;
    }
    return max;
}

double measurement(double* vector_b, int n) {
    double sum = 0;
    for (int i = 0; i < n; i += 1) {
        if (i % 2 == 0) {
            sum += fabs(vector_b[i] - 1);
        } else {
            sum += fabs(vector_b[i]);
        }
    }
    double devisor = 1;
    if (n%2 != 0) {
        devisor = n / 2 + 1;
    } else {
        devisor = n;
    }
    return sum / devisor;
}

double solve_measurement(double* matrix,double* vector_b, double* vector_result, int n) {
    double devisor = 0;
    double result_sum = 0;
    for (int i = 0; i < n; i += 1) {
        double sum = 0;
        for (int j = 0; j < n; j += 1) {
            sum += matrix[i * n + j]*vector_b[j];
        }
        devisor += fabs(vector_result[i]);
        vector_result[i] = sum - vector_result[i];
        result_sum += fabs(vector_result[i]);

    }
    return result_sum / devisor;
}

int solve_function(double* matrix, double* block_matrix, double* reverse_matrix, double* matrix_result, 
    int* list_swap, int* swap_vec, int n, int m, double* vector, double* vector_result, double matrix_norm) {
    int result = block_method_gauss(matrix, block_matrix, reverse_matrix, matrix_result,
        list_swap, swap_vec, n, m, vector, vector_result, matrix_norm);
    if (result == -1) {
        return -1;
    }
    reverse_block_gauss(matrix, block_matrix, reverse_matrix, matrix_result,
        list_swap, swap_vec, n, m, vector, vector_result, matrix_norm);
    return 0;
}

// Функция reduce_sum
void reduce_sum(int p, double* a, int n)
{
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
    static int t_in = 0;
    static int t_out = 0;
    static double *r = nullptr;
    int i;

    if (p <= 1) {
        return; // Нет необходимости синхронизации для одного потока
    }

    pthread_mutex_lock(&m);

    // Если это первый поток, инициализируем массив
    if (r == nullptr) {
        r = new double[n]();
        for (i = 0; i < n; i++) {
            r[i] = a[i];
        }
    } else {
        for (i = 0; i < n; i++) {
            r[i] += a[i];
        }
    }

    t_in++;
    if (t_in >= p) {
        t_out = 0;
        pthread_cond_broadcast(&c_in); // Сообщаем, что все потоки достигли этой точки
    } else {
        while (t_in < p) {
            pthread_cond_wait(&c_in, &m);
        }
    }

    if (r != a) {
        for (i = 0; i < n; i++) {
            a[i] = r[i]; // Обновляем массив для других потоков
        }
    }

    t_out++;
    if (t_out >= p) {
        t_in = 0;
        delete[] r; // Освобождаем память
        r = nullptr;
        pthread_cond_broadcast(&c_out);
    } else {
        while (t_out < p) {
            pthread_cond_wait(&c_out, &m);
        }
    }

    pthread_mutex_unlock(&m);
}
 
// void thread_function(double* matrix, double* block_matrix, int thread_num, int total_threads, int n, int m, double* vector, int step) {
//     double* block_matrix_thread = allocateMatrix(m);
//     double* proud_matrix_thread = allocateMatrix(m);
//     double* vector_result = new double[m];
//     int k = n / m;
//     if (n % m != 0) {
//         k += 1;
//     }
//     int interval = k / total_threads;
//     int ending = thread_num == total_threads ? thread_num*interval : k;
//     int beginner = (thread_num - 1)*interval < step ? step : (thread_num - 1)*interval;
//     for (int i = beginner; i < ending; i+= 1) {
//         if (beginner == step) {
//             continue;
//         }
//         block_matrix = get_block(matrix, block_matrix, n, m, i, step);
//         for (int j = step; j < k; j += 1) {
//             proud_matrix_thread = proud_of_matrix(block_matrix, get_block(matrix, block_matrix_thread, n, m, step, j), proud_matrix_thread);
//             sub_matrix(get_block(matrix, block_matrix_thread, n, m, i, j), proud_matrix_thread, m);
//             put_block(matrix, block_matrix_thread, n, m, i. j);
//             proud_vector(block_matrix, &vector[step*m], vector_result, m, m);
//             sub_vector(&vector[i*m], vector_result, m);
//         }
//     }

// } // надо добавить потчный поиск нормы вначало, после синхронизируем, двигаем наш элемент одним потоком, потом снова синхронизация и дальше вычисления

int thread_step_blocks_gauss(int total_thread, int thread_num, double *matrix, double *block_matrix, double *reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec, int n, int m, int step, double *vector, double* vector_result, double matrix_norm) {
    int k = n / m;
    int l = n - k*m;
    double* temp;
    int result = 0;
    thread_min_norm_transfer(total_thread, thread_num, matrix, block_matrix, reverse_matrix, matrix_result, list_swap, swap_vec, n, m, step, vector, matrix_norm);
    reduce_sum(total_thread);
    if (thread_num == 1) {
        matrix_with_one(reverse_matrix, m);
        block_matrix = get_block(matrix, block_matrix, n, m, step, step);
        result = method_gauss(block_matrix, reverse_matrix, list_swap, m, matrix_norm);
        reverse_method_gauss(block_matrix, reverse_matrix, list_swap, m);
        proud_vector(reverse_matrix, &vector[step*m], vector_result, m, m);
        put_vector(vector, vector_result, m, step, m);
    }
    reduce_sum(total_thread);
    int p = k / total_thread;
    int ending = thread_num == total_thread ? total_thread : p;
    if (n%m != 0) {
        k = k + 1;
    } else {
        l = m;
    } // добавленое изменение, так как остаточные блоки забываются
    // Эта строка вынесется в итоговой функции за потоки и верхняя часть тоже
    // for (int j = step; j < k; j += 1) {
    //     block_matrix = get_block(matrix, block_matrix, n, m, step, j);
    //     matrix_result = proud_of_matrix(reverse_matrix, block_matrix, matrix_result, m);
    //     put_block(matrix, matrix_result, n, m, step, j);
    // }  // предыдущая часть является последовательной программой, она будет идти перед потоковыми функциями
    for (int i = step + 1 + (thread_num -1)*p; i < ending; i += 1) {
        block_matrix = get_block(matrix, block_matrix, n, m, i, step);
        for (int row = 0; row < m ; row += 1) {
            for (int col = 0; col < m; col += 1) {
                reverse_matrix[row*m + col] = block_matrix[row*m + col];
            }
        }
        int greater = matrix_norm < 1e+39 ? 1 : 2;
        for (int v = 0; v < m; v += 1) {
            double sum = 0;
            for (int d = 0; d < m; d += greater) {
                sum += reverse_matrix[v*m + d]*vector[step*m + d];

            }
            vector_result[v] = sum;
        }
        // printMatrix(vector_result, m, 1, m);
        //proud_vector(reverse_matrix, &vector[step*m], vector_result, m);

        // put_vector(vector, vector_result, m, step);
        if (i == k - 1) {
            sub_vector(&vector[i*m], vector_result, l);
        } else {
            sub_vector(&vector[i*m], vector_result, m);
        }
        for (int j = step; j < k; j += 1) {
            if (j == step) {
                matrix_with_one(block_matrix, m);
                temp = block_matrix;
            } else {
                temp = get_block(matrix, block_matrix, n, m, step, j);
            }
            // из-за того что на e^+40 0 = e^-14 при умножении будет не 0, а довольно большое число надо написать умножение с учетом нормы
            matrix_result = proud_of_matrix(reverse_matrix, temp, matrix_result, m);
            sub_matrix(get_block(matrix, block_matrix, n, m, i, j), matrix_result, m);
            put_block(matrix, block_matrix, n, m, i, j); // используем reverse, чтобы лишний раз не копировать матрицу в proud
            // так как reverse в этой части функции уже не используется
        }
    }
    // printMatrix(matrix, n, n, n);
    // printMatrix(vector, n, 1, n);
    return result;
}

void thread_min_norm_transfer(int total_thread, int thread_num,double* matrix, double* block_matrix, double* reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec, int n, int m, int step, double* vector, double matrix_norm){
    int k = n / m;
    static double min_norm_all = 1e+40;
    int p = k / total_thread;
    double min_norm = 1e+40;
    double temp;
    int ending = thread_num == total_thread ? k : thread_num*p;
    int index[2] = {0, 0};
    for(int i = step + (thread_num- 1)*p; i < ending; i += 1) {
        for (int j = step; j < k; j += 1) { // замена 0 на step для ускорения и избежания ошибок
            block_matrix = get_block(matrix, block_matrix, n, m, i, j);
            for (int a = 0; a < m; a += 1) {
                for (int b = 0; b < m; b += 1) {
                    matrix_result[a*m + b] = block_matrix[a*m + b];
                }
            }
            matrix_with_one(reverse_matrix, m);
            temp = norm_reverse(matrix_result, reverse_matrix, list_swap, m, matrix_norm);
            if (temp < min_norm) {
                min_norm = temp;
                index[0] = i;
                index[1] = j;
            }
        }
    }
    mx.lock();
    if (min_norm < min_norm_all) {
        min_norm_all = min_norm;
    }
    mx.unlock();
    reduce_sum(total_thread);
    if (fabs(min_norm - min_norm_all) < 1e-9) {
        if (n%m != 0) {
            k += 1;
        }
        for (int i = step; i < k; i += 1){
            if (i == index[0]) {
                for (int j = 0; j < k; j += 1) {
                    block_matrix = get_block(matrix, block_matrix, n, m, i, j);
                    for (int a = 0; a < m; a += 1) {
                        for (int b = 0; b < m; b += 1) {
                            matrix_result[a*m + b] = block_matrix[a*m + b];
                        }
                    }
                    put_block(matrix,get_block(matrix, block_matrix, n, m, step, j), n, m, i, j);
                    put_block(matrix, matrix_result, n, m, step, j);           
                }
                for (int c =  0; c < m; c += 1) {
                    temp = vector[i*m + c];
                    vector[i*m + c] = vector[step*m + c];
                    vector[step*m + c] = temp;
                }
                break;
            }
        }
        for (int j = step; j < k; j += 1){ // Замена 0 на step
            if (j == index[1]) {
                for (int i = 0; i < k; i += 1) {
                    block_matrix = get_block(matrix, block_matrix, n, m, i, j);
                    for (int a = 0; a < m; a += 1) {
                        for (int b = 0; b < m; b += 1) {
                            matrix_result[a*m + b] = block_matrix[a*m + b];
                        }
                    }
                    put_block(matrix, get_block(matrix, block_matrix, n, m, i, step), n, m, i, j);
                    put_block(matrix, matrix_result, n, m, i, step);           
                }
                swap_vec[step] = index[1];
                break;
            }
        }
    }
    
}

int thread_block_method_gauss(int total_thread, int thread_num, double *matrix, double *block_matrix, double *reverse_matrix, double* matrix_result, int* list_swap, int* swap_vec,  int n, int m, double* vector, double* vector_result, double matrix_norm){
    int k = n / m;
    int result = 0;
    int l = n - k*m;
    for (int i = k; i > 0; i -= 1) {
        int j = k - i;
        result = thread_step_blocks_gauss(total_thread, thread_num, matrix, block_matrix, reverse_matrix, matrix_result, list_swap, swap_vec, n, m, j, vector, vector_result, matrix_norm);
        reduce_sum(total_thread);
    }
    if (result == -1) {
        return -1;
    }
    if (n % m != 0 && thread_num == total_thread) {
        matrix_with_one(reverse_matrix, m);
        block_matrix = get_block(matrix, block_matrix, n, m, k, k);
        for (int i = n - k *m; i > 0; i -= 1) {
            int j = n - k*m - i;
            result = step_gauss(block_matrix, reverse_matrix, list_swap, m, j, matrix_norm);
        }
        reverse_method_gauss(block_matrix, reverse_matrix, list_swap, m);
        // printMatrix(reverse_matrix, m, m, m);
        proud_vector(reverse_matrix, &vector[k*m], vector_result, m, l);
        put_vector(vector, vector_result, m, k, l); // заменял l на m в индексе строки
        proud_of_matrix(get_block(matrix, block_matrix, n, m, k, k), reverse_matrix, matrix_result, m); // добавил так как нет обратного хода и это важно
        put_block(matrix, matrix_result, n, m, k, k);  // было block_matrix
    }
    return result;
}

int thread_solve_function(int total_thread, double* matrix, double* block_matrix, double* reverse_matrix, double* matrix_result, 
    int* list_swap, int* swap_vec, int n, int m, double* vector, double* vector_result, double matrix_norm) {
    std::vector<std::thread> threads(total_threads);
    int result;
    for (int i = 0; i < total_thread; i += 1) {
        thread t(thread_block_method_gauss, total_thread, i + 1, matrix, block_matrix, reverse_matrix, matrix_result,
        list_swap, swap_vec, n, m, vector, vector_result, matrix_norm, std::ref(result));
        if (result == -1) {
            return -1;
        }
        t.join();
    }
    reverse_block_gauss(matrix, block_matrix, reverse_matrix, matrix_result,
        list_swap, swap_vec, n, m, vector, vector_result, matrix_norm);
    return 0;
}







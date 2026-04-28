#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>

int type_of_solve(double teta, double b, double x_min, double x_max, double y_min, double y_max);
double solve_y_input(double teta, double b, double x_min, double x_max, double y_min, double y_max);
double solve_y_out(double teta, double b, double x_min, double x_max, double y_min, double y_max);
double solve_x_input(double teta, double b, double x_min, double x_max, double y_min, double y_max);
double solve_x_out(double teta, double b, double x_min, double x_max, double y_min, double y_max);
int solve_index_x(double x, double len_pix, int N);
int solve_index_y(double y, double len_pix, int M);
double solve_for_quadro(double teta, double b, double x_min, double x_max, double y_min, double y_max);

// Имеем входные данные из файла такие как:
//Размерности матрицы M и N
//матрица кэф. ослабления M на N
//Размер пикселя в физических единицах
//вектор координат центров пикселей по X и по Y от мин. к макс.(эту часть решил убрать, проще их быстро рассчитать в программе)
// Результатом будет матрица синусограммы, вектор углов поворота, вектор координат детекторов
int main() {
    // Часть под считывание данных
    std::ifstream file("phantom_size_128.txt");  // Открываем файл
    
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл!" << std::endl;
        return 1;
    }
    int N;
    int M;
    file >> M >> N;
    double len_pix;
    double* matrix_input = new double[M * N];
    for (int i = 0; i < M; i+= 1) {
        for (int j = 0; j < N; j+= 1) {
            file >> matrix_input[i*N + j];
        }
    }
    file >> len_pix;
    file.close();
    // Определяем параметры сканирования
    double* x_vec = new double[N];
    for (int i = 1; i < N + 1; i += 1) {
        x_vec[i - 1] = (i - (N + 1) / 2.0) * len_pix;
    }
    double* y_vec = new double[M];
    for (int i = 1; i < M + 1; i += 1) {
        y_vec[i - 1] = ((M + 1) / 2.0 - i) * len_pix;
    }
    double len_matrix = std::max(N, M) * len_pix; // пока заглушка тут введем рассчет длине матрицы
    int num_det = 400; // диапазон от 100 до 500
    int num_angle = 60; // здесь определяем число поворотов
    double delta_angle = M_PI / num_angle; // соответственно угол поворота
    double d_num_det = 400.0;
    double max_det_cord = sqrt(2)*len_matrix / 2.0;
    double delt_det = 2.0 * max_det_cord / (d_num_det - 1); // шаг между детекторами
    double* matrix_res = new double[num_angle * num_det](); // результирующая матрица
    double x_min = x_vec[0] - len_pix / 2.0; // границы томограммы
    double x_max = x_vec[N - 1] + len_pix / 2.0;
    double y_min = y_vec[M - 1] - len_pix / 2.0;
    double y_max = y_vec[0] + len_pix / 2.0;

    auto start = std::chrono::high_resolution_clock::now();

    // используем уравнение прямой и вычислим
    for(int i_angle = 0; i_angle < num_angle; i_angle += 1) {
        double teta = delta_angle*i_angle;
        // std::cout << teta << std::endl;
        double k = -1 * cos(teta) / sin(teta);
        int count_det = -1;
        for(double i_det = -max_det_cord; i_det < max_det_cord + delt_det; i_det += delt_det){
            count_det += 1;
            double b = i_det / sin(teta);
            // далее пойдет опредление пересечения с границами томограммы
            // соответственно определяем по четырем сторонам
            int code = type_of_solve(teta, b, x_min, x_max, y_min, y_max);
            // std::cout << code << " " << i_angle << std::endl;
            // std::cout << code << " " << i_angle << std::endl;
            // if (i_angle > 31 && i_angle < 36) {
            //     std::cout << code << " " << i_angle << std::endl;
            // }
            if (code == 7) {
                if (x_max > i_det && i_det > x_min) {
                    //std::cout << " Я ТОТОЧКИ 7" << std::endl;
                    int x_out = solve_index_x(i_det, len_pix, N);
                    for (int id = 0; id < M; id += 1) {
                        matrix_res[i_angle*num_det + count_det] += matrix_input[id*N + x_out]*len_pix;
                    }
                }
                continue;
            }
            if (code == 8) {
                // std::cout << " Я ТОТОЧКИ 8" << std::endl;
                if (y_max > i_det && i_det > y_min) {
                    int y_out = solve_index_y(i_det, len_pix, M);
                    for (int id = 0; id < N; id += 1) {
                        matrix_res[i_angle*num_det + count_det] += matrix_input[y_out*N + id]*len_pix;
                    }
                }
                continue;
            }
            if (code == 1) {
                double y_input = solve_y_input(teta, b, x_min, x_max, y_min, y_max);
                int y_input_index = solve_index_y(y_input, len_pix, M);
                double y_out = solve_y_out(teta, b, x_min, x_max, y_min, y_max);
                int y_out_index = solve_index_y(y_out, len_pix, M);
                // std::cout << y_out_index << " " << y_input_index << " "<< std::endl;
                if (y_input_index > y_out_index) {
                    int temp = y_input_index;
                    y_input_index = y_out_index;
                    y_out_index = temp;
                }
                double sum = 0;
                for(int ind_y = y_input_index; ind_y <= y_out_index; ind_y += 1) {
                    double sum_y = 0;
                    for (int ind_x = 0; ind_x < N; ind_x += 1) {
                        double l = solve_for_quadro(teta, b, x_vec[ind_x] - len_pix / 2.0, x_vec[ind_x] + len_pix / 2.0, y_vec[ind_y] - len_pix / 2.0, y_vec[ind_y] + len_pix / 2.0);
                        if (l < 1e-8 && sum_y > 1e-9) {
                            break;
                        } // Проверяем это ускорение, пока не ускорило но верно, попробую добавить в другие
                        //std::cout << l << " " << ind_y << " " << ind_x << std::endl;
                        sum_y += l*matrix_input[ind_y*N + ind_x];
                    }
                    sum += sum_y;
                }
                // std::cout << " Я ТОТОЧКИ 1" << std::endl;
                matrix_res[i_angle*num_det + count_det] = sum;
                continue;
                //Здесь все корректно
            }
            if (code == 4) {
                double x_input = solve_x_input(teta, b, x_min, x_max, y_min, y_max);
                int x_input_index = solve_index_x(x_input, len_pix, N);
                double x_out = solve_x_out(teta, b, x_min, x_max, y_min, y_max);
                int x_out_index = solve_index_x(x_out, len_pix, N);
                double sum = 0;
                if (x_input_index > x_out_index) {
                    int temp = x_input_index;
                    x_input_index = x_out_index;
                    x_out_index = temp;
                }
                for(int ind_y = 0; ind_y < M; ind_y += 1) {
                    double sum_y = 0;
                    for (int ind_x = x_input_index; ind_x <= x_out_index; ind_x += 1) {
                        double l = solve_for_quadro(teta, b, x_vec[ind_x] - len_pix / 2.0, x_vec[ind_x] + len_pix / 2.0, y_vec[ind_y] - len_pix / 2.0, y_vec[ind_y] + len_pix / 2.0);
                         if (l < 1e-8 && sum_y > 1e-9) {
                            break;
                        }
                        // std::cout <<  << " " <<  << std::endl;
                        sum_y += l*matrix_input[ind_y*N + ind_x];
                    }
                    sum += sum_y;
                }
                matrix_res[i_angle*num_det + count_det] = sum;
                continue;
                //Аналогично все корректно
            }
            if (code == 2) {
                // std::cout << " Я ТОТОЧКИ 2" << std::endl;
                double y_input = solve_y_input(teta, b, x_min, x_max, y_min, y_max);
                int y_input_index = solve_index_y(y_input, len_pix, M);
                double x_out = solve_x_out(teta, b, x_min, x_max, y_min, y_max);
                int x_out_index = solve_index_x(x_out, len_pix, N);
                double sum = 0;
                for(int ind_y = 0; ind_y < M; ind_y += 1) {
                    double sum_y = 0;
                    for (int ind_x = 0; ind_x < N; ind_x += 1) {
                        double l = solve_for_quadro(teta, b, x_vec[ind_x] - len_pix / 2.0, x_vec[ind_x] + len_pix / 2.0, y_vec[ind_y] - len_pix / 2.0, y_vec[ind_y] + len_pix / 2.0);
                         if (l < 1e-8 && sum_y > 1e-9) {
                            break;
                        }
                        sum_y += l*matrix_input[ind_y*N + ind_x];
                    }
                    sum += sum_y;
                }
                
                matrix_res[i_angle*num_det + count_det] = sum;
                continue;
                //Здесь почему то надо полный цикл

            }
            if (code == 3) {
                double x_input = solve_x_input(teta, b, x_min, x_max, y_min, y_max);
                int x_input_index = solve_index_x(x_input, len_pix, N);
                double y_out = solve_y_out(teta, b, x_min, x_max, y_min, y_max);
                int y_out_index = solve_index_y(y_out, len_pix, M);
                double sum = 0;
                for(int ind_y = 0; ind_y < M; ind_y += 1) {
                    double sum_y = 0;
                    for (int ind_x = 0; ind_x < N; ind_x += 1) {
                        double l = solve_for_quadro(teta, b, x_vec[ind_x] - len_pix / 2.0, x_vec[ind_x] + len_pix / 2.0, y_vec[ind_y] - len_pix / 2.0, y_vec[ind_y] + len_pix / 2.0);
                         if (l < 1e-8 && sum_y > 1e-9) {
                            break;
                        }
                        sum_y += l*matrix_input[ind_y*N + ind_x];
                    }
                    sum += sum_y;
                }
                // std::cout << " Я ТОТОЧКИ 3 " << std::endl;
                matrix_res[i_angle*num_det + count_det] = sum;
                continue;
                //Нужен полный цикл
            }
            if (code == 5) {
                double y_input = solve_y_input(teta, b, x_min, x_max, y_min, y_max);
                int y_input_index = solve_index_y(y_input, len_pix, M);
                double x_out = solve_x_out(teta, b, x_min, x_max, y_min, y_max);
                int x_out_index = solve_index_x(x_out, len_pix, N);
                double sum = 0;
                for(int ind_y = 0; ind_y < M; ind_y += 1) {
                    double sum_y = 0;
                    for (int ind_x = 0; ind_x < N; ind_x += 1) {
                        double l = solve_for_quadro(teta, b, x_vec[ind_x] - len_pix / 2.0, x_vec[ind_x] + len_pix / 2.0, y_vec[ind_y] - len_pix / 2.0, y_vec[ind_y] + len_pix / 2.0);
                         if (l < 1e-8 && sum_y > 1e-9) {
                            break;
                        }
                        sum_y += l*matrix_input[ind_y*N + ind_x];
                    }
                    sum += sum_y;
                }
                // std::cout << " Я ТОТОЧКИ 5" << std::endl;
                matrix_res[i_angle*num_det + count_det] = sum;
                continue;
                //Нужен полный цикл, видимо и в 6 тоже
            }
            if (code == 6) {
                // std::cout << " Я ТОТОЧКИ 6" << std::endl;
                double y_out = solve_y_out(teta, b, x_min, x_max, y_min, y_max);
                int y_out_index = solve_index_y(y_out, len_pix, M);
                double x_input = (y_max - b) / k;
                int x_input_index = solve_index_x(x_input, len_pix, N);
                double sum = 0;
                for(int ind_y = 0; ind_y < M; ind_y += 1) {
                    double sum_y = 0;
                    for (int ind_x = 0; ind_x < N; ind_x += 1) {
                        double l = solve_for_quadro(teta, b, x_vec[ind_x] - len_pix / 2.0, x_vec[ind_x] + len_pix / 2.0, y_vec[ind_y] - len_pix / 2.0, y_vec[ind_y] + len_pix / 2.0);
                         if (l < 1e-8 && sum_y > 1e-9) {
                            break;
                        }
                        sum_y += l*matrix_input[ind_y*N + ind_x];
                    }
                    sum += sum_y;
                }
                matrix_res[i_angle*num_det + count_det] = sum;
                continue;
            }
            ///ПОМЕНЯЛ ВСЕ ПРЕДЕЛЫ В ЦИКЛАХ ОТ НАЧАЛА ДО САМОГО КОНЦА ДЛЯ ОТЛАДКИ

        }
            
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::ofstream out_file("out.txt");  // Открываем файл для записи

    if (!out_file.is_open()) {
        std::cerr << "Ошибка: не удалось создать файл out.txt" << std::endl;
        return 1;
    }
    out_file << num_angle << " " << num_det << " " << max_det_cord << std::endl;

    // Записываем матрицу синусограммы
    for (int i = 0; i < num_angle; i++) {
        for (int d = 0; d < num_det; d++) {
            out_file << matrix_res[i * num_det + d];
            if (d < num_det - 1) {
                out_file << " ";  // пробел между элементами (кроме последнего)
            }
        }
        out_file << std::endl;  // новая строка после каждого угла
    }

    out_file.close();
    delete[] matrix_input;
    delete[] x_vec;
    delete[] y_vec;
    delete[] matrix_res;

    std::chrono::duration<double> duration_sec = end - start;
    std::cout << "Время выполнения: " << duration_sec.count() << " с" << std::endl;

}

// реализация некоторых функция для решения
int type_of_solve(double teta, double b, double x_min, double x_max, double y_min, double y_max) {
    ///
    ///Данная функция будет выдавать тип прохождения нашей прямой через томограммуу
    ///Всего их 6 - описаны далее
    ///
    if (fabs(sin(teta)) < 1e-12) {
        return 7;
    }
    if (fabs(cos(teta)) < 1e-12) {
        return 8;
    }
    double k = -cos(teta) / sin(teta);
    if (y_max >= (x_min * k + b) && (x_min * k + b) > y_min && (x_max*k + b) < y_max && (x_max*k + b) >= y_min) {
        return 1; // вошла через левую стенку вышла через правую
    }
    if (y_max >= (x_min * k + b) && (x_min * k + b) > y_min && ((y_max - b) / k) < x_max && ((y_max - b) / k) >= x_min) {
        return 2; // вошла через левую вышла через верхнюю
    }
    if (x_max >= ((y_min - b) / k) && ((y_min - b) / k) > x_min && (x_max*k + b) < y_max && (x_max*k + b) >= y_min) {
        return 3; //вошла через нижнюю вышла через правую
    }
    if (x_max >= ((y_min - b) / k) && ((y_min - b) / k) > x_min && ((y_max - b) / k) < x_max && ((y_max - b) / k) >= x_min) {
        return 4; // вошла через нижнюю вышла через верхнюю
    }
    if (y_max >= (x_min * k + b) && (x_min * k + b) > x_min && x_max >= ((y_min - b) / k) && ((y_min - b) / k) > x_min) {
        return 5; //вошла через левую и вышла через нижнюю
    }
    if (((y_max - b) / k) < x_max && ((y_max - b) / k) >= x_min && (x_max*k +b) < y_max && (x_max*k +b) >= y_min) {
        return 6; // вошла через верхнюю и вышла через правую
    }
    return 0; // нет пересечений
}
double solve_y_input(double teta, double b, double x_min, double x_max, double y_min, double y_max) {
    ///
    ///Данная функция определяет точку вхождения по y для случаев 1,2,5
    ///
    double k = -cos(teta) / sin(teta);
    return x_min*k + b;
}
double solve_y_out(double teta, double b, double x_min, double x_max, double y_min, double y_max) {
    ///
    ///Данная функция определяет точку выхода по y для случаев 1,3,6
    ///
    double k = -cos(teta) / sin(teta);
    return x_max*k + b;
}

double solve_x_input(double teta, double b, double x_min, double x_max, double y_min, double y_max) {
    ///
    ///Данная функция определяет точку вхождения по x для случаев 3,4
    ///
    double k = -cos(teta) / sin(teta);
    return (y_min - b) / k;
}

double solve_x_out(double teta, double b, double x_min, double x_max, double y_min, double y_max) {
    ///
    ///Данная функция определяет точку выхода по x для случаев 2,4
    ///
    double k = -cos(teta) / sin(teta);
    return (y_max - b) / k;
}
//Важно, что функции выше также могут применяться для поиска пересечения внутри конкретного квадратика
int solve_index_x(double x, double len_pix, int N) {
    /// Определение индекса по x через координаты
    return x / len_pix + (N + 1) / 2.0 - 1;
}
int solve_index_y(double y, double len_pix, int M) {
    /// Определение иднекса по y через координаты
    return -y / len_pix + (M + 1) / 2.0 - 1;
}

double solve_for_quadro(double teta, double b, double x_min, double x_max, double y_min, double y_max) {
    /// Данная функция будет полностью решать(находить точки пересечения)
    ///В конкретном квадратике, задаем его переменными x_min и тд
    ///Просто подставим в них края конкретного квадрата
    /// выдает длинну в квадратике
    double k = -1 * cos(teta) / sin(teta);
    int code = type_of_solve(teta, b, x_min, x_max, y_min, y_max);

    if (fabs(sin(teta)) < 1e-12) return (x_max - x_min);

    if (fabs(cos(teta)) < 1e-12) return (y_max - y_min);

    if (code == 1) {
        double y_input = solve_y_input(teta, b, x_min, x_max, y_min, y_max);
        double y_out = solve_y_out(teta, b, x_min, x_max, y_min, y_max);
        return sqrt((x_max - x_min)*(x_max - x_min) + (y_out - y_input)*(y_out - y_input));
    }
    if (code == 2) {
        double y_input = solve_y_input(teta, b, x_min, x_max, y_min, y_max);
        double x_out = solve_x_out(teta, b, x_min, x_max, y_min, y_max);
        return sqrt((x_out - x_min)*(x_out - x_min) + (y_max - y_input)*(y_max - y_input));
    }
    if (code == 3) {
        double x_input = solve_x_input(teta, b, x_min, x_max, y_min, y_max);
        double y_out = solve_y_out(teta, b, x_min, x_max, y_min, y_max);
        return sqrt((x_max - x_input)*(x_max - x_input) + (y_out - y_min)*(y_out - y_min));
    }
    if (code == 4) {
        double x_input = solve_x_input(teta, b, x_min, x_max, y_min, y_max);
        double x_out = solve_x_out(teta, b, x_min, x_max, y_min, y_max);
        return sqrt((y_max - y_min)*(y_max - y_min) + (x_out - x_input)*(x_out - x_input));
    }
    if (code == 5) {
        double y_input = solve_y_input(teta, b, x_min, x_max, y_min, y_max);
        double x_out = solve_x_out(teta, b, x_min, x_max, y_min, y_max);
        return sqrt((y_min - y_input)*(y_min - y_input) + (x_out - x_min)*(x_out - x_min));
    }
    if (code == 6) {
        double x_input = (y_max - b) / k;
        double y_out = solve_y_out(teta, b, x_min, x_max, y_min, y_max);
        return sqrt((x_max - x_input)*(x_max - x_input) + (y_max - y_out)*(y_max - y_out));
    }
    //Скорее всего что-то не так здесь
    return 0;
    ///Возвращает длинну отрезка в квадратике

}
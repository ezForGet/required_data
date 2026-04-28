from math import pi, sqrt, cos, sin, floor
import numpy as np
import matplotlib
matplotlib.use('TkAgg')  # или 'Qt5Agg'
import matplotlib.pyplot as plt
name_file = input()
with open(name_file) as file:
    t = file.readline().rstrip().split()
    matrix = []
    M = int(t[0])
    N = int(t[1])
    num_angel = int(file.readline().rstrip())
    num_det = int(file.readline().rstrip())
    delta = float(file.readline().rstrip())
    for i in range(num_angel):
        matrix.append([float(x) for x in file.readline().rstrip().split()])
angel_step = pi / num_angel
vec_angel = [x * angel_step for x in range(num_angel)]
det_step = sqrt(2) * N * delta / (num_det - 1)
vec_det = [-1 * sqrt(2) / 2 * N * delta + det_step*x for x in range(num_det)]
N_x = N
N_y = M
x = [0.0] * N_x
y = [0.0] * N_y
for i in range(0, N_x):
    x[i] = (-1 * N_x / 2 + 0.5 + (i + 1 - 1)) * delta
    y[i] = (-1 * N_y / 2 + 0.5 + (i + 1 - 1)) * delta
def sum_g(i, j, x, y, vec_angel, vec_det, matrix, M, det_step):
    result = 0
    for k in range(len(vec_angel)):
        ksi = x[j] * cos(vec_angel[k]) + y[i] * sin(vec_angel[k])
        idx = floor((ksi - vec_det[0]) / det_step) + 1
        p = matrix[k][idx] #находим по ближайшему соседу
        result += p
    return result / M
matrix_g = [[0.0] * N_x for _ in range(N_y)]
for i in range(N_y):
    for j in range(N_x):
        matrix_g[i][j] = sum_g(i, j, x, y, vec_angel, vec_det, matrix, num_angel, det_step)
N_p = N_x
delta_p = 1 / N_p / delta
p_x = [delta_p * x for x in range(-N_p // 2, N_p // 2)]
p_y = [delta_p * x for x in range(-N_p // 2, N_p // 2)]
# ... ваш существующий код до создания G ...

# Преобразуем matrix_g в numpy array (важно!)
matrix_g_np = np.array(matrix_g, dtype=float)

# 5.4 Двумерное преобразование Фурье
G_shifted = np.fft.fftshift(np.fft.fft2(matrix_g_np))

# Нормировка (исправленный вариант)
G_physical = G_shifted * (delta * delta) / np.sqrt(N_x * N_y)

# 5.3 Частотные сетки (нужны для фильтра)
N_rho = N_x
d_rho = 1 / (N_rho * delta)

rho_x = np.arange(-N_rho // 2, N_rho // 2) * d_rho
rho_y = np.arange(-N_rho // 2, N_rho // 2) * d_rho

# Создаём 2D сетки для фильтра
RHOx, RHOy = np.meshgrid(rho_x, rho_y)
rho = np.sqrt(RHOx**2 + RHOy**2)

# 5.5 Применение ρ-фильтра и аподизации
# Обнуляем центр (чтобы избежать деления на ноль)
rho_center_y = N_rho // 2
rho_center_x = N_rho // 2
rho[rho_center_y, rho_center_x] = 1  # временно, чтобы избежать nan

# Фильтр ρ/2
filter_rho = rho / 2

# Аподизирующая функция (прямоугольное окно = 1)
A_2d = np.ones((N_rho, N_rho))

# Применяем фильтр
M_filtered = G_physical * filter_rho * A_2d

# Возвращаем ноль в центр (если нужно)
#M_filtered[rho_center_y, rho_center_x] = 0

# 5.6 Обратное двумерное преобразование Фурье
# Сначала возвращаем квадранты на место (ifftshift)
M_shifted_back = np.fft.ifftshift(M_filtered)

# Обратное БПФ
mu_reconstructed_full = np.fft.ifft2(M_shifted_back)
mu_reconstructed_full = np.real(mu_reconstructed_full) * (d_rho**2) * np.sqrt(N_x * N_y)

# 5.7 Обрезка границ
# crop = 0
#mu_reconstructed = mu_reconstructed_full[crop:-crop, crop:-crop]

# Визуализация (если есть matplotlib)
import matplotlib.pyplot as plt

plt.figure(figsize=(8, 8))
plt.imshow(mu_reconstructed_full, cmap='gray')
plt.colorbar()
plt.title('Восстановленная томограмма')
plt.xlabel('x, px')
plt.ylabel('y, px')
plt.show()

# Сохранение результата
import scipy.io as sio # type: ignore
sio.savemat('reconstructed.mat', {'mu_reconstructed': mu_reconstructed_full})

# После получения mu_reconstructed

# Сохраняем как текстовый файл с разделителями (пробелы)
np.savetxt('tomogram.txt', mu_reconstructed_full, fmt='%.6f', delimiter=' ')

print("Томограмма сохранена в tomogram.txt")

print("Готово! Размер восстановленного изображения:", mu_reconstructed_full.shape)

    






        

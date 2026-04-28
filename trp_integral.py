from math import log, cos, sqrt, sin
a = 1
b = 3
def f(x):
    return cos(log(x))
def f_dir_max(a, b):
    df_2 = lambda x : sin(log(x)) / x**2 - cos(log(x)) / x**2
    h = (b - a) / 0.05
    l = [a + 0.05*num for num in range(int(h + 1))]
    return abs(max(list(map(df_2, l)), key=abs))
        
r = float(input()) # вводим нужную погрешность
h = sqrt(r * 12 / (b - a) / f_dir_max(a, b))
integral_sum = 0
x = float(a)
while x <= b:
    x += h
    integral_sum += (f(x) + f(x - h)) / 2 * h
print(f"Интегральная сумма : {integral_sum}")
print(f"Остаточный член : {r}")
print(f"Шаг интегрирования : {h}")
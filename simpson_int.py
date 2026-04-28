from math import log, cos, sqrt, sin
a = 1
b = 3
def f(x):
    return x / sin(x)**2
def f_dir_max(a, b):
    df_2 = lambda x : (4 * x * cos(2*x)**2 + 52 * x * cos(2*x) + 64*x - 40*sin(2*x) - 4 * sin(4*x)) / sin(x)**6
    h = (b - a) / 0.001
    l = [a + 0.001*num for num in range(int(h))]
    l.append(df_2(3))
    l.append(df_2(1))
    return abs(max(list(map(df_2, l)), key=abs))
        
r = float(input()) # вводим нужную погрешность
h = (r * 180 / (b - a) / f_dir_max(a, b))**(1 / 4)
integral_sum_odd = 0
integral_sum_even = 0
x = 1.0
num = int((b - a) / h)
if num % 2 != 0:
    num += 1
    h = (b - a) / num
count = 1
while count < num:
    x += h
    if count % 2 == 1:
        integral_sum_odd += f(x)
    else:
        integral_sum_even += f(x)
    count += 1
result = h / 3 * (f(a) + f(b) + 4 * integral_sum_odd + 2 * integral_sum_even)
print(f"Интегральная сумма : {result}")
print(f"Остаточный член : {r}")
print(f"Шаг интегрирования : {h}")
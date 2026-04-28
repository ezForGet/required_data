n = int(input())
matrix = []
flag = True
quadro = [x for x in range(1, n**2 + 1)]
for i in range(n):
    matrix.append([int(x) for x in input().split()])
total = sum(matrix[0])
sum1 = 0
sum2 = 0
for i in range(n):
    sum1 += matrix[i][i]
    sum2 += matrix[n - 1 - i][i]
if sum1 != total or sum2 != total:
    flag = False
sum1 = 0
sum2 = 0
for i in range(n):
    for j in range(n):
        sum1 += matrix[i][j]
        sum2 += matrix[j][i]
        if matrix[i][j] in quadro:
            quadro.remove(matrix[i][j])
    if sum1 != total or sum2 != total:
        flag = False
        break
    sum1 = 0
    sum2 = 0
if flag and len(quadro) == 0:
    print('YES')
else:
    print('NO')
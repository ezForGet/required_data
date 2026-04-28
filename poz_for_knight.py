matrix = [['.'] * 8 for _ in range(8)]
s = input()
ind_j = ord(s[0]) - 97
ind_i = 8 - int(s[1])
matrix[ind_i][ind_j] = 'N'
if ind_i - 2 >= 0:
    if ind_j - 1 >= 0:
        matrix[ind_i - 2][ind_j - 1] = '*'
    if ind_j + 1 <= 7:
        matrix[ind_i - 2][ind_j + 1] = '*'
if ind_i + 2 <= 7:
    if ind_j - 1 >= 0:
        matrix[ind_i + 2][ind_j - 1] = '*'
    if ind_j + 1 <= 7:
        matrix[ind_i + 2][ind_j + 1] = '*'
if ind_j + 2 <= 7:
    if ind_i - 1 >= 0:
        matrix[ind_i - 1][ind_j + 2] = '*'
    if ind_i + 1 <= 7:
        matrix[ind_i + 1][ind_j + 2] = '*'
if ind_j - 2 >= 0:
    if ind_i - 1 >= 0:
        matrix[ind_i - 1][ind_j - 2] = '*'
    if ind_i + 1 <= 7:
        matrix[ind_i + 1][ind_j - 2] = '*'
for i in range(8):
    for j in range(8):
        print(matrix[i][j], end=' ')
    print()
    
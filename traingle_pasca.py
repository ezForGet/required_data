def pascal(n):
    if n == 0:
        print(1)
    elif n == 1:
        print(1)
    else:
        list1 = [[1], [1, 1]]
        for i in range(2, n + 1):
            list1.append([])
            for j in range(i + 1):
                if j == 0 or j == i:
                    list1[i].append(1)
                else:
                    list1[i].append(list1[i - 1][j - 1] + list1[i - 1][j])
        for d in range(n):
            for k in range(len(list1[d])):
                print(list1[d][k], end=' ')
            print()
n = int(input())
pascal(n)
                    
        
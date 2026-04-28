my_dict = {1 : '.,?!:',
2 : 'ABC',
3 : 'DEF',
4 : 'GHI',
5 : 'JKL',
6 : 'MNO',
7 : 'PQRS',
8 : 'TUV',
9 : 'WXYZ',
0 : ' '
}
s = input()
v = sorted(list(my_dict.values()))
ind = sorted(list(my_dict.keys()))
for i in s:
    for j in v:
        if i.lower() in j.lower():
            count = j.lower().index(i.lower())
            number = v.index(j)
            print(str(ind[number]) * (count + 1), end='')
    
    
    

def create_dict(name, value):
    mydict = {}
    for i in range(len(name)):
        mydict[name[i]] = value[i]
    return mydict

def read_csv(filename):
    with open(filename) as file:
        name = file.readline().strip().split(',')
        values = [x.strip().split(',') for x in file.readlines()]
        res_dict = []
        for i in range(len(values)):
            res_dict.append(create_dict(name, values[i]))
        return res_dict
filename = input()
print(read_csv(filename))
  
        
    
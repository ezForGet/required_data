import random
def generate_password(length):
    password = ''
    for _ in range(length):
        j = random.randint(97, 122)
        reg = random.randint(0,1)
        if reg == 0:
            password += chr(j).lower()
        else:
            password += chr(j).upper()
    return password
        
            
    
    

    

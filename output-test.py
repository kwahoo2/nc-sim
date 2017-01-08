import csv
with open("steps.dat", "rb") as f:
    byte = f.read(1)
    x = 0
    y = 0
    z = 0
    list = []
    while byte:
        step = ord(byte)
        #print(str(step))
        if (step & 0b10000000) == 0:
            if step & 0b00000001:
                x +=1
            if step & 0b00001000:
                x -= 1
            if step & 0b00000010:
                y += 1
            if step & 0b00010000:
                y -= 1
            if step & 0b00000100:
                z += 1
            if step & 0b00100000:
                z -= 1
        # Do stuff with byte.
        #print(str(x)+" "+str(y)+" "+str(z))
        line = [x, y, z]
        list.append(line)
        byte = f.read(1)
    

print(list)
with open('outputfromsteps.asc', 'w') as out:
    out_csv = csv.writer(out, delimiter = ' ')
    out_csv.writerows(list)

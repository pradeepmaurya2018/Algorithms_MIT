import msvcrt

print ("press 'escape' to quit...")

while 1:
    char = msvcrt.getch()
    if char == chr(27):
        break
    print (char)
    print("There")

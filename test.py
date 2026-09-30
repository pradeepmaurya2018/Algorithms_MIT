def add(x,y):
    return x + y

def subtract(x,y):
    return x -y

def multiply(x,y):
    return x * y

def divide(x,y):
    return x / y

print("Select Operation")
print("1 add")
print("2 subtract")
print("3 multiply")
print("4 divide")
c=0
while c<1:

    choice = input("Enter choice(1/2/3/4):")
    print(choice)

    if choice in ('1','2','3','4'):
        try:
            num1 = float(input("Enter first number: "))
            num2 = float(input("Enter second number: "))
        except ValueError:
            print("Invalid Input. Please enter a number.")
            continue
    if choice == '1':
        print(num1, "+", num2, "=", add(num1,num2))

    elif choice == '2':
        print(num1, "-", num2, "=", subtract(num1,num2))

    elif choice == '3':
        print(num1, "*", num2, "=", multiply(num1,num2))

    elif choice == '4':
        print(num1, "/", num2, "=", divide(num1,num2))

    next_calculation = input("Let's do next calculation?(Yes/No):")

    if next_calculation == "No":
        break
    else:
        print("Invalid input")
    c+=1
def exception():
    try:
        a=5
        b=0
        # ans=a/b
        # print(ans)
        raise ValueError("This is ans exception")
    except ValueError as e:
        print(f"Value error {e}")

    except Exception as e:
        print("Final")
    finally:
        print("this is finally block")




if __name__=="__main__":
    exception()
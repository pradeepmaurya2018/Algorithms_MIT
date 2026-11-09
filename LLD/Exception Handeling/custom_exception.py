class CustomException(Exception):
    def __init__(self, exc) -> None:
        super().__init__(exc)



if __name__ == "__main__":
    try:
        raise CustomException("This is exc")
    except Exception as e:
        print(e)
    # custom_exception=CustomException()

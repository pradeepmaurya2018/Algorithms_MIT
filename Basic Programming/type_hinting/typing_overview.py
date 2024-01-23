import collections
from typing import Optional, Union


def returnString(string:str)-> str:
    return string+string
def returnList(listt:list)->list:
    return listt+listt

class Student:
    pass
def returnIntOrNone()->Optional[int,str]:
    return Student()

# print(returnString("pradeep"))
# print(returnList([1, 24, 1]))
print(returnIntOrNone())

age:int=20
color:str="red"
ammount:int=3
name:Union[int,float]="name"

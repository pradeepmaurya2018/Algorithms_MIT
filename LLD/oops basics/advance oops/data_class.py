from dataclasses import dataclass

@dataclass
class Node:
    val:int
    next:Node

node=Node(val=3,next=None)
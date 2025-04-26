import heapq


class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next

    def __repr__(self):  # Optional, for easier debugging
        return f"ListNode({self.val})"


def mergeKLists(lists):
    # Min-heap
    heap = []

    # Push the head of each list into the heap
    for head in lists:
        if head:
            heapq.heappush(heap, (head.val, head))  # Tuple with value and ListNode

    # Dummy node to help build the merged list
    dummy = ListNode()
    current = dummy

    # Process the heap
    while heap:
        val, node = heapq.heappop(heap)  # Pop the smallest item
        current.next = node
        current = current.next

        # If the popped node has a next, push it into the heap
        if node.next:
            heapq.heappush(heap, (node.next.val, node.next))

    return dummy.next


def create_linked_list(values):
    dummy = ListNode()
    current = dummy
    for val in values:
        current.next = ListNode(val)
        current = current.next
    return dummy.next

# Helper function to print a linked list
def print_linked_list(head):
    result = []
    while head:
        result.append(head.val)
        head = head.next
    print(result)

# Example input
lists = [
    create_linked_list([1, 4, 5]),
    create_linked_list([1, 3, 4]),
    create_linked_list([2, 6])
]

# Merge the lists
merged_head = mergeKLists(lists)

# Output the result
print_linked_list(merged_head)

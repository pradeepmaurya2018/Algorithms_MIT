from data_structures_and_algorithms.abstract_data_type.intermediate_data_structure.Trees.BinaryTree.BasicTree import \
    TreeNode


def printAllSubtree(root):
    if root is None:
        return ""

    left = printAllSubtree(root.left)
    right = printAllSubtree(root.right)
    print(left, right)
    total = f"{left} {root.val}  {right}"
    # print(total)
    return total
    # if len(left)<len(right):
    #     return right
    # return left


root = TreeNode("Chandra")
root.left = TreeNode("Bundusara")
root.right = TreeNode("Ashoka")
root.left.left = TreeNode("Ramavardana")
root.left.right = TreeNode("Salishukla")
root.right.left = TreeNode("Brihdratha")
root.right.right = TreeNode("Stadhugna")
printAllSubtree(root)

//
// Created by 2025 on 3/26/2026.
//

#include "../../header.h"

struct tnode {
    int data;
    tnode* left;
    tnode* right;

};

void inorder(auto* root) {
    if (!root) return;
    cout<<(root->data);
    inorder(root->left);
    inorder(root->right);

}

int main() {
    tnode* root=new tnode(0, nullptr, nullptr);
    root->left=new tnode(1,nullptr, nullptr);
    root->right=new tnode(2,nullptr, nullptr);

    root->left->left=new tnode(1,nullptr, nullptr);
    root->left->right=new tnode(2,nullptr, nullptr);

    root->right->left=new tnode(4,nullptr, nullptr);
    root->right->right=new tnode(9,nullptr, nullptr);
    inorder(root);

}

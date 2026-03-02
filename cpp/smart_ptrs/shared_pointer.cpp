//
// Created by 2025 on 2/26/2026.
//
struct Node {
    int data;
    Node(int data) {
        this->data=data;
    }
};

class SharePointer {
public:
    int ref_cnt=0;
    Node* node=nullptr;
    SharePointer(SharePointer&)=delete;
    SharePointer operator=(SharePointer&)=delete;

    auto make_shared(Node node) {
        this->node=new Node(node.data);
    }
};
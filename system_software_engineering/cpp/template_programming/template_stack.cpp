//
// Created by 2025 on 4/1/2026.
//
#include "../header.h"
template <typename T, int size>
class Stack {
    public:
    vector<T> st;
    void push(T x) {
        st.push_back(x);
    }
    T pop() {
        return st.back();
    }
};

int main(int argc, char *argv[]) {
    Stack<int, 5> st;
    st.push(10);
    st.push(20);
    st.push(30);
    cout<<st.pop();
    constexpr exp =3+8;
}
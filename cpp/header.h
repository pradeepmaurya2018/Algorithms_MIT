#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <print>
#include <map>
#include <string>

using namespace std;

// ------------------ TYPE TRAITS ------------------

template<typename T, typename = void>
struct is_iterable : false_type {};

template<typename T>
struct is_iterable<T, void_t<
    decltype(begin(declval<T>())),
    decltype(end(declval<T>()))
>> : true_type {};

// string should NOT be treated as container
template<>
struct is_iterable<string> : false_type {};


// ------------------ PRINT SINGLE ------------------

// Primitive types
template<typename T>
enable_if_t<!is_iterable<T>::value>
print_one(const T& x) {
    cout << x;
}

// Containers
template<typename T>
enable_if_t<is_iterable<T>::value>
print_one(const T& container) {
    cout << "[ ";
    for (const auto& x : container) {
        print_one(x);
        cout << " ";
    }
    cout << "]\n";
}

// Pair
template<typename A, typename B>
void print_one(const pair<A,B>& p) {
    cout << "(";
    print_one(p.first);
    cout << ", ";
    print_one(p.second);
    cout << ")\n";
}


// ------------------ MAIN PRINT ------------------

template<typename... Args>
void print(const Args&... args) {
    string sep = "";
    ((cout << sep, print_one(args), sep = " "), ...);
    cout << "\n";
}

template<typename K, typename V>
void print_one(const map<K,V>& m) {
    cout << "{ ";
    for (const auto& [k, v] : m) {
        print_one(k);
        cout << ": ";
        print_one(v);
        cout << "| ";
    }
    cout << "}\n";
}

// ------------------ DEBUG ------------------

#define debug(...) debug_out(#__VA_ARGS__, __VA_ARGS__)

template<typename... Args>
void debug_out(const string& names, const Args&... args) {
    cout << "[DEBUG] " << names << " = ";
    print(args...);
}
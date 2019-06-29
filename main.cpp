//
// Created by Alexandra Ivanova on 29/06/2019.
//
#include <iostream>
#include "set.h"
//#include <set>

int main() {
    set<int> a;
    a.insert(8);
    a.insert(3);
    a.insert(5);
    a.insert(4);
    a.insert(3);
    a.insert(1);
    a.insert(8);
    a.insert(8);
    a.insert(10);
    a.insert(9);
    for (auto it = a.begin(); it != a.end(); it++) {
        std::cout << *(a.upper_bound(*it)) << " ";
    }

}
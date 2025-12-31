#include <iostream>
#include <concepts>

template <typename A, typename B>
void func(A a, B b) {
    std::cout << "Normal version" << std::endl;
}

template <typename A, typename B>
requires std::integral<B>
void func(A a, B b) {
    std::cout << "Partial version" << std::endl;
}

int main() {
    int a = 10;
    double b = 12;
    func(a, b);
    func(a, a);
}


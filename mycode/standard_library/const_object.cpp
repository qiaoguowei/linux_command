#include <iostream>

class A {

public:
    void update(const int& value) {
        _value = value;
    }

    int get() const {
        return _value;
    }

private:
    int _value = 0;
};

// const成员函数可以被对应的具有相同形参列表的非const成员函数重载。
class B {

public:
    const int& get() const {
        std::cout << "B::get() const" << std::endl;
        return _value;
    }

    int& get() {
        std::cout << "B::get()" << std::endl;
        return _value;
    }

private:
    int _value = 5;
};

int main() {

    const A a;
    //a.update(5);
    std::cout << a.get() << std::endl;

    B b;
    std::cout << b.get() << std::endl;
    int& tmp = b.get();

    const B bb;
    std::cout << bb.get() << std::endl;
}
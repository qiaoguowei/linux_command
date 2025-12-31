#include <iostream>

class Base {
private:
    virtual std::string classID() const {
        return std::string("Base");
    }

protected:
    virtual void dowork() = 0;

public:
    void work() {
        std::cout << "this class id is " << classID() << std::endl;
        dowork();
    }

    virtual ~Base() {

    }
};

class DerivedA : public Base {
private:
    std::string classID() const override {
        return std::string("DerivedA");
    }

protected:
    virtual void dowork() {
        std::cout << "this is DerivedA dowork!" << std::endl;
    }
};

int main() {
    Base *bp = new DerivedA();
    bp->work();
    delete bp;
}
#include "blackboard/black_board.h"
#include <iostream>
#include <string>

using namespace bbd;

void RegisteCallback(int a, int b) {
    std::cout << "[RegisteCallback] add called: " << a + b << std::endl;
    return;
}

class MyClass {
public:
    int Multiply(int x, int y) {
        std::cout << "[MyClass::Multiply] add called: " << x + y << std::endl;
        return x + y;
    }
};

int main() {
    // 注册一个函数（lambda）
    bbd::Register("add-lambda", [](int a, int b) -> void {
        std::cout << "[Lambda] add called: " << a + b << std::endl;
        return;
    });
    bbd::Register("add-func", RegisteCallback);
    MyClass obj;
    bbd::Register("add-class", &MyClass::Multiply, &obj);
    // 调用注册的函数
    bbd::Call("add-lambda", 1, 2);
    bbd::Call("add-class", 3, 4);
    bbd::Call("add-func", 5, 6);
    return 0;
}

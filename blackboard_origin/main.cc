#include "blackboard/black_board.h"
#include <iostream>
#include <string>

using namespace bbd;

int main() {
    // 注册一个函数（lambda）
    auto handler = bbd::Register("add", [](int a, int b) -> int {
        std::cout << "[Register] add called: " << a << ", " << b << std::endl;
        return a + b;
    });
    // 调用注册的函数
    auto result = bbd::Call<int>("add", 1, 2);
    if (result) {
        std::cout << "[Call] add result: " << *result << std::endl;
    } else {
        std::cout << "[Call] add failed" << std::endl;
    }

    // 订阅数据（Sub）
    bbd::Sub<Sync>("number", [](const int& value) {
        std::cout << "[Sub] value: " << value << std::endl;
    });
    // 发布新数据，触发订阅回调
    bbd::Pub<Save>("number", 42);

    // 设置数据（Set）
    bbd::Set("greeting", std::string("Hello, Blackboard!"));
    std::string greeting_value;
    bbd::Get("greeting", greeting_value);
    std::cout << "[Get] greeting: " << greeting_value << std::endl;

    return 0;
}

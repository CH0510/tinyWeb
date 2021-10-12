#include "src/Timestamp.h"
#include <iostream>

using namespace tinyWeb;

int main(int argc, char **argv) {
    Timestamp now = Timestamp::now();
    std::cout << now.microSecondsSinceEpoch() << std::endl;
    std::cout << now.toString() << std::endl;
    std::cout << now.toUTCFormatString() << std::endl;
    std::cout << now.toLocalFormatString() << std::endl;

    return 0;
}
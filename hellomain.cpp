#include <iostream>
#include "hello.h"

int main(int argc, char **argv) 
{
    std::cout << "> main()" << std::endl;
    hello("world");
    hello("mike");
    std::cout << "< main()" << std::endl;
    return 0;
}


#include <iostream>
#include "hello.h"

#include <gsl/gsl_cdf.h>

int main(int argc, char **argv) 
{
    std::cout << "> main()" << std::endl;
    hello("world");
    hello("mike");

    std::cout << "gsl test: " << std::endl;
    std::cout << gsl_cdf_lognormal_Q(0.1, 0, 1);
    std::cout << "end gsl test" << std::endl;
        
    std::cout << "< main()" << std::endl;
    return 0;
}


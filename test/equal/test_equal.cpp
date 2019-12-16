#include <vector>
#include <random>
#include <stdio.h>
using namespace std;

#define COLOR_NONE         "\033[0m"
#define COLOR_RED          "\033[0;32;31m"
#define COLOR_GREEN        "\033[0;32;32m"
#define PRINTF_INFO(...) 	\
{								\
	printf(COLOR_GREEN "[INFO]: "  __VA_ARGS__);	\
	CLEAR_COLOR;				\
}
#define PRINTF_ERROR(...) \
{								\
	printf(COLOR_RED "[ERROR]: " __VA_ARGS__);	\
	CLEAR_COLOR;				\
}

#define CLEAR_COLOR printf(COLOR_NONE)

double randVal(double basic_val)
{
    double val = basic_val + 1e-4*rand()/RAND_MAX;
    return val;
}

void test(int times)
{
    static int equal_nbr = 0;
    for(int i=0; i<times; i++)
    {
        double bv = 1.1;
        double a = randVal(bv);
        double b = randVal(bv);

        bool tf = a==b;
        if(tf)
        {
            PRINTF_ERROR("rand val %e %e, is equal %d\n", a, b, tf);
            equal_nbr++;
        }
        else
        {
            PRINTF_INFO("rand val %e %e, is equal %d\n", a, b, tf);
        }
        
    }
    PRINTF_INFO("equal nbr %d\n", equal_nbr);
}


int main(int argc, char** argv)
{
    int times = 100;
    if(argc>1)
        times = atoi(argv[1]);

    test(times);
    return 0;
}
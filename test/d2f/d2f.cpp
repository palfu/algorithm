#include <vector>
#include <opencv2/core.hpp>

using namespace cv;
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

double randVal(float basic_val)
{
    double val = basic_val + 1.0*rand()/RAND_MAX*100;
    return val;
}

void check_print(double dv, float fv, double dt=0.01)
{
    double dd = abs(dv-fv);
    if(dd>dt)
    {
        PRINTF_ERROR("double %0.4f, float %0.4f, delta %0.4f\n", dv, fv, dd);
    }else
    {
        PRINTF_INFO("double %0.4f, float %0.4f, delta %0.4f\n", dv, fv, dd);
    }
}

void check_print(Point3d p3d, Point3f p3f, double dt=0.01)
{
    double dx = abs(p3d.x-p3f.x);
    double dy = abs(p3d.y-p3f.y);
    double dz = abs(p3d.z-p3f.z);

    if(dx>dt || dy>dt || dz>dt)
    {
        PRINTF_ERROR("double %0.4f, %0.4f, %0.4f, float %0.4f, %0.4f, %0.4f, delta %0.4f, %0.4f, %0.4f\n", p3d.x, p3d.y, p3d.z, p3f.x, p3f.y, p3f.z, dx, dy, dz);
    }else
    {
        PRINTF_INFO("double %0.4f, %0.4f, %0.4f, float %0.4f, %0.4f, %0.4f, delta %0.4f, %0.4f, %0.4f\n", p3d.x, p3d.y, p3d.z, p3f.x, p3f.y, p3f.z, dx, dy, dz);
    }
}

void test_d2f(double basic_val=-1.0, int nbr=10)
{
    if(basic_val<0.0)
        basic_val = 1575617705.714;

    PRINTF_INFO("use basic val %0.4f\n", basic_val);
    float float_max = __FLT_MAX__;

    for(int i=0; i<10; i++)
    {
        double dv = randVal(basic_val);
        Point3d p3d = Point3d(randVal(basic_val), randVal(basic_val), randVal(basic_val));

        float fv = dv;
        Point3f p3f = Point3f(p3d.x, p3d.y, p3d.z);

        check_print(dv, fv);
        check_print(p3d, p3f);
    }
}


int main(int argc, char** argv)
{
    double basic_val=-1.0;
    if(argc>1)
        basic_val = atof(argv[1]);

    test_d2f(basic_val);
    return 0;
}
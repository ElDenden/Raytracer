#include "Utility.h"

//Vector functions

float len_2(const mat& vec)
{
    return vec.dot(vec);
}

float len(const mat& vec)
{
    return sqrt(len_2(vec));
}

// mat cross(const mat& u, const mat& v)
// {
//     mat cr = mat::Zero();

//     cr << u(1,0)*v(2,0) - u(2,0)*v(1,0), u(2,0)*v(0,0) - u(0,0)*v(2,0), u(0,0)*v(1,0) - u(1,0) * v(0,0);

//     return cr;
// }

mat normalize(const mat& m)
{
    return m / len(m);
}

float dot(const mat& u, const mat& v)
{
    return u.dot(v);
}

mat clamp(const mat& m)
{
    mat new_m = m;
    if(new_m(0,0) < 0.0)
    {
        new_m(0,0) = 0.0;
    }

    if(new_m(0,0) > 1.0)
    {
        new_m(0,0) = 1.0;
    }

    if(new_m(1,0) < 0.0)
    {
        new_m(1,0) = 0.0;
    }

    if(new_m(1,0) > 1.0)
    {
        new_m(1,0) = 1.0;
    }

    if(new_m(2,0) < 0.0)
    {
        new_m(2,0) = 0.0;
    }

    if(new_m(2,0) > 1.0)
    {
        new_m(2,0) = 1.0;
    }

    //std::cout << new_m << std::endl;

    return new_m;
}

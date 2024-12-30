#pragma once
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/Geometry>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>

typedef Eigen::Vector3f mat;

#define PI 3.14159265358979f
#define INF std::numeric_limits<float>::infinity()

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//Cmath
using std::fabs;

//Random
inline float random_float()
{
    static std::uniform_real_distribution<float> distribution(0.0,1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline float random_float(float min, float max)
{
    static std::uniform_real_distribution<float> distribution(min,max);
    static std::mt19937 generator;
    return distribution(generator);
}

inline bool terminate(float p)
{

    static std::bernoulli_distribution b(p);
    static std::mt19937 generator;
    float result = b(generator);

    return result;
}

//Vector functions

/// @brief Returns the length squared of the given vector
/// @param vec Dynamic floating point eigen matrix
/// @return Length squared of the given vector
float len_2(const mat& vec);

/// @brief Returns the length of the given vector
/// @param vec Dynamic floating point eigen matrix
/// @return Length of the given vector
float len(const mat& vec);

// mat cross(const mat& u, const mat& v);

mat normalize(const mat& m);

float dot(const mat& u, const mat& v);

mat clamp(const mat& m);

/// @brief Returns a (3,1) matrix initialized with random floating point numbers sampled from a uniform distribution [0,1]
/// @return Dynamic eigen matrix of size (3,1)
inline mat rand_mat()
{
    mat rand = mat::Zero();

    for(int i = 0; i < 3; i++)
    {
        rand(i) = random_float();
    }

    return rand;
}

/// @brief Returns a (3,1) matrix initialized with random floating point numbers sampled from a uniform distribution [min,max]
/// @param min The lower boundary of the uniform distribution's domain
/// @param max The upper boundary of the uniform distribution's domain
/// @return Dynamic eigen matrix of size (3,1)
inline mat rand_mat(float min, float max)
{
    mat rand = mat::Zero();

    for(int i = 0; i < 3; i++)
    {
        rand(i) = random_float(min,max);
    }

    return rand;
}

/// @brief Returns a (3,1) matrix initialized with random floating point numbers sampled from a uniform distribution [-1, 1]
/// @return Dynamic eigen matrix of size (3,1)
inline mat rand_in_unit_sphere()
{
    while(true)
    {
        mat rand = mat::Random();

        if(len_2(rand) < 1)
        {
            return rand;
        }
    }
}

/// @brief Returns a (3,1) matrix initialized with random floating point numbers sampled from a uniform distribution [-1, 1]. This matrix represents a 3D unit vector that is on a unit sphere.
/// @return Dynamic eigen matrix of size (3,1)
inline mat rand_unit_vector()
{
    return normalize(rand_in_unit_sphere());
}

/// @brief Returns a (3,1) matrix initialized with random floating point numbers sampled from a uniform distribution [-1, 1]. This matrix represents a 3D vector that is on a unit sphere.
/// @param n Normal of the intersected surface
/// @return Dynamic eigen matrix of size (3,1)
inline mat rand_on_hemisphere(const mat& n)
{
    mat rand = rand_unit_vector();

    if(rand.dot(n) > 0)
    {
        return rand;
    }
    
    return -1 * rand;
}

inline mat weighted_hemisphere_sampling()
{
    //Sample r and theta
    float r = std::sqrt(random_float(0,1));
    float theta = 2 * PI * random_float(0,1);

    //Transform into x and y coordinates and get z value
    float x = std::cos(theta) * r;
    float y = std::sin(theta) * r;
    float z = std::sqrt(1 - x * x - y * y );

    //Get direction vector
    mat dir = mat::Zero();

    dir << x , y, z;

    return dir;
}
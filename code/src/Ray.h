#pragma once
#include "Utility.h"

class Ray
{
    public:
        //Constructors

        /// @brief Default construction of the ray class
        Ray();
        
        /// @brief Custom construction of the ray class
        /// @param origin Eigen matrix representing the origin of the ray
        /// @param direction Eigen matrix representing the direction vector of the ray
        Ray(const mat& origin,const mat& direction);

        //Accessors

        /// @brief Returns the origin point of the ray
        /// @return Eigen matrix representing a point in 3D
        mat getOrigin()const;

        /// @brief Returns the direction vector of the ray
        /// @return Eigen matrix representing a vector in 3D
        mat getDirection() const;

        /// @brief Returns a point along the ray for some parameter t
        /// @param t floating point that scales the direction vector of the ray
        /// @return Eigen Matrix representing a point in 3D
        mat value_at(float t) const;
    ;
    private:
        mat origin;
        mat direction;
    ;
};
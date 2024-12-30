#pragma once
#include "Ray.h"
#include "Utility.h"
#include "Interval.h"

class Material;
class Geometry;

class HitRecord
{
    public:
        mat position;
        mat n;
        float t;
        bool n_is_outward;
        mat L;
        mat R;
        const Geometry* geo;
        mat V;
        mat transform;
        mat intensity;

        /// @brief Sets the direction of the normal relative to the surface and the casted ray
        /// @param  ray The casted ray
        /// @param normal The normal of the object's surface at the intersection with the ray
        void set_normal_direction(const Ray& ray, const mat& normal);
    ;
};

class Hittable
{
    public:
        //Destructors

        virtual ~Hittable() = default;

        //Pure virtual functions

        /// @brief Determines whether the given ray intersects with a Hittable object
        /// @param ray A Ray object
        /// @param t Interval for which an intersection is valid
        /// @param rec The record object keeping track of the information of the objects that the rays hit
        /// @return 
        virtual bool hit(const Ray& ray, Interval t, HitRecord& record) const = 0;


    ;
};
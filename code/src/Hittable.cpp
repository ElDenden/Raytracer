#include "Hittable.h"


void HitRecord::set_normal_direction(const Ray& ray, const mat& normal)
{
    if(dot(ray.getDirection(), normal) < 0)
    {
        this->n_is_outward = true;
        this->n = normal;
        return;
    }

    this->n_is_outward = false;
    this->n = -1* normal;
}
#include "Ray.h"

//Constructors

Ray::Ray()
{

}

Ray::Ray(const mat& origin,const mat& direction)
{
    this->origin = origin;
    this->direction = direction;
}

//Accessors

mat Ray::getOrigin() const
{
    return this->origin;
}
    
mat Ray::getDirection() const
{
    return this->direction;
}

mat Ray::value_at(float t) const
{
    return this->origin + this->direction*t;
}
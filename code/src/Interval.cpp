#include "Interval.h"

//Static variables
const Interval Interval::empty(+INF,-INF);
const Interval Interval::universe(-INF,+INF);

Interval::Interval()
{
    this->min = (+INF);
    this->max = (-INF);
}

Interval::Interval(float min, float max)
{
    this->min = min;
    this->max = max;
}

bool Interval::isBounding(float t) const
{
    return min <= t && t <= max;
}

bool Interval::isStrictlyBounding(float t) const
{
    return min < t && t < max;
}

float Interval::size() const
{
    return max - min;
}

Interval Interval::pad(float epsilon) const
{
    float padding = epsilon / 2;
    return Interval(min - padding, max + padding);
}

float Interval::clamp(float x) const
{
    if(x < min)
        return min;
    if(x > max)
        return max;
    return x;
}
#pragma once
#include "Utility.h"

class Interval
{
    public:
        float min;
        float max;

        //Constructors

        Interval();

        Interval(float min, float max);

        /// @brief Checks if a given value falls within the inclusive interval
        /// @param t The value we're checking
        /// @return Boolean that represents whether t satisfies the condition mentioned above
        bool isBounding(float t) const;

        /// @brief Returns the length of the interval
        /// @return Floating point value representing the length of the interval
        float size() const;
        
        /// @brief Returns a new interval padding on each side by epsilon/2
        /// @param epsilon Floating point representing the total padding of the interval
        /// @return A new padded interval
        Interval pad(float epsilon) const;

        /// @brief Checks if a given value falls within the strict interval
        /// @param t The value we're checking
        /// @return Boolean that represents whether t satisfies the condition mentioned above
        bool isStrictlyBounding(float t) const;

        /// @brief Clamps the value of x such that it is within the interval boundaries
        /// @param x Floating point value to be clamped
        /// @return A clamped value of x
        float clamp(float x) const;

        static const Interval empty;

        static const Interval universe;
    ;
};


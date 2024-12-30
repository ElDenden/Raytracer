#pragma once
#include "../external/json.hpp"
#include "Hittable.h"
#include "Utility.h"

//This class serves as a base class for more specialized geometric models.
class Geometry: public Hittable
{
    public:
        //Constructors

        //Default construction of a geometry object
        Geometry();

        //Destructors
        virtual ~Geometry();
        //Accessors

        //Returns the object's type
        std::string getType() const;
        //Returns the object's reflection coefficient for ambient light
        float getKa() const;
        //Returns the object's reflection coefficient for diffuse light
        float getKd() const;
        //Returns the object's reflection coefficient for specular light
        float getKs() const;
        //Returns the object's phong coefficient
        float getPc() const;
        //Returns the object's reflection color from an ambient light source
        mat getAc() const;
        //Returns the object's reflection color from a diffuse light source
        mat getDc() const;
        //Returns the object's reflection color from a specular light source
        mat getSc() const;
        //Returns the object's transformation matrix as a copy
        mat getTransform() const;
        //Returns the description of the geometry object
        std::string getComment() const;

        //Query Methods

        //Returns the object's visibility status
        bool isVisible();

        //Mutators

        //Sets the object's type
        void setType(std::string* type);
        //Sets the object's reflection coefficient for ambient light (0<=ka<=1)
        void setKa(float* ka);
        //Sets the object's reflection coefficient for diffuse light (0<=kd<=1)
        void setKd(float* kd);
        //Sets the object's reflection coefficient for specular light (0<=ks<=1)
        void setKs(float* ks);
        //Sets the object's phong coefficient (1 < pc)
        void setPc(float* pc);
        //Sets the object's reflection color from an ambient light source (0<=ac<=1)
        void setAc(mat& ac);
        //Sets the object's reflection color from a diffuse light source (0<=dc<=1)
        void setDc(mat& dc);
        //Sets the object's reflection color from a specular light source (0<=sc<=1)
        void setSc(mat& sc);
        //Sets the object's transformation matrix as a copy
        void setTransform(mat& transform);
        //Sets the object's visibility status
        void setVisibility(bool* isVisible);
        //Sets the object's comment description
        void setComment(std::string* comment);

        //Pure Virtual functions
        virtual bool hit(const Ray& ray, Interval t, HitRecord& record) const = 0;
    ;
    private:
        //Attributes

        //Type of the geometry object
        std::string* type;
        //Reflection coefficient for ambient light
        float* ka;
        //Reflection coefficient for diffuse light
        float* kd;
        //Reflection coefficient for specular light
        float* ks;
        //Phong coefficient
        float* pc;
        //Reflection color of an ambient light source in rgb values
        mat ac;
        //Reflection color of a diffuse light source in rgb values
        mat dc;
        //Reflection color of a specular light source in rgb values
        mat sc;
        //4x4 affine transformation matrix specified in row major represention
        mat transform;
        //Defines whether the object is visible or not
        bool* visible;
        //Description of the geometry object
        std::string* comment;
    ;
};

//This class describes a sphere model with a specific radius and center.
class Sphere: public Geometry
{
    public:
        //Constructors
        
        //Default construction of a sphere object
        Sphere();

        //Custom constructions of a sphere object
        Sphere(mat& centre, float* radius);

        //Accessors
        
        //Returns the sphere's radius
        float getRadius();
        //Returns the sphere's center
        mat getCentre();
        
        //Mutators

        //Sets the sphere's radius
        void setRadius(float* radius);
        //Sets the sphere's center
        void setCentre(mat& center);

        //Pure Virtual functions
        bool hit(const Ray& ray, Interval t, HitRecord& record) const override;
    ;
    private:
        //Attributes

        //Radius of the sphere
        float* radius;
        //Column vector of 3 floating point values representing the center of the sphere
        mat centre;

    ;
};

//This class describes a rectangle model with four specific points p1, p2, p3, p4 
//which represent its corners in counterclockwise order with respect to the normal.
class Rectangle: public Geometry
{
    public:
        //Constructors

        //Default constructions of a rectangle object
        Rectangle();

        //Custom constructions of a rectangle object
        Rectangle(const mat& p1,const mat& p2,const mat& p3,const mat& p4);

        //Virtual functions
        bool hit(const Ray& ray, Interval t, HitRecord& record) const override;

        bool isInside(float a, float b, HitRecord& record) const; //I made this non virtual
    ;
    private:
        //Attributes

        /// @brief Initial point of the rectangle
        mat q;
        /// @brief One of two vector that lies along the edge of the rectangle starting from point p
        mat v;
        /// @brief The other vector that lies along the edge of the rectangle starting from point p
        mat u;
        /// @brief D constant in the plane equation
        float D;
        /// @brief Normalized normal of the plane that contains the rectangle
        mat n;
        /// @brief Caching value that is going to be used to determine whether a point is inside the rectangle or outside
        mat w;
    ;
};

//BoxPadder


//This class serves as a base class model for light objects
class Light
{
    public:
        //Constructors
        Light();

        //Destructor

        virtual~Light(){};

        //Accessors
        
        //Returns the light object's type
        std::string getType() const;
        //Returns the light object's diffuse light intensity
        mat getId() const;
        //Returns the light object's specular light intensity
        mat getIs() const;
        //Returns the light object's transformation matrix as a copy
        mat getTransform() const;

        //Mutators

        //Sets the light object's type
        void setType(std::string* type);
        //Sets the light object's diffuse light intensity
        void setId(const mat& id);
        //Sets the light object's specular light intensity
        void setIs(const mat& is);
        //Sets the light object's transform matrix
        void setTransform(mat& transform);

        //Virtual functions for convenience
        virtual bool isUsed(){return true;};

        virtual void useCentre(bool* hello){};

        virtual unsigned int getGridDim()
        {
            return 0;
        }

        virtual bool getUseCentre()
        {
            return false;
        }

        virtual std::vector<mat> sample() const;

        virtual mat getCentre(){return mat::Zero();};
    ;
    private:
        //Attributes

        //Type of the light
        std::string* type;
        //Intensity of diffuse light in rgb values
        mat id;
        //Intensity of specular light in rgb values
        mat is;
        //4x4 affine transformation matrix specified in row major represention
        mat transform;
    ;
};

//This class models a point light source
class Point: public Light
{
    public:
        //Constructors

        //Default construction of the point object
        Point();

        //Custom constructions of the point object
        Point(mat& centre, bool* isUsed);

        //Accessors

        //Returns the point light source's center
        mat getCentre();

        //Query methods

        //Returns whether this light is being used or not
        bool isUsed();

        //Mutators

        //Sets the point light source's center
        void setCentre(mat& centre);
        //Set whether this light should be used or not
        void setUse(bool* isUsed);
    ;
    private:
        //Attributes

        //Column vector of 3 floating point values representing the center of the point light source
        mat centre;
        //Specifies if this light is to be used
        bool* use;

    ;
};

//This class models a rectangular area light source
class Area: public Light
{
    public:
        //Constructors

        //Default constructions of a rectangle object
        Area();

        //Custom constructions of a rectangle object
        Area(const mat& p1,const mat& p2,const mat& p3,const mat& p4, bool* usecentre, unsigned int* grid_dim);

        /// @brief Samples points from the area light
        /// @return Vector of point locations
        std::vector<mat> sample() const;

        //Virtual functions
        mat rand_point_inside(const mat& q,const mat& u,const mat& v) const;

        //Accessors
        unsigned int getGridDim()
        {
            return *this->grid_dim;
        }

        //Sets the light object's stratified sampling grid size
        inline void setGridDim(unsigned int* grid_dim)
        {
            this->grid_dim = grid_dim;
        }
        //Sets the light object's usecenter status
        void useCentre(bool* isUsingCentre)
        {
            if(this->usecentre != nullptr)
            {
                delete this->usecentre;
            }
            this->usecentre = isUsingCentre;
        }
        bool getUseCentre()
        {
            return *this->usecentre;
        }
    ;
    private:
        //Attributes

        /// @brief Initial point of the rectangle
        mat q;
        /// @brief One of two vector that lies along the edge of the rectangle starting from point p
        mat v;
        /// @brief The other vector that lies along the edge of the rectangle starting from point p
        mat u;

        //Specifies whether the light source of an area light should act as a point light (for debugging purposes)
        bool* usecentre;
        //Specifies the grid size of a stratified sampling (n x n)
        unsigned int* grid_dim;
    ;
};

class Output
{
    public:
        //Constructors

        //Default constructions of an Output object
        Output();

        //Accessors
        
        //Returns the filename of the Output object
        std::string getFilename() const;
        //Returns the size of the Output object as a 2x1 column vector
        Eigen::Vector2i getSize() const;
        //Returns the fov of the Output object
        float getFov() const;
        //Returns the center of the Output object
        mat getCentre() const;
        //Returns the up vector of the Output object
        mat getUp() const;
        //Returns the lookat vector of the Output object
        mat getLookat() const;
        //Returns the ambient intensity of the Output object
        mat getAi() const;
        //Returns the background color of the Output object
        mat getBkc() const;
        //Returns the raysperpixel attribute of the Output object
        std::vector<unsigned int> getRaysperpixel() const;
        //Returns the antialiasing option of the Output object
        bool getAntialiasing() const;
        //Returns the twosiderender option of the Output object
        bool getTwosiderender() const;
        //Returns the globalillum option of the Output object
        bool getGlobalillum() const;
        //Returns the maxbounces of the Output object
        float getMaxbounces() const;
        //Returns the probterminate of the Output object
        float getProbterminate() const;

        //Mutators
        
        //Sets the filename of the Output object
        void setFilename(std::string* filename);
        //Sets the size of the Output object
        void setSize(const Eigen::Vector2i& size);
        //Sets the fov of the Output object
        void setFov(float* fov);
        //Sets the center of the Output object
        void setCentre(mat& centre);
        //Sets the up vector of the Output object
        void setUp(mat& up);
        //Sets the lookat vector of the Output object
        void setLookat(mat& lookat);
        //Sets the ambient intensity of the Output object
        void setAi(mat& ai);
        //Sets the background color of the Output object
        void setBkc(mat& bkc);
        //Sets the raysperpixel attribute of the Output object
        void setRaysperpixel(std::vector<unsigned int>* raysperpixel);
        //Sets the antialiasing option of the Output object
        void setAntialiasing(bool* antialiasing);
        //Sets the twosiderender option of the Output object
        void setTwosiderender(bool* twosiderender);
        //Sets the globalillum option of the Output object
        void setGlobalillum(bool* globalillum);
        //Sets the maxbounces of the Output object
        void setMaxbounces(float* maxbounces);
        //Sets the probterminate of the Output object
        void setProbterminate(float* probterminate);


    ;
    private:
        //Attributes

        //Name of the output ppm file
        std::string* filename;
        //Specifies the resolution of the output image
        Eigen::Vector2i size;
        //Field of view as an angle in degrees (on the vertical axis)
        float* fov;
        //Column vector of 3 floating point values representing the coordinates of the camera's center
        mat centre;
        //Column vector of 3 floating point values representing the up vector
        mat up;
        //Column vector of 3 floating point values representing the lookat vector
        mat lookat;
        //Ambient intensity of the scene in rgb
        mat ai;
        //Background color of the ouput in rgb
        mat bkc;
        //Specifies the technique to use to determine the number of rays used per pixel
        std::vector<unsigned int>* raysperpixel;
        //Used to toggle raysperpixel. If false, user implementation decides how many rays are used per pixel.
        bool* antialiasing;
        //When true, assumes that light bounces on geometric models the same way if light touches the model from the front or the back. 
        //Default value is true.
        bool* twosiderender;
        //When true, assumes rendering is done with global illumination. Otherwise, rendering is done with Phong shading.
        bool* globalillum;
        //Specifies the maximum number of times a ray can bounce
        float* maxbounces;
        //Specifies the probability that a ray terminates
        float* probterminate;
    ;
};

//Methods for parsing JSON contents

//Parses the geometry objects of a JSON file
std::vector<shared_ptr<Geometry>>* parseGeometry(nlohmann::json& j);
//Parses the light objects of a JSON file
std::vector<Light*>* parseLight(nlohmann::json& j);
//Parses the ouput objects of a JSON file
std::vector<Output*>* parseOutput(nlohmann::json& j);
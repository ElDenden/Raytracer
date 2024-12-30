#include "json_objparser.h"

typedef Eigen::Vector3f mat;

//Class Definitions

//Geometry

//Constructors

Geometry::Geometry(){}

//Destructors

Geometry::~Geometry(){}

//Accessors

std::string Geometry::getType() const
{
    return *this->type;
}

float Geometry::getKa() const
{
    return *this->ka;
}

float Geometry::getKd() const
{
    return *this->kd;
}

float Geometry::getKs() const
{
    return *this->ks;
}

float Geometry::getPc() const
{
    return *this->pc;
}

mat Geometry::getAc() const
{
    return this->ac;
}

mat Geometry::getDc() const
{
    return this->dc;
}

mat Geometry::getSc() const
{
    return this->sc;
}

mat Geometry::getTransform() const
{
    return this->transform;
}

std::string Geometry::getComment() const
{
    return *this->comment;
}

bool Geometry::isVisible()
{
    return *this->visible;
}

//Mutators

void Geometry::setType(std::string* type)
{
    this->type = type;
}

void Geometry::setKa(float* ka)
{
    this->ka = ka;
}

void Geometry::setKd(float* kd)
{
    this->kd = kd;
}

void Geometry::setKs(float* ks)
{
    this->ks = ks;
}

void Geometry::setPc(float* pc)
{
    this->pc = pc;
}

void Geometry::setAc(mat& ac)
{
    this->ac = ac;
}

void Geometry::setDc(mat& dc)
{
    this->dc = dc;
}

void Geometry::setSc(mat& sc)
{
    this->sc = sc;
}

void Geometry::setTransform(mat& transform)
{
    this->transform = transform;
}

void Geometry::setComment(std::string* comment)
{
    this->comment = comment;
}

void Geometry::setVisibility(bool* isVisible)
{
    this->visible = isVisible;
}

//Sphere class

//Constructors

Sphere::Sphere(){}

Sphere::Sphere(mat& centre, float* radius)
{
    this->centre = centre;
    this->radius = radius;
}

//Accesors

float Sphere::getRadius()
{
    return *this->radius;
}

mat Sphere::getCentre()
{
    return this->centre;
}

void Sphere::setRadius(float* radius)
{
    this->radius = radius;
}

void Sphere::setCentre(mat& centre)
{
    this->centre = centre;
}

//Pure Virtual functions

bool Sphere::hit(const Ray& ray, Interval t, HitRecord& record) const
{
    mat oc = ray.getOrigin() - this->centre;
    float a = ray.getDirection().dot(ray.getDirection());
    float half_b = ray.getDirection().dot(oc);
    float c = oc.dot(oc) - (*this->radius)* (*this->radius);

    float discriminant = half_b*half_b - a*c;
    
    if (discriminant < 0.001) return false;

    float sqrtd = std::sqrt(discriminant);

    auto root = (-half_b - sqrtd) / a;
    if (!t.isBounding(root)) {
        root = (-half_b + sqrtd) / a;
        if (!t.isBounding(root))
            return false;
    }

    record.t = root;

    record.position = ray.value_at(record.t);

    record.n = (record.position - this->centre) / (*this->radius);

    record.set_normal_direction(ray, record.n);
    
    record.geo = this;

    return true;
}

//Rectangle class

//Constructors

Rectangle::Rectangle(){}

Rectangle::Rectangle(const mat& p1,const mat& p2,const mat& p3,const mat& p4)
{
    this->q = p1;

    this->u = p2 - p1;

    this->v = p4 - p1;

    mat normal = this->u.cross(this->v);

    this->n = normalize(normal);

    this->D = this->n.dot(this->q);

    //Calculate the caching value used to calculate the planar coordinates of the rectangle
    this->w = normal / normal.dot(normal);
}

//Pure virtual methods

bool Rectangle::hit(const Ray& ray, Interval t_interval, HitRecord& record) const
{
    float denominator = this->n.dot(ray.getDirection());

    if(fabs(denominator) < 1e-8) return false; // If denom is 0 then ray is parallel to plane

    float t = (this->D - this->n.dot(ray.getOrigin())) / denominator;

    if(!t_interval.isBounding(t)) return false;

    mat intersection = ray.value_at(t);

    //Check if point on plane lies within rectangle

    mat p = intersection - this->q; // Vector on plane

    float alpha = this->w.dot(p.cross(this->v));

    float beta = this->w.dot(this->u.cross(p));

    //std::cout << "alpha: " << alpha << "\tbeta: " << beta << std::endl;

    if(!isInside(alpha,beta,record)) return false;

    record.t = t;
    record.position = intersection;
    record.set_normal_direction(ray, this->n);
    record.geo = this;
    //std::cout << "record.t: " << record.t << "\nrecord.position: " << record.position << "\n record.n: " << record.n << std::endl;

    return true;
}

bool Rectangle::isInside(float alpha, float beta, HitRecord& record) const
{
    if(alpha < 0 || 1 < alpha || beta < 0 || 1 < beta) return false;

    return true;
}

//Light class

//Constructors

Light::Light(){}

//Accessors

std::string Light::getType() const
{
    return *this->type;
}

mat Light::getId() const
{
    return this->id;
}

mat Light::getIs() const
{
    return this->is;
}

mat Light::getTransform() const
{
    return this->transform;
}

//Mutators

void Light::setType(std::string* type)
{
    this->type = type;
}

void Light::setId(const mat& id)
{
    this->id = id;
}

void Light::setIs(const mat& is)
{
    this->is = is;
}

void Light::setTransform(mat& transform)
{
    this->transform = transform;
}

//Virtual functions for convenience
std::vector<mat> Light::sample() const
{
    std::vector<mat> s(0);
    return s;
};

//Point class

//Constructors

Point::Point(){}

Point::Point(mat& centre, bool* isUsed)
{
    this->centre = centre;
    this->use = isUsed;
}

//Accessors

mat Point::getCentre()
{
    return this->centre;
}

bool Point::isUsed()
{
    return *this->use;
}

//Mutators

void Point::setCentre(mat& centre)
{
    this->centre = centre;
}

void Point::setUse(bool* isUsed)
{
    this->use = isUsed;
}

//Area class

//Constructors

Area::Area(){this->usecentre = nullptr;}

Area::Area(const mat& p1,const mat& p2,const mat& p3,const mat& p4, bool* usecentre, unsigned int* grid_dim)
{
    this->q = p1;

    this->u = p2 - p1;

    this->v = p4 - p1;

    this->usecentre = usecentre;
    this->grid_dim = grid_dim;
}

//Methods

std::vector<mat> Area::sample() const
{
    std::vector<mat> points;

    if(*this->usecentre)
    {
        points.push_back(this->q + 0.5 * this->u + 0.5 * this->v);
        return points;
    }

    mat step_u = this->u / *this->grid_dim;
    mat step_v = this->v / *this->grid_dim;

    for(int i = 0; i < *this->grid_dim; i++)
    {
        for(int j =0; j < *this->grid_dim; j++)
        {   
            mat q_var = this->q + i*step_u + j*step_v;
            points.push_back(rand_point_inside(q_var, step_u, step_v));
        }
    }

    return points;
    
}

mat Area::rand_point_inside(const mat& q,const mat& u,const mat& v) const
{
    return q + random_float()* u + random_float()* v;
}



//Output class

//Constructors

Output::Output()
{
    this->antialiasing = nullptr;
}

//Accessors

std::string Output::getFilename() const
{
    return *this->filename;
}

Eigen::Vector2i Output::getSize() const
{
    return this->size;
}

float Output::getFov() const
{
    return *this->fov;
}

mat Output::getCentre() const
{
    return this->centre;
}

mat Output::getUp() const
{
    return this->up;
}

mat Output::getLookat() const
{
    return this->lookat;
}

mat Output::getAi() const
{
    return this->ai;
}

mat Output::getBkc() const
{
    return this->bkc;
}

std::vector<unsigned int> Output::getRaysperpixel() const
{
    return *this->raysperpixel;
}

bool Output::getAntialiasing() const
{
    return *this->antialiasing;
}

bool Output::getTwosiderender() const
{
    return *this->twosiderender;
}

bool Output::getGlobalillum() const
{
    return *this->globalillum;
}

float Output::getMaxbounces() const
{
    return *this->maxbounces;
}

float Output::getProbterminate() const
{
    return *this->probterminate;
}

//Mutators

void Output::setFilename(std::string* filename)
{
    this->filename = filename;
}

void Output::setSize(const Eigen::Vector2i& size)
{
    this->size = size;
}

void Output::setFov(float* fov)
{
    this->fov = fov;
}

void Output::setCentre(mat& centre)
{
    this->centre = centre;
}

void Output::setUp(mat& up)
{
    this->up = up;
}

void Output::setLookat(mat& lookat)
{
    this->lookat = lookat;
}

void Output::setAi(mat& ai)
{
    this->ai = ai;
}

void Output::setBkc(mat& bkc)
{
    this->bkc = bkc;
}

void Output::setRaysperpixel(std::vector<unsigned int>* raysperpixel)
{
    this->raysperpixel = raysperpixel;
}

void Output::setAntialiasing(bool* antialiasing)
{
    if(this->antialiasing != nullptr)
    {
        delete this->antialiasing;
    }
    this->antialiasing = antialiasing;
}

void Output::setTwosiderender(bool* twosiderender)
{
    this->twosiderender = twosiderender;
}

void Output::setGlobalillum(bool* globalillum)
{
    this->globalillum = globalillum;
}

void Output::setMaxbounces(float* maxbounces)
{
    this->maxbounces = maxbounces;
}

void Output::setProbterminate(float* probterminate)
{
    this->probterminate = probterminate;
}

//Methods for parsing JSON contents

//Parses the geometry objects of a JSON file
std::vector<shared_ptr<Geometry>>* parseGeometry(nlohmann::json& j)
{
    //Create a pointer to a vector that will hold all parsed geometric objects
    std::vector<shared_ptr<Geometry>>* geometries = new std::vector<shared_ptr<Geometry>>();
    for (auto itr = j["geometry"].begin(); itr!= j["geometry"].end(); itr++){
        
        //Instantiate a Geometry object
        shared_ptr<Geometry> geometry;
        std::string* type;
        //Check and store all mandatory attributes
        if(itr->contains("type"))
        {
            type = new std::string((*itr)["type"].get<std::string>());
        }
        else
        {
            std::cerr<<"Error: Missing type specifier for geometry object"<<std::endl;
            exit(1);
        }

        //Check for the type of the geo object
        if(*type=="sphere")
        {
            mat centre = mat::Zero();
            float* radius;

            if(itr->contains("centre"))
            {
                //Extract centre
                int i = 0;
                for (auto itr2 =(*itr)["centre"].begin(); itr2!= (*itr)["centre"].end(); itr2++)
                {
                    if(i<3)
                    {
                        centre(i++) = (*itr2).get<float>();
                    }
                    else 
                    {
                        std::cout<<"Warning: Too many entries in centre"<<std::endl;
                    }
                }
            }
            else
            {
                std::cerr<<"Error: Missing centre specifier for sphere object"<<std::endl;
                exit(1);
            }

            if(itr->contains("radius"))
            {
                radius = new float((*itr)["radius"].get<float>());
            }
            else
            {
                std::cerr<<"Error: Missing radius specifier for sphere object"<<std::endl;
                exit(1);
            }

            //Assign object to geometry pointer

            geometry = make_shared<Sphere>(centre, radius);

            //std::cerr << "Hello from Sphere"<< std::endl;

        }
        else if(*type == "rectangle")
        {

            //std::cout << geometry->getType();
            mat p1 = mat::Zero();
            mat p2 = mat::Zero();
            mat p3 = mat::Zero();
            mat p4 = mat::Zero();

            //std::cerr << "Hello before p1"<< std::endl;
            if(itr->contains("p1"))
            {
                //Extract p1
                int i = 0;
                for (auto itr2 =(*itr)["p1"].begin(); itr2!= (*itr)["p1"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p1(i++) = (*itr2).get<float>();
                    } 
                    else 
                    {
                        std::cout<<"Warning: Too many entries in p1"<<std::endl;
                    }
                }
            }
            else
            {
                std::cerr<<"Error: Missing p1 specifier for rectangle object"<<std::endl;
                exit(1);
            }

            //std::cerr << "Hello before p2"<< std::endl;
            if(itr->contains("p2"))
            {
                //Extract p2
                int i = 0;
                for (auto itr2 =(*itr)["p2"].begin(); itr2!= (*itr)["p2"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p2(i++) = (*itr2).get<float>();
                    } 
                    else 
                    {
                        std::cout<<"Warning: Too many entries in p2"<<std::endl;
                    }
                }
            }
            else
            {
                std::cerr<<"Error: Missing p2 specifier for rectangle object"<<std::endl;
                exit(1);
            }
            //std::cerr << "Hello before p3"<< std::endl;
            if(itr->contains("p3"))
            {
                //Extract p3
                int i = 0;
                for (auto itr2 =(*itr)["p3"].begin(); itr2!= (*itr)["p3"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p3(i++) = (*itr2).get<float>();
                    } 
                    else 
                    {
                        std::cout<<"Warning: Too many entries in p3"<<std::endl;
                    }
                }
            }
            else
            {
                std::cerr<<"Error: Missing p3 specifier for rectangle object"<<std::endl;
                exit(1);
            }
            //std::cerr << "Hello before p4"<< std::endl;
            if(itr->contains("p4"))
            {
                //Extract p4
                int i = 0;
                for (auto itr2 =(*itr)["p4"].begin(); itr2!= (*itr)["p4"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p4(i++) = (*itr2).get<float>();
                    } 
                    else 
                    {
                        std::cout<<"Warning: Too many entries in p4"<<std::endl;
                    }
                }

            }
            else
            {
                std::cerr<<"Error: Missing p4 specifier for rectangle object"<<std::endl;
                exit(1);
            }

            //Assign object to geometry pointer
            geometry = make_shared<Rectangle>(p1,p2,p3,p4);
        }
        else
        {
            std::cout<<"Error: Invalid type specifier for geometry object"<<std::endl;
            delete type;
            exit(1);
        }

        //Set the type of the geo object
        geometry->setType(type);

        //Continue checking for mandatory geo attributes
        //Create matrices required for geo object
        mat ac = mat::Zero();
        mat dc = mat::Zero();
        mat sc = mat::Zero();
        
        //std::cerr << "Hello before ka"<< std::endl;
        if(itr->contains("ka"))
        {
            geometry->setKa(new float((*itr)["ka"].get<float>()));
        }
        else
        {
            std::cerr<<"Error: Missing ka specifier for geometry object"<<std::endl;
            exit(1);
        }

        //std::cerr << "Hello before kd"<< std::endl;
        if(itr->contains("kd"))
        {
            geometry->setKd(new float((*itr)["kd"].get<float>()));
        }
        else
        {
            std::cerr<<"Error: Missing kd specifier for geometry object"<<std::endl;
            exit(1);
        }

        //std::cerr << "Hello before ks"<< std::endl;
        if(itr->contains("ks"))
        {
            geometry->setKs(new float((*itr)["ks"].get<float>()));
        }
        else
        {
            std::cerr<<"Error: Missing ks specifier for geometry object"<<std::endl;
            exit(1);
        }

        //std::cerr << "Hello before pc"<< std::endl;
        if(itr->contains("pc"))
        {
            geometry->setPc(new float((*itr)["pc"].get<float>()));
        }
        else
        {
            std::cerr<<"Error: Missing pc specifier for geometry object"<<std::endl;
            exit(1);
        }

        //Ok
        //std::cerr << "Hello before ac"<< std::endl;
        if(itr->contains("ac"))
        {
            int i{0};
            for (auto itr2 =(*itr)["ac"].begin(); itr2!= (*itr)["ac"].end(); itr2++)
            {
                if(i<3)
                {
                    ac(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in ac"<<std::endl;
                }
            }
            //Set geometry attribute
            geometry->setAc(ac);
        }
        else
        {
            std::cerr<<"Error: Missing ac specifier for geometry object"<<std::endl;
            exit(1);
        }
        //std::cerr << "Hello before dc"<< std::endl;
        if(itr->contains("dc"))
        {
            int i{0};
            for (auto itr2 =(*itr)["dc"].begin(); itr2!= (*itr)["dc"].end(); itr2++)
            {
                if(i<3)
                {
                    dc(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in dc"<<std::endl;
                }
            }

            //Set geometry attribute
            geometry->setDc(dc);
        }
        else
        {
            std::cerr<<"Error: Missing dc specifier for geometry object"<<std::endl;
            exit(1);
        }
        //std::cerr << "Hello before sc"<< std::endl;
        if(itr->contains("sc"))
        {
            int i{0};
            for (auto itr2 =(*itr)["sc"].begin(); itr2!= (*itr)["sc"].end(); itr2++)
            {
                if(i<3)
                {
                    sc(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in sc"<<std::endl;
                }
            }

            //Set geometry attribute
            geometry->setSc(sc);
        }
        else
        {
            std::cerr<<"Error: Missing sc specifier for geometry object"<<std::endl;
            exit(1);
        }

        if(itr->contains("visible"))
        {
            geometry->setVisibility(new bool((*itr)["visible"].get<bool>()));
        }

        if(itr->contains("comment"))
        {
            geometry->setComment(new std::string((*itr)["comment"].get<std::string>()));
        }

        //Populate geometries vector
        geometries->push_back(geometry);
    }

    // std::cout << "done1" << std::endl;

    //Return reference to geometries vector
    return geometries;
}
//Parses the light objects of a JSON file
std::vector<Light*>* parseLight(nlohmann::json& j)
{
    //Create a lights vector that is going to hold all light objects
    std::vector<Light*>* lights = new std::vector<Light*>();
    for (auto itr = j["light"].begin(); itr!= j["light"].end(); itr++){
        
        //Instantiate a pointer to a Light object
        Light* light;

        std::string* type;
        //Check and store all mandatory attributes
        if(itr->contains("type"))
        {
            type = new std::string((*itr)["type"].get<std::string>());
            
        }
        else 
        {
            std::cerr<<"Error: Missing type specifier for light object"<<std::endl;
            exit(1);
        }

        //Check for the type and assign the corresponding attributes

        if(*type == "point")
        {
            //Instantiate matrix and initialize boolean required for attributes
            mat centre = mat::Zero();
            bool* isUsed;
            if(itr->contains("centre"))
            {
                int i{0};
                for (auto itr2 =(*itr)["centre"].begin(); itr2!= (*itr)["centre"].end(); itr2++)
                {
                    if(i<3)
                    {
                        centre(i++) = (*itr2).get<float>();
                    }
                    else 
                    {
                        std::cout<<"Warning: Too many entries in centre"<<std::endl;
                    }
                }
            }
            else
            {
                std::cerr<<"Error: Missing centre specifier for point object"<<std::endl;
                exit(1);
            }

            if(itr->contains("use"))
            {
                isUsed = new bool((*itr)["use"].get<bool>());
            }
            else
            {
                isUsed = new bool(true);
            }

            //Assign point object to light pointer
            
            //OK

            light = new Point(centre, isUsed);
        }
        else if(*type == "area")
        {
            //Instantiate matrices, boolean, and integer required for area object
            mat p1 = mat::Zero();
            mat p2 = mat::Zero();
            mat p3 = mat::Zero();
            mat p4 = mat::Zero();
            bool* usecenter;
            unsigned int* n;

            //Ok

            if(itr->contains("p1"))
            {
                //Extract p1
                int i = 0;
                for (auto itr2 =(*itr)["p1"].begin(); itr2!= (*itr)["p1"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p1(i++) = (*itr2).get<float>();
                    } 
                    else
                    {
                        std::cout<<"Warning: Too many entries in p1"<<std::endl;
                    }
                }

            }
            else
            {
                std::cerr<<"Error: Missing p1 specifier for area light object"<<std::endl;
                exit(1);
            }

            if(itr->contains("p2"))
            {
                //Extract p2
                int i = 0;
                for (auto itr2 =(*itr)["p2"].begin(); itr2!= (*itr)["p2"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p2(i++) = (*itr2).get<float>();
                    } 
                    else 
                    {
                        std::cout<<"Warning: Too many entries in p2"<<std::endl;
                    }
                }

            }
            else
            {
                std::cerr<<"Error: Missing p2 specifier for area light object"<<std::endl;
                exit(1);
            }

            if(itr->contains("p3"))
            {
                //Extract p3
                int i = 0;
                for (auto itr2 =(*itr)["p3"].begin(); itr2!= (*itr)["p3"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p3(i++) = (*itr2).get<float>();
                    } 
                    else 
                    {
                        std::cout<<"Warning: Too many entries in p3"<<std::endl;
                    }
                }


            }
            else
            {
                std::cerr<<"Error: Missing p3 specifier for area light object"<<std::endl;
                exit(1);
            }

            if(itr->contains("p4"))
            {
                //Extract p4
                int i = 0;
                for (auto itr2 =(*itr)["p4"].begin(); itr2!= (*itr)["p4"].end(); itr2++)
                {
                    if(i<3)
                    {
                        p4(i++) = (*itr2).get<float>();
                    } 
                    else 
                    {
                        std::cout<<"Warning: Too many entries in p4"<<std::endl;
                    }
                }

            }
            else
            {
                std::cerr<<"Error: Missing p4 specifier for area light object"<<std::endl;
                exit(1);
            }

            if(itr->contains("usecenter"))
            {
                usecenter = new bool((*itr)["usecenter"].get<bool>());
            }
            else
            {
                usecenter = new bool(false);
            }

            if(itr->contains("n"))
            {
                n = new unsigned int((*itr)["n"].get<unsigned int>());
            }
            else
            {
                n = new unsigned int(1);
            }

            //Assign area object to light pointer

            light = new Area(p1,p2,p3,p4,usecenter,n);
            //OK
        }
        else
        {
            std::cout<<"Error: Invalid type specifier for light object"<<std::endl;
            delete type;
            exit(1);
        }

        //Set type of object
        light->setType(type);

        //Continue checking for mandatory light attributes
        
        //Instantiate required matrices for light object
        mat id = mat::Zero();
        mat is = mat::Zero();

        if(itr->contains("id"))
        {
            //Extract id
            int i = 0;
            for (auto itr2 =(*itr)["id"].begin(); itr2!= (*itr)["id"].end(); itr2++)
            {
                if(i<3)
                {
                    id(i++) = (*itr2).get<float>();
                } 
                else 
                {
                    std::cout<<"Warning: Too many entries in id"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing id specifier for light object"<<std::endl;
        }

        if(itr->contains("is"))
        {
            //Extract is
            int i = 0;
            for (auto itr2 =(*itr)["is"].begin(); itr2!= (*itr)["is"].end(); itr2++)
            {
                if(i<3)
                {
                    is(i++) = (*itr2).get<float>();
                } 
                else 
                {
                    std::cout<<"Warning: Too many entries in is"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing is specifier for light object"<<std::endl;
        }

        //Set attributes of light object
        light->setId(id);
        light->setIs(is);

        //Push light object into lights vector
        lights->push_back(light);
    }

    //Return reference to lights vector
    
    // std::cout << "done2" << std::endl;

    return lights;
}

//Parses the ouput objects of a JSON file
std::vector<Output*>* parseOutput(nlohmann::json& j)
{
    //Create an outputs vector that is going to hold all output objects
    std::vector<Output*>* outputs = new std::vector<Output*>();

    for (auto itr = j["output"].begin(); itr!= j["output"].end(); itr++){
        
        //Instantiate an output object
        Output* output = new Output();

        //Instantiate all matrices required for the output object
        Eigen::Vector2i size = Eigen::Vector2i::Zero();
        mat lookat = mat::Zero();
        mat up = mat::Zero();
        mat centre = mat::Zero();
        mat ai = mat::Zero();
        mat bkc = mat::Zero();
        std::vector<unsigned int>* raysperpixel = new std::vector<unsigned int>();

        if(itr->contains("filename"))
        {
            output->setFilename(new std::string((*itr)["filename"].get<std::string>()));
        }
        else
        {
            std::cerr<<"Error: Missing filename specifier for output object"<<std::endl;
            exit(1);
        }

        if(itr->contains("size"))
        {
            //Extract size
            int i = 0;
            for (auto itr2 =(*itr)["size"].begin(); itr2!= (*itr)["size"].end(); itr2++)
            {
                if(i<2)
                {
                    size(i++) = (*itr2).get<int>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in size"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing size specifier for output object"<<std::endl;
            exit(1);
        }

        if(itr->contains("lookat"))
        {
            //Extract lookat
            int i = 0;
            for (auto itr2 =(*itr)["lookat"].begin(); itr2!= (*itr)["lookat"].end(); itr2++)
            {
                if(i<3)
                {
                    lookat(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in lookat"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing lookat specifier for output object"<<std::endl;
            exit(1);
        }

        if(itr->contains("up"))
        {
            //Extract up
            int i = 0;
            for (auto itr2 =(*itr)["up"].begin(); itr2!= (*itr)["up"].end(); itr2++)
            {
                if(i<3)
                {
                    up(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in up"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing up specifier for output object"<<std::endl;
            exit(1);
        }

        if(itr->contains("centre"))
        {
            //Extract centre
            int i = 0;
            for (auto itr2 =(*itr)["centre"].begin(); itr2!= (*itr)["centre"].end(); itr2++)
            {
                if(i<3)
                {
                    centre(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in centre"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing centre specifier for output object"<<std::endl;
            exit(1);
        }

        if(itr->contains("ai"))
        {
            //Extract ai
            int i = 0;
            for (auto itr2 =(*itr)["ai"].begin(); itr2!= (*itr)["ai"].end(); itr2++)
            {
                if(i<3)
                {
                    ai(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in ai"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing ai specifier for output object"<<std::endl;
            exit(1);
        }

        if(itr->contains("bkc"))
        {
            //Extract bkc
            int i = 0;
            for (auto itr2 =(*itr)["bkc"].begin(); itr2!= (*itr)["bkc"].end(); itr2++)
            {
                if(i<3)
                {
                    bkc(i++) = (*itr2).get<float>();
                }
                else 
                {
                    std::cout<<"Warning: Too many entries in bkc"<<std::endl;
                }
            }
        }
        else
        {
            std::cerr<<"Error: Missing bkc specifier for output object"<<std::endl;
            exit(1);
        }

        if(itr->contains("fov"))
        {
            output->setFov(new float((*itr)["fov"].get<float>()));
        }
        else
        {
            std::cerr<<"Error: Missing fov specifier for output object"<<std::endl;
            exit(1); 
        }

        if(itr->contains("globalillum"))
        {
            output->setGlobalillum(new bool((*itr)["globalillum"].get<bool>()));
        }
        else
        {
            output->setGlobalillum(new bool(false));
        }

        if(itr->contains("twosiderender"))
        {
            output->setTwosiderender(new bool((*itr)["twosiderender"].get<bool>()));
        }
        else
        {
            output->setTwosiderender(new bool(true));
        }


        if(itr->contains("antialiasing") && output->getGlobalillum() != true)
        {
            output->setAntialiasing(new bool((*itr)["antialiasing"].get<bool>()));
        }
        else
        {
            output->setAntialiasing(new bool(false));
        }
        
        if(itr->contains("maxbounces"))
        {
            output->setMaxbounces(new float((*itr)["maxbounces"].get<float>()));
        }
        else
        {
            output->setMaxbounces(new float(5));
        }

        if(itr->contains("probterminate"))
        {
            output->setProbterminate(new float((*itr)["probterminate"].get<float>()));
        }
        else
        {
            output->setProbterminate(new float(0.5));
        }

        if(itr->contains("raysperpixel"))
        {
            //Extract raysperpixel
            int i = 0;
            for (auto itr2 =(*itr)["raysperpixel"].begin(); itr2!= (*itr)["raysperpixel"].end(); itr2++)
            {
                raysperpixel->push_back((*itr2).get<unsigned int>());
                i++;
                if(i > 3)
                {
                    std::cout<<"Warning: Too many entries in raysperpixel"<<std::endl;
                }
            }
        }
        else
        {
            output->setRaysperpixel(new std::vector<unsigned int>(0));
        }

        //Set all matrices
        output->setSize(size);
        output->setLookat(lookat);
        output->setUp(up);
        output->setCentre(centre);
        output->setAi(ai);
        output->setBkc(bkc);
        output->setRaysperpixel(raysperpixel);

        //Insert output object into outputs vector
        outputs->push_back(output);
    }

    //Return the reference of the outputs vector

    // std::cout << "done3" << std::endl;

    return outputs;
}
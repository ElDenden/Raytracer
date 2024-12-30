#include "RayTracer.h"
#include "Ray.h"

RayTracer::RayTracer(nlohmann::json& j){
    //We create the scene in here
    this->scene = new Scene(parseGeometry(j), parseLight(j), parseOutput(j));
}

void RayTracer::run(){
    //We compute the ray tracing algorithm and ouput a ppm file

    //Output loop
    for(auto outp = this->scene->getOutput()->begin(); outp != this->scene->getOutput()->end(); outp++)
    {
        Camera c(**outp);

        c.render(*this->scene);
    }
}

//Scene class

//Constructors

Scene::Scene(std::vector<shared_ptr<Geometry>>* geometry, std::vector<Light*>* light, std::vector<Output*>* output)
{
    this->geometry = geometry;
    this->light = light;
    this->output = output;

    //Check if there is an area light source
    for(auto itr = this->light->begin(); itr != this->light->end(); itr++)
    {
        if((**itr).getType() == "area"){
            for(auto itr2 = this->output->begin(); itr2 != this->output->end(); itr2++)
            {
                (**itr2).setAntialiasing(new bool(false));
            }
        }
    }
}

//Accessors

std::vector<shared_ptr<Geometry>>* Scene::getGeometry() const
{
    return this->geometry;
}

std::vector<Light*>* Scene::getLight() const
{
    return this->light;
}

std::vector<Output*>* Scene::getOutput() const
{
    return this->output;
}

//Pure Virtual Methods
bool Scene::hit(const Ray& ray, Interval t, HitRecord& record) const
{
    HitRecord temp;
    bool hit = false;
    float closest = t.max;

    for(const auto& obj : *geometry)
    {
        if(obj->hit(ray,Interval(t.min, closest),temp))
        {
            hit = true;
            closest = temp.t;
            record = temp;
        }
    }

    return hit;
}

//Camera class

Camera::Camera(Output& outp)
{
    //Image dimensions
    this->im_width = (int) outp.getSize()(0,0);
    this->im_height = (int) outp.getSize()(1,0);

    //Camera components
    this->look_at = outp.getLookat();
    this->camera_center = outp.getCentre();
    this->up = outp.getUp();
    this->fov = outp.getFov();

    //Filename

    this->filename = outp.getFilename();

    //Projection plane dimensions and axes vectors
    float p_height = 2 * std::sqrt(len(look_at)) * fabs((float) std::tan(PI* fov / 360 )); //Determines the height based on the fov

    float p_width = p_height * ((float)(im_width) / im_height);

    mat p_horizontal = mat::Zero();
    p_horizontal(0,0) = p_width;
    mat p_vertical = mat::Zero();
    p_vertical(1,0) = -p_height;

    //Pixel components
    float pix_space_width = p_width / im_width;
    float pix_space_height = p_height / im_height; 
    this->pix_horizontal = mat::Zero();
    this->pix_horizontal(0,0) = pix_space_width;
    this->pix_vertical = mat::Zero();
    this->pix_vertical(1,0) = -pix_space_height;

    //Define viewport's upper left pixel
    mat p_upper_left = camera_center + look_at -0.5*p_horizontal + -0.5*p_vertical;

    this->pixel00  = p_upper_left + 0.5* pix_horizontal + 0.5*pix_vertical;

    //Other components
    this->bkc = outp.getBkc();
    this->ai = outp.getAi();
    this->maxbounces = outp.getMaxbounces();
    this->probterminate = outp.getProbterminate();
    this->anti_aliasing = outp.getAntialiasing();
    this->raysperpixel = outp.getRaysperpixel();
    this->twosideisrender = outp.getTwosiderender();
    this->globalillum = outp.getGlobalillum();

    //Store the samples per pixel
    switch(raysperpixel.size())
    {
        case 0:
        {
            this->samples_per_pix = 1;
            if(globalillum)
            {
                this->samples_per_pix = 1000;
            }
        }
        break;
        case 1:
        {
            this->samples_per_pix = this->raysperpixel.at(0);
        }
        break;
        case 2:
        {
            this->samples_per_pix = this->raysperpixel.at(0) * this->raysperpixel.at(0) * this->raysperpixel.at(1);
        }
        break;
        case 3:
        {
            this->samples_per_pix = this->raysperpixel.at(0) * this->raysperpixel.at(1) * this->raysperpixel.at(2);
        }
        break;
    }
}

void Camera::render(const Scene& scene)
{
    //Render all pixels!
    std::vector<float> r;
    std::vector<float> g;
    std::vector<float> b;

    if(this->anti_aliasing)
    {
        std::cout << "aa" << std::endl;
        for(int j = 0; j < im_height; j++)
        {
            for(int i = 0; i < im_width; i++)
            {
                std::vector<mat> d_samples = pix_sample();
                mat pixel_loc = pixel00 + i*pix_horizontal + j*pix_vertical;
                mat color = mat::Zero();

                for(auto itr = d_samples.begin(); itr != d_samples.end(); itr++)
                {
                    mat pixel_loc_sample = pixel_loc + *itr;
                    
                    mat ray_direction = pixel_loc_sample - camera_center;

                    Ray ray(camera_center, ray_direction);

                    color += ray_color(ray, scene);
                }

                color = clamp(color / this->samples_per_pix);

                r.push_back(color(0,0));
                g.push_back(color(1,0));
                b.push_back(color(2,0));
            }
        }
    }
    else if (this->globalillum)
    {
        std::cout << "globalIllum" << std::endl;
        for(int j = 0; j < im_height; j++)
        {
            for(int i = 0; i < im_width; i++)
            {
                std::vector<mat> d_samples = pix_sample();
                mat pixel_loc = pixel00 + i*pix_horizontal + j*pix_vertical;
                mat color = mat::Zero();

                for(auto itr = d_samples.begin(); itr != d_samples.end(); itr++)
                {
                    mat pixel_loc_sample = pixel_loc + *itr;
                    
                    mat ray_direction = pixel_loc_sample - camera_center;

                    Ray ray(camera_center, ray_direction);

                    color += ray_color(ray, scene);
                }

                color = clamp(color / this->samples_per_pix);

                //Apply gamma correction
                color = gamma_correction(2.2, color);

                r.push_back(color(0,0));
                g.push_back(color(1,0));
                b.push_back(color(2,0));
            }
        }
    }
    else
    {
        std::cout << "no aa" << std::endl;
        for(int j = 0; j < im_height; j++)
        {
            for(int i = 0; i < im_width; i++)
            {
                mat pixel_loc = pixel00 + i*pix_horizontal + j*pix_vertical;
                mat ray_direction = pixel_loc - camera_center;

                Ray ray(camera_center, ray_direction);
                
                mat color = mat::Zero();

                color = ray_color(ray, scene);

                r.push_back(color(0,0));
                g.push_back(color(1,0));
                b.push_back(color(2,0));
            }
        }
    }
    
    std::cout << "all done" << std::endl;

    //PPM file
    saveasppm(filename, r, g, b, im_width, im_height);
}

//Free function definitions
mat Camera::ray_color(const Ray& ray, const Scene& scene)
{
    if(this->globalillum)
    {
        HitRecord temp;
        return globalIllumAlgo(ray, this->maxbounces, scene, temp, this->maxbounces);
    }

    return localIllumAlgo(ray,scene);
}

mat Camera::localIllumAlgo(const Ray& ray, const Scene& scene)
{
    HitRecord temp;

    if(scene.hit(ray, Interval(0.001, INF), temp))
    {
        //Find ray(s) going towards light source
        std::vector<mat> colors;

        for(auto itr = scene.getLight()->begin(); itr != scene.getLight()->end(); itr++)
        {
            if((*itr)->getType() == "point")
            {
                if((*itr)->isUsed() == false) continue; //Skips the light if specified in json

                mat direction = (*itr)->getCentre() - temp.position;

                HitRecord temp2;

                //Check if point is occluded
                if(!scene.hit(Ray(temp.position,direction), Interval(0.001, 0.999), temp2))
                {
                    //Point is not occluded
                    temp.L = normalize(direction);

                    temp.V = normalize(-1 * (temp.position - this->camera_center));

                    if(this->twosideisrender && temp.V.dot(temp.n) < 0)
                    {
                        temp.n *= -1;
                    }

                    temp.R = reflect(temp.L, temp.n);

                    //Calculate individual intensity
                    colors.push_back(B_phong(**itr, *temp.geo, temp));
                }
                else
                {   
                    //If point is occluded
                    mat black = mat::Zero();
                    colors.push_back(black);
                }
            }
            if((*itr)->getType() == "area")
            {
                std::vector<mat> points = (*itr)->sample();

                for(auto itr2 = points.begin(); itr2 !=points.end(); itr2++)
                {
                    //Check if point is occluded

                    mat direction = *itr2 - temp.position;

                    HitRecord temp2;

                    if(!scene.hit(Ray(temp.position,direction), Interval(0.001, 0.999), temp2))
                    {   
                        //Point is not occluded
                        temp.L = normalize(direction);

                        temp.V = normalize(-1 * (temp.position - this->camera_center));

                        if(this->twosideisrender && temp.V.dot(temp.n) < 0)
                        {
                            temp.n *= -1;
                        }

                        temp.R = reflect(temp.L,temp.n);

                        float div = ((*itr)->getUseCentre())? 1: ((*itr)->getGridDim() * (*itr)->getGridDim());

                        //Calculate individual intensity
                        colors.push_back(B_phong(**itr, *temp.geo, temp) / div);
                    }
                    else
                    {   
                        //If point is occluded
                        mat black = mat::Zero();
                        colors.push_back(black);
                    }
                }
            }
        }
        mat A = ai.cwiseProduct((*temp.geo).getAc()) * (*temp.geo).getKa();

        mat color = mat::Zero();

        // if((*temp.geo).getComment() == "small block top")
        // {
        //     std::cout << "size: " << colors.size() << std::endl;
        // }

        for(auto itr = colors.begin(); itr != colors.end(); itr++)
        {
            //std::cout << *(itr) <<std::endl;
            color += *(itr);
        }

        //std::cout << "color: " << color << std::endl;

        // std::cout << "ambient: " << A << std::endl;
        
        //std::cout << "result: " << color / colors.size() <<std::endl;
        color = clamp(color + A);

        // if((*temp.geo).getComment() == "small block top")
        // {
        //     std::cout << "color: " << color << std::endl;
        // }


        return color;
    }
    //std::cout << "hello from ray_color" << std::endl;

    //Return background color if nothing was hit
    return this->bkc;
}

mat Camera::globalIllumAlgo(const Ray& ray, int numberOfBounces, const Scene& scene, HitRecord& record, int stop)
{
    if(numberOfBounces <= 0 || stop <= 0)
    {
        //Calculate the direct illumination
        //Find ray(s) going towards light source
        std::vector<mat> colors;

        for(auto itr = scene.getLight()->begin(); itr != scene.getLight()->end(); itr++)
        {
            if((*itr)->getType() == "point")
            {
                if((*itr)->isUsed() == false) continue; //Skips the light if specified in json

                mat direction = (*itr)->getCentre() - record.position;

                HitRecord temp2;

                //Check if point is occluded
                if(!scene.hit(Ray(record.position,direction), Interval(0.001, 0.999), temp2))
                {
                    //Point is not occluded
                    record.L = normalize(direction);

                    record.V = normalize(-1 * (record.position - this->camera_center));

                    if(this->twosideisrender && dot(record.V,record.n) < 0)
                    {
                        record.n *= -1;
                    }

                    record.R = reflect(record.L, record.n);

                    //Calculate individual intensity
                    colors.push_back(B_phong_global(**itr, *record.geo, record));
                }
                else
                {   
                    //If point is occluded
                    mat black = mat::Zero();
                    colors.push_back(black);
                }
            }
            if((*itr)->getType() == "area")
            {
                (*itr)->useCentre(new bool(true));

                std::vector<mat> points = (*itr)->sample();

                for(auto itr2 = points.begin(); itr2 !=points.end(); itr2++)
                {
                    //Check if point is occluded

                    mat direction = *itr2 - record.position;

                    HitRecord temp2;

                    if(!scene.hit(Ray(record.position, direction), Interval(0.001, 0.999), temp2))
                    {   
                        //Point is not occluded
                        record.L = normalize(direction);

                        record.V = normalize(-1 * (record.position - this->camera_center));

                        if(this->twosideisrender && record.V.dot(record.n) < 0)
                        {
                            record.n *= -1;
                        }

                        record.R = reflect(record.L, record.n);

                        //Calculate individual intensity
                        colors.push_back(B_phong_global(**itr, *record.geo, record));
                    }
                    else
                    {
                        //If point is occluded
                        mat black = mat::Zero();
                        colors.push_back(black);
                    }
                }
            }
        }

        mat color = mat::Zero();

        for(auto itr = colors.begin(); itr != colors.end(); itr++)
        {
            color += *(itr);
        }

        // if(numberOfBounces == 0 && stop != 0 && record.geo->getComment() == "back_wall")
        // {
        //     std::cout << "color: " << color << std::endl;
        //     std::cout << "size: " << colors.size() << std::endl;
        //     std::cout << "aC: " << record.geo->getAc() << std::endl;
        //     std::cout << "Ka: " << record.geo->getKa() << std::endl;
        //     std::cout << "Dc: " << record.geo->getDc() << std::endl;
        //     std::cout << "Kd: " << record.geo->getKd() << std::endl;
        //     std::cout << "geo.getSc(): " << record.geo->getSc() << std::endl;
        //     std::cout << "geo.getKs(): " << record.geo->getKs() << std::endl;
        //     std::cout << "geo.getPc(): " << record.geo->getPc() << std::endl;
        // }

        color = clamp(color);

        return color;
    }

    HitRecord temp;

    if(scene.hit(ray, Interval(0.001, INF), temp))
    {
        //Sample a random point on the hemisphere at the location of the intersection
        temp.V = normalize(-1 * (temp.position - this->camera_center));
        if(this->twosideisrender && temp.V.dot(temp.n) < 0)
        {
            temp.n *= -1;
        }
        //Generate random vector on same side as normal
        mat rand = rand_on_hemisphere(temp.n);

        while(rand.dot(temp.n) > 0.999)
        {
            rand = rand_on_hemisphere(temp.n);
        }

        //Create basis vectors for the local coordinate frame

        mat y = temp.n.cross(rand);

        mat x = y.cross(temp.n);

        //Get cosine weighted direction vector sample
        mat dir = weighted_hemisphere_sampling();

        //Create transform matrix
        Eigen::Matrix<float, 3,3,Eigen::RowMajor> transform = Eigen::Matrix<float, 3,3,Eigen::RowMajor>::Zero();

        transform << x(0,0) , y(0,0) , temp.n(0,0),
                     x(1,0) , y(1,0) , temp.n(1,0),
                     x(2,0) , y(2,0) , temp.n(2,0);
        
        //Transform our direction to the coordinate frame of the intersection
        mat trans_dir = transform * dir;

        HitRecord temp3;

        while(!scene.hit(Ray(temp.position, trans_dir), Interval(0.001, INF), temp3))
        {
            //Get cosine weighted direction vector sample
            dir = weighted_hemisphere_sampling();
            
            //Transform our direction to the coordinate frame of the intersection
            trans_dir = transform * dir;
        }


        if(terminate(this->probterminate))
        {
            numberOfBounces = 0;
        }
        else
        {
            numberOfBounces--;
        }
        Light l;

        l.setId(globalIllumAlgo(Ray(temp.position, trans_dir), numberOfBounces, scene, temp, stop-1));
        
        //If we are at the point the viewer is looking at
        if(stop == this->maxbounces)
        {
            // mat color = mat::Zero();

            // std::vector<mat> colors;

            // colors.push_back(l.getId());

            //Check if pixel is not obstructed
            // for(auto itr = scene.getLight()->begin(); itr != scene.getLight()->end(); itr++)
            // {
            //     if((*itr)->getType() == "point")
            //     {

            //         if((*itr)->isUsed() == false) continue; //Skips the light if specified in json

            //         mat direction = (*itr)->getCentre() - temp.position;

            //         HitRecord temp2;

            //         //Check if point is occluded
            //         if(scene.hit(Ray(temp.position, direction), Interval(0.001, 0.999), temp2))
            //         {
            //             mat black = mat::Zero();

            //             colors.push_back(black);
            //         }
            //         else
            //         {
            //             // mat v = normalize(-1 * (temp.position - this->camera_center));
                        
            //             // float vn = dot(v, temp.n);

            //             // if(twosideisrender && vn < 0)
            //             // {
            //             //     temp.n *= -1;
            //             // }

            //             // mat l = normalize(direction);

            //             // mat r = reflect(l, temp.n);

            //             // float rv = dot(r,v);

            //             // float rv_pow;

            //             // if(rv < 0)
            //             // {
            //             //     rv = 0;
            //             // }

            //             // if(temp.geo->getPc() == 0 && rv ==0)
            //             // {
            //             //     rv_pow = 0;
            //             // }
            //             // else
            //             // {
            //             //     rv_pow = std::pow(rv, temp.geo->getPc());
            //             // }

            //             // mat spec = temp.geo->getKs() * p->getIs() * temp.geo->getSc() * rv_pow;

            //             // speculars.push_back(spec);
            //         }
            //     }
            //     if((*itr)->getType() == "area")
            //     {
            //         std::vector<mat> points = (*itr)->sample();

            //         for(auto itr2 = points.begin(); itr2 !=points.end(); itr2++)
            //         {
            //             //Check if point is occluded

            //             mat direction = *itr2 - temp.position;

            //             HitRecord temp2;

            //             if(scene.hit(Ray(temp.position, direction), Interval(0.001, 0.999), temp2))
            //             {   
            //                 mat black = mat::Zero();
            //                 colors.push_back(black);
            //             }
            //             else
            //             {
            //                 // mat v = normalize(-1 * (temp.position - this->camera_center));
                            
            //                 // float vn = dot(v, temp.n);

            //                 // if(twosideisrender && vn < 0)
            //                 // {
            //                 //     temp.n *= -1;
            //                 // }

            //                 // mat l = normalize(direction);

            //                 // mat r = reflect(l, temp.n);

            //                 // float rv = dot(r,v);

            //                 // float rv_pow;

            //                 // if(rv < 0)
            //                 // {
            //                 //     rv = 0;
            //                 // }

            //                 // if(temp.geo->getPc() == 0 && rv ==0)
            //                 // {
            //                 //     rv_pow = 0;
            //                 // }
            //                 // else
            //                 // {
            //                 //     rv_pow = std::pow(rv, temp.geo->getPc());
            //                 // }

            //                 // mat spec = temp.geo->getKs() * a->getIs() * temp.geo->getSc() * rv_pow;

            //                 // speculars.push_back(spec);
            //             }
            //         }
            //     }
            // }

            // //Calculate the total color that we see
            // for(auto itrC = colors.begin(); itrC != colors.end(); itrC++)
            // {
            //     color += *itrC;
            // }

            // color /= colors.size();

            mat A = this->ai.cwiseProduct((*temp.geo).getAc()) * (*temp.geo).getKa();

            mat result = clamp(l.getId() + A);
            return result;
        }

        mat direction = temp.position - record.position;

        record.L = normalize(direction);

        record.V = normalize(-1 * (record.position - this->camera_center));

        if(this->twosideisrender && record.V.dot(record.n) < 0)
        {
            record.n *= -1;
        }

        return B_phong_global(l, *record.geo, record);
    }

    //std::cout << "hello from ray_color" << std::endl;

    //Return background color if nothing was hit
    return this->bkc;
}

mat Camera::B_phong_global(const Light& light, const Geometry& geo, const HitRecord& record)
{
    float ln = record.n.dot(record.L);

    mat B;

    if(ln > 0.01)
    {
        B = light.getId().cwiseProduct(geo.getDc()) * geo.getKd() * ln;
    }
    else
    {
        B = mat::Zero();
    }

    return B;
}

mat Camera::B_phong(const Light& light, const Geometry& geo, const HitRecord& record)
{   
    // std::cout << "ai: " << ai << std::endl;
    // std::cout << "aC: " << geo.getAc() << std::endl;
    // std::cout << "Ka: " << geo.getKa() << std::endl;
    // std::cout << "Id: " << light.getId() << std::endl;
    // std::cout << "Dc: " << geo.getDc() << std::endl;
    // std::cout << "Kd: " << geo.getKd() << std::endl;
    // std::cout << "record.n: " << record.n << std::endl;
    // std::cout << "record.L: " << record.L << std::endl;
    // std::cout << "light.getIs(): " << light.getIs() << std::endl;
    // std::cout << "geo.getSc(): " << geo.getSc() << std::endl;
    // std::cout << "geo.getKs(): " << geo.getKs() << std::endl;
    // std::cout << "normalize(-1 * record.position): " << normalize(-1 * record.position) << std::endl;
    // std::cout << "record.R: " << record.R << std::endl;
    // std::cout << "geo.getPc(): " << geo.getPc() << std::endl;

    //std::cout << "A: " << A << std::endl;


    float ln = record.n.dot(record.L);
    float rv = record.V.dot(record.R);
    float rv_old = rv;
    float rv_pow;

    if(rv < 0)
    {
        rv = 0;
    }

    if(geo.getPc() == 0 && rv == 0)
    {
        rv_pow = 0;
    }
    else
    {
        rv_pow = std::pow(rv, geo.getPc());
    }

    mat B;
    mat C;
    
    if(ln > 0.01)
    {
        B = light.getId().cwiseProduct(geo.getDc()) * geo.getKd() * ln;
        C = light.getIs().cwiseProduct(geo.getSc()) * geo.getKs() * rv_pow;
    }
    else
    {
        B = mat::Zero();
        C = mat::Zero();
    }

    // if(geo.getComment() == "small block top")
    // {
    //     std::cout << "B: " << B << std::endl;

    //     std::cout << "C: " << C << std::endl;
    // }

    mat I = B + C;

    // if(I(0,0) > 0.9 && I(1,0) && 0.9 && I(2,0) > 0.9)
    // {
    //     std::cout << "ai: " << ai << std::endl;
    //     std::cout << "aC: " << geo.getAc() << std::endl;
    //     std::cout << "Ka: " << geo.getKa() << std::endl;
    //     std::cout << "Id: " << light.getId() << std::endl;
    //     std::cout << "Dc: " << geo.getDc() << std::endl;
    //     std::cout << "Kd: " << geo.getKd() << std::endl;
    //     std::cout << "record.n: " << record.n << std::endl;
    //     std::cout << "record.L: " << record.L << std::endl;
    //     std::cout << "light.getIs(): " << light.getIs() << std::endl;
    //     std::cout << "geo.getSc(): " << geo.getSc() << std::endl;
    //     std::cout << "geo.getKs(): " << geo.getKs() << std::endl;
    //     std::cout << "normalize(-1 * (record.position - this->camera_center)): " << normalize(-1 * (record.position - this->camera_center)) << std::endl;
    //     std::cout << "record.R: " << record.R << std::endl;
    //     std::cout << "geo.getPc(): " << geo.getPc() << std::endl;
    //     std::cout << "rvpow: " << rv_pow << std::endl;
    //     std::cout << "rv: " << rv << std::endl;
    //     std::cout << "rv_old: " << rv_old << std::endl;
    //     std::cout << "\ncolor: " << I << std::endl;
    //     std::cout << "position: " << record.position << std::endl;
    //     std::cout << "C: " << C <<std::endl;
    //     std::cout << "B: " << B << std::endl;
    // }
    //std::cout << "I: " << I << std::endl;
    return I;
}

mat Camera::reflect(const mat& direction, const mat& normal)
{
    return 2 * direction.dot(normal) * normal - direction;
}

std::vector<mat> Camera::pix_sample()
{   
    std::vector<mat> displacements(this->samples_per_pix);
    int i{0};
    switch(raysperpixel.size())
    {
        case 0:
        {
            for(int b = 0; b < 1000; b++)
            {
                float dx =  -0.5 + random_float();
                float dy =  -0.5 + random_float();

                mat dis = dx * this->pix_horizontal + dy * this->pix_vertical;
                displacements[i++] = dis;
            }
        }
        break;
        case 1:
        {
            // random uniform sampling
            for(int b = 0; b < this->raysperpixel.at(0); b++)
            {
                float dx =  -0.5 + random_float();
                float dy =  -0.5 + random_float();

                mat dis = dx * this->pix_horizontal + dy * this->pix_vertical;
                displacements[i++] = dis;
            }
        }
        break;
        case 2:
        {
            // axa random uniform sampling with b samples
            //Grid basis vectors
            mat g_x = this->pix_horizontal / this->raysperpixel.at(0);
            mat g_y = this->pix_vertical / this->raysperpixel.at(0);

            //Starting position of the grid
            mat start = -0.5 * this->pix_horizontal + -0.5 * this->pix_vertical;

            //Sample generation for each grid cell
            for(int v = 0; v < this->raysperpixel.at(0); v++)
            {
                for(int u = 0; u < this->raysperpixel.at(0); u++)
                {
                    for(int b = 0; b < this->raysperpixel.at(1); b++)
                    {
                        mat dis_x = random_float() * g_x;
                        mat dis_y = random_float() * g_y;

                        mat sample = start + u * g_x + v * g_y + dis_x + dis_y;

                        displacements[i++] = sample;
                    }
                }
            }
        }
        break;
        case 3:
        {
            // axb random uniform sampling with c samples
            //Grid basis vectors
            mat g_x = this->pix_horizontal / this->raysperpixel.at(1);
            mat g_y = this->pix_vertical / this->raysperpixel.at(0);

            mat start = -0.5 * this->pix_horizontal + -0.5 * this->pix_vertical;

            for(int v = 0; v < this->raysperpixel.at(0); v++)
            {
                for(int u = 0; u < this->raysperpixel.at(1); u++)
                {
                    for(int c = 0; c < this->raysperpixel.at(2); c++)
                    {
                        mat dis_x = random_float() * g_x;
                        mat dis_y = random_float() * g_y;

                        mat sample = start + u * g_x + v * g_y + dis_x + dis_y;

                        displacements[i++] = sample;
                    }
                }
            }
        }
        break;
    }

    return displacements;
}

mat Camera::gamma_correction(float gamma, const mat& color)
{
    return color.array().pow(1/gamma).matrix();
}
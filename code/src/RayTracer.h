#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "json_objparser.h"
//myppmsaver requires <string>
#include "myppmsaver.h"
#include "Hittable.h"
#include "Utility.h"

class Scene;

class RayTracer {
  public:
    //Constructors

    /// @brief Constructs and sets the scene for the raytracer
    /// @param j A json object
    RayTracer(nlohmann::json& j);

    void run();

  ;
  private:
    Scene* scene;
  ;
};

class Scene : public Hittable {
  public:
    Scene(std::vector<shared_ptr<Geometry>>* geometry, std::vector<Light*>* light, std::vector<Output*>* output);
    //Accessor methods

    //Get Geometry
    std::vector<shared_ptr<Geometry>>* getGeometry() const;
    //Get Light
    std::vector<Light*>* getLight() const;
    //Get Output
    std::vector<Output*>* getOutput() const;

    //Memory management methods
    void clear() {geometry->clear();};

    //Pure Virtual methods
    bool hit(const Ray& ray, Interval t, HitRecord& rec) const override;
  private:
    std::vector<shared_ptr<Geometry>>* geometry;
    std::vector<Light*>* light;
    std::vector<Output*>* output;
};

class Light;

class Camera
{
    public:
      Camera(Output& outp);
      /// @brief Renders the given scene
      /// @param scene Scene object containing all information about the scene
      void render(const Scene& scene);

      mat ray_color(const Ray& ray, const Scene& obj);

      mat B_phong(const Light& light, const Geometry& geo, const HitRecord& record);

      mat B_phong_global(const Light& light, const Geometry& geo, const HitRecord& record);

      mat localIllumAlgo(const Ray& ray, const Scene& scene);

      mat globalIllumAlgo(const Ray& ray, int numberOfBounces, const Scene& scene, HitRecord& record, int stop);

      mat reflect(const mat& ray, const mat& normal);

      mat gamma_correction(float gamma, const mat& color);

      //Antialiasing
      /// @brief Generates a sample displacement around the current pixel at i,j
      /// @param i The width position of the pixel
      /// @param j The height position of the pixel
      /// @return A vector containing the displacements of the samples from the center of the pixel at i,j
      std::vector<mat> pix_sample();
    ;

    private:
      //Image components

      int im_width;
      int im_height;

      //Camera components
      mat look_at;
      mat camera_center;
      mat up;
      float fov;

      //Pixel components

      mat pixel00;
      mat pix_horizontal;
      mat pix_vertical;

      //File Name
      std::string filename;

      //Other components
      mat bkc;
      mat ai;
      float maxbounces;
      float probterminate;
      bool anti_aliasing;
      std::vector<unsigned int> raysperpixel;
      unsigned int samples_per_pix;
      bool twosideisrender;
      bool globalillum;
    ;
};

#endif





#include "myppmsaver.h"

//Most of this code was taken from the simpleppm.h file

 int saveasppm(std::string file_name, const std::vector<float>& r, const std::vector<float>& g, const std::vector<float>& b, int dimx, int dimy){
   
    std::cout << file_name << std::endl;
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    ofs << "P6" << std::endl << dimx << ' ' << dimy << std::endl << "255" << std::endl;
 
    for (unsigned int j = 0; j < dimy; ++j)
        for (unsigned int i = 0; i < dimx; ++i)
        {   
            //std::cout << "r: " << (255.0 * r[dimx*j + i]) << " g: " << (255.0 * g[dimx*j + i]) << " b: " << (255.0 * b[dimx*j + i]) << std::endl;
            ofs << (char)(255.0 * r[dimx*j + i]) <<  (char)(255.0 * g[dimx*j + i]) << (char)(255.0 * b[dimx*j + i]);
        }
    
    ofs.close();
 
    return 0;
}
// This code only shows how to load an object. Input this into your
// program to also draw it.

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"


int main( int argc, char** argv )
{

  std::string inputfile = "../../common/data/bunny.obj";
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  int flags = 0; // see load_flags_t enum for more information.

  // Load the object
  bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), 0, flags);

  // Check for errors
  if (!err.empty()) { // `err` may contain warning message.
    std::cerr << err << std::endl;
  }

  if (!ret) {
    exit(1);
  }
  else
  {
    std::cout << "Loaded " << inputfile
              << " ("
              << shapes.size() << " shapes"
              << ")"
              << "\n";
  }


  // Populate your VBO with shapes[0].mesh.positions and shapes[0].mesh.normals
  // ...
  // Populate your EBO with shapes[0].mesh.indices
  // ...
  

  return 0;
}




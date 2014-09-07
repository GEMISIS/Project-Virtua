#ifndef _WAVEFRONT_OBJECT_H_
#define _WAVEFRONT_OBJECT_H_

#include "tiny_obj_loader.h"
#include "lodepng.h"

#include <vector>

using namespace tinyobj;

class WavefrontObject
{
public:
	WavefrontObject(const char* filename);
	void Draw();
private:
	unsigned int totalShapes;
	std::vector<unsigned int> verticesHandles;
	std::vector<unsigned int> uvHandles;
	std::vector<unsigned int> normalsHandles;
	std::vector<unsigned int> indicesHandles;
	std::vector<unsigned int> meshSizes;
	std::vector<unsigned int> textures;

	void loadTexture(tinyobj::shape_t* shape, int spot);
};

#endif
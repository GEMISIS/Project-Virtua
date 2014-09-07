#include "pvmm/WavefrontObject.h"
#include "pvmm/MidOpenGL.h"

using namespace PV;

WavefrontObject::WavefrontObject(const char* filename)
{
	std::vector<tinyobj::shape_t> shapes;
	LoadObj(shapes, filename);

	for (int i = 0; i < shapes.size(); i += 1)
	{
		mesh_t* mesh = &shapes[i].mesh;

		pv_glEnableVertexAttribArray(0);
		unsigned int verticesHandle = 0;
		pv_glGenBuffers(1, &verticesHandle);
		pv_glBindBuffer(PV_GL_ARRAY_BUFFER, verticesHandle);
		pv_glBufferData(PV_GL_ARRAY_BUFFER, mesh->positions.size() * sizeof(float), &mesh->positions[0], PV_GL_STATIC_DRAW);
		pv_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		this->verticesHandles.push_back(verticesHandle);

		if (mesh->normals.size() > 0)
		{
			unsigned int normalsHandle = 0;
			pv_glEnableVertexAttribArray(1);
			pv_glGenBuffers(1, &normalsHandle);
			pv_glBindBuffer(PV_GL_ARRAY_BUFFER, normalsHandle);
			pv_glBufferData(PV_GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(float), &mesh->normals[0], PV_GL_STATIC_DRAW);
			pv_glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			this->normalsHandles.push_back(normalsHandle);
		}
		else
		{
			this->normalsHandles.push_back(0);
		}

		if (mesh->texcoords.size() > 0)
		{
			unsigned int uvHandle = 0;
			pv_glEnableVertexAttribArray(2);
			pv_glGenBuffers(1, &uvHandle);
			pv_glBindBuffer(PV_GL_ARRAY_BUFFER, uvHandle);
			pv_glBufferData(PV_GL_ARRAY_BUFFER, mesh->texcoords.size() * sizeof(float), &mesh->texcoords[0], PV_GL_STATIC_DRAW);
			pv_glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			this->uvHandles.push_back(uvHandle);
		}
		else
		{
			this->uvHandles.push_back(0);
		}

		unsigned int indicesHandle = -1;
		pv_glEnableVertexAttribArray(3);
		pv_glGenBuffers(1, &indicesHandle);
		pv_glBindBuffer(PV_GL_ELEMENT_ARRAY_BUFFER, indicesHandle);
		pv_glBufferData(PV_GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], PV_GL_STATIC_DRAW);
		pv_glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		this->indicesHandles.push_back(indicesHandle);

		this->meshSizes.push_back(mesh->indices.size());

		pv_glBindBuffer(PV_GL_ELEMENT_ARRAY_BUFFER, 0);
		pv_glBindBuffer(PV_GL_ARRAY_BUFFER, 0);
		pv_glDisableVertexAttribArray(0);
		pv_glDisableVertexAttribArray(1);
		pv_glDisableVertexAttribArray(2);

		this->loadTexture(&shapes[i], i);

		mesh = NULL;
	}
	this->totalShapes = shapes.size();
	shapes.clear();
}

void WavefrontObject::loadTexture(tinyobj::shape_t* shape, int spot)
{
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, shape->material.diffuse_texname);

	this->textures.push_back(0);
	if (error == 0)
	{
		glGenTextures(1, &this->textures[spot]);
		glBindTexture(GL_TEXTURE_2D, this->textures[spot]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, PV_GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, PV_GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, PV_GL_GENERATE_MIPMAP, GL_TRUE);
		pv_glGenerateMipmap(GL_TEXTURE_2D);
	}
}

void WavefrontObject::Draw()
{
	for (int i = 0; i < this->totalShapes; i += 1)
	{
		glBindTexture(GL_TEXTURE_2D, this->textures[i]);
		pv_glEnableVertexAttribArray(0);
		pv_glBindBuffer(PV_GL_ARRAY_BUFFER, this->verticesHandles[i]);
		pv_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		if (this->normalsHandles.size() > i)
		{
			if (this->normalsHandles[i] != 0)
			{
				pv_glEnableVertexAttribArray(1);
				pv_glBindBuffer(PV_GL_ARRAY_BUFFER, this->normalsHandles[i]);
				pv_glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}
		}
		if (this->uvHandles.size() > i)
		{
			if (this->uvHandles[i] != 0)
			{
				pv_glEnableVertexAttribArray(2);
				pv_glBindBuffer(PV_GL_ARRAY_BUFFER, this->uvHandles[i]);
				pv_glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			}
		}
		pv_glEnableVertexAttribArray(3);
		pv_glBindBuffer(PV_GL_ELEMENT_ARRAY_BUFFER, this->indicesHandles[i]);
		pv_glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawElements(GL_TRIANGLE_STRIP, this->meshSizes[i], GL_UNSIGNED_INT, (void*)0);
		pv_glBindBuffer(PV_GL_ELEMENT_ARRAY_BUFFER, 0);
		pv_glBindBuffer(PV_GL_ARRAY_BUFFER, 0);

		pv_glDisableVertexAttribArray(0);
		pv_glDisableVertexAttribArray(1);
		pv_glDisableVertexAttribArray(2);
		pv_glDisableVertexAttribArray(3);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

#include <Windows.h>
#include <algorithm>
#include "pvmm/MidOpenGL.h"
#include "pvmm/wavefrontObject.h"

namespace PV
{
	void WavefrontObject::loadGraphics(const char* fileName)
	{
		this->arrayHandles = (unsigned int*)malloc(this->faces.size() * sizeof(unsigned int));
		pv_glGenVertexArrays(this->faces.size(), this->arrayHandles);
		for (unsigned int j = 0; j < this->faces.size(); j += 1)
		{
			pv_glBindVertexArray(this->arrayHandles[j]);

			pv_glEnableVertexAttribArray(0);

			pv_glGenBuffers(1, &this->faces[j].modelVerticesHandle);
			pv_glBindBuffer(PV_GL_ARRAY_BUFFER, this->faces[j].modelVerticesHandle);
			pv_glBufferData(PV_GL_ARRAY_BUFFER, this->faces[j].outputVertexIndices.size(), &this->faces[j].outputVertexIndices[0], PV_GL_STATIC_DRAW);
			pv_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			if (this->faces[j].hasUVs)
			{
				pv_glEnableVertexAttribArray(1);
				pv_glGenBuffers(1, &this->faces[j].modelUVsHandle);
				pv_glBindBuffer(PV_GL_ARRAY_BUFFER, this->faces[j].modelUVsHandle);
				pv_glBufferData(PV_GL_ARRAY_BUFFER, this->faces[j].outputUVIndices.size(), &this->faces[j].outputUVIndices[0], PV_GL_STATIC_DRAW);
				pv_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			}

			if (this->faces[j].hasNormals)
			{
				pv_glEnableVertexAttribArray(2);
				pv_glGenBuffers(1, &this->faces[j].modelNormalsHandle);
				pv_glBindBuffer(PV_GL_ARRAY_BUFFER, this->faces[j].modelNormalsHandle);
				pv_glBufferData(PV_GL_ARRAY_BUFFER, this->faces[j].outputNormalIndices.size(), &this->faces[j].outputNormalIndices[0], PV_GL_STATIC_DRAW);
				pv_glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}
		}
	}

	void WavefrontObject::draw()
	{
		for (unsigned int i = 0; i < this->faces.size(); i += 1)
		{
			pv_glBindVertexArray(this->arrayHandles[i]);
			switch (this->faces[i].drawType)
			{
			case NONE:
				glDrawArrays(GL_TRIANGLES, 0, this->faces[i].outputVertexIndices.size() / 3);
				break;
			case TRIANGLES:
				glDrawArrays(GL_TRIANGLES, 0, this->faces[i].outputVertexIndices.size() / 3);
				break;
			case QUADS:
				glDrawArrays(GL_QUADS, 0, this->faces[i].outputVertexIndices.size() / 4);
				break;
			}
		}
	}
}

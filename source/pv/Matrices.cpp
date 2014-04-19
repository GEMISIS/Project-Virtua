#include <math.h>
#include "pv/Matrices.h"

namespace PV
{
	namespace Math
	{
		template <class T>
		Matrix<T>::Matrix(unsigned int width, unsigned int height)
		{
			this->width = width;
			this->height = height;
			this->matrix = new T[width * height];
			this->SetIdentity();
		}
		template <>
		Matrix<float> Matrix<float>::operator+(const Matrix<float> inputMatrix)
		{
			Matrix<float> outputMatrix(this->width, this->height);

			for (int i = 0; i < this->width * this->height && i < inputMatrix.width * inputMatrix.height; i += 1)
			{
				outputMatrix[i] = this->matrix[i] + inputMatrix[i];
			}
			return outputMatrix;
		}
		template <>
		Matrix<float> Matrix<float>::operator*(Matrix<float> inputMatrix)
		{
			if (this->width == inputMatrix.height)
			{
				Matrix<float> outputMatrix(this->width, inputMatrix.height);

				for (int y = 0; y < outputMatrix.height; y += 1)
				{
					for (int x = 0; x < outputMatrix.width; x += 1)
					{
						outputMatrix[x + y * outputMatrix.width] = 0.0f;
						for (int i = 0; i < this->width; i += 1)
						{
							float valueA = this->matrix[x + i * outputMatrix.width];
							float valueB = inputMatrix.matrix[i + y * outputMatrix.width];
							outputMatrix[x + y * outputMatrix.width] += valueA * valueB;
						}
					}
				}

				return outputMatrix;
			}
			return *this;
		}
		template <>
		Matrix<float> Matrix<float>::operator=(const float* inputMatrix)
		{
			int length = width * height;
			if (this->matrix != 0)
			{
				delete this->matrix;
			}
			this->matrix = new float[length];
			for (int i = 0; i < length; i += 1)
			{
				this->matrix[i] = inputMatrix[i];
			}
			return *this;
		}
		template <class T>
		T& Matrix<T>::operator[](const int subscript)
		{
			return this->matrix[subscript];
		}
		template <class T>
		const T& Matrix<T>::operator[](const int subscript) const
		{
			return this->matrix[subscript];
		}
		template <>
		void Matrix<float>::Rotate(float pitch, float yaw, float roll)
		{
			Matrix<float> pitchMatrix(this->width, this->height);
			Matrix<float> yawMatrix(this->width, this->height);
			Matrix<float> rollMatrix(this->width, this->height);

			pitchMatrix[5] = cos(-pitch);
			pitchMatrix[6] = -sin(-pitch);
			pitchMatrix[9] = sin(-pitch);
			pitchMatrix[10] = cos(-pitch);

			yawMatrix[0] = cos(-yaw);
			yawMatrix[2] = sin(-yaw);
			yawMatrix[8] = -sin(-yaw);
			yawMatrix[10] = cos(-yaw);

			rollMatrix[0] = cos(-roll);
			rollMatrix[1] = -sin(-roll);
			rollMatrix[4] = sin(-roll);
			rollMatrix[5] = cos(-roll);

			*this = (pitchMatrix * yawMatrix * rollMatrix) * *this;
		}
		template<>
		void Matrix<float>::Translate(float x, float y, float z)
		{
			this->matrix[12] += x;
			this->matrix[13] += y;
			this->matrix[14] += z;
		}
		template<>
		void Matrix<float>::SetIdentity()
		{
			for (int y = 0; y < height; y += 1)
			{
				for (int x = 0; x < width; x += 1)
				{
					if (x == y)
					{
						this->matrix[x + y * width] = 1.0f;
					}
					else
					{
						this->matrix[x + y * width] = 0.0f;
					}
				}
			}
		}
		template <>
		float* Matrix<float>::getArray()
		{
			return this->matrix;
		}
	};
};


#include "pv/Matrices.h"

namespace PV
{
	namespace Math
	{
		Matrix<float>::Matrix(unsigned int width, unsigned int height)
		{
			this->matrix = NULL;
			this->width = width;
			this->height = height;
		}
		template <>
		Matrix<float> Matrix<float>::operator+(Matrix<float> inputMatrix)
		{
			Matrix<float> outputMatrix(this->width, this->height);
			outputMatrix.matrix = new float[this->width * this->height];

			for (int i = 0; i < this->width * this->height && i < inputMatrix.width * inputMatrix.height; i += 1)
			{
				float value = this->matrix[i] + inputMatrix[i];
				outputMatrix[i] = value;
			}
			return outputMatrix;
		}
		template <>
		Matrix<float> Matrix<float>::operator*(Matrix<float> inputMatrix)
		{
			if (this->width == inputMatrix.height)
			{
				Matrix<float> outputMatrix(this->width, inputMatrix.height);
				outputMatrix.matrix = new float[this->width * inputMatrix.height];

				Matrix<float> matA(this->width, this->height);
				matA = this->matrix;

				Matrix<float> matB(inputMatrix.width, inputMatrix.height);
				matB = inputMatrix.matrix;

				for (int y = 0; y < outputMatrix.height; y += 1)
				{
					for (int x = 0; x < outputMatrix.width; x += 1)
					{
						float finalValue = 0.0f;
						for (int i = 0; i < matA.width; i += 1)
						{
							float valueA = matA[i + y * matA.width];
							float valueB = matB[x + i * matB.width];
							float temp = valueA * valueB;
							finalValue = finalValue + temp;
						}
						outputMatrix[x + y * outputMatrix.width] = finalValue;
					}
				}

				return outputMatrix;
			}
			else if (this->width == inputMatrix.height)
			{
				Matrix<float> outputMatrix(this->height, inputMatrix.width);
				outputMatrix.matrix = new float[this->width * inputMatrix.height];

				Matrix<float> matA(inputMatrix.width, inputMatrix.height);
				matA = inputMatrix.matrix;

				Matrix<float> matB(this->width, this->height);
				matB = this->matrix;

				for (int y = 0; y < outputMatrix.height; y += 1)
				{
					for (int x = 0; x < outputMatrix.width; x += 1)
					{
						float finalValue = 0.0f;
						for (int i = 0; i < matA.width; i += 1)
						{
							float valueA = matA[i + y * matA.width];
							float valueB = matB[x + i * matB.width];
							float temp = valueA * valueB;
							finalValue = finalValue + temp;
						}
						outputMatrix[x + y * outputMatrix.width];
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
			if (this->matrix != NULL)
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
		template <>
		float& Matrix<float>::operator[](int subscript)
		{
			return this->matrix[subscript];
		}
		template <>
		float* Matrix<float>::getArray()
		{
			return this->matrix;
		}
	};
};


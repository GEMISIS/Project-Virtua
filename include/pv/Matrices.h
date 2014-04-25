#ifndef _MATRICES_H_
#define _MATRICES_H_

#include "pv/types.h"
#include <math.h>

namespace PV
{
	namespace Math
	{
		/**
		* A vector with 2 components (x and y).
		*/
		struct vec2
		{
			float x, y;
		};
		/**
		* A vector with 3 components (x, y, and z).
		*/
		struct vec3
		{
			float x, y, z;
		};
		/**
		* Normalizes a vector to be between 0 and 1 and returns the result..
		* @param vector The vector to normalize.
		* @return Returns the vector normalized to be between 0 and 1.
		*/
		inline vec2 normalize(vec2 vector)
		{
			float length = sqrtf((vector.x * vector.x) + (vector.y * vector.y));
			vec2 output = { vector.x / length, vector.y / length };
			return output;
		}
		/**
		* Normalizes a vector to be between 0 and 1 and returns the result..
		* @param vector The vector to normalize.
		* @return Returns the vector normalized to be between 0 and 1.
		*/
		inline vec3 normalize(vec3 vector)
		{
			float length = sqrtf((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
			vec3 output = { vector.x / length, vector.y / length, vector.z / length };
			return output;
		}
		/**
		* Cross multiplies two vectors and returns the result.
		* @param a The first vector to use in the cross multiplication.
		* @param b The second vector to use in the cross multiplication.
		* @return Returns the product of the the two vectors.
		*/
		inline vec2 crossMultiply(vec2 a, vec2 b)
		{
			vec2 output = { a.x * b.y, a.y * b.x };
			return output;
		}
		/**
		* Cross multiplies two vectors and returns the result.
		* @param a The first vector to use in the cross multiplication.
		* @param b The second vector to use in the cross multiplication.
		* @return Returns the product of the the two vectors.
		*/
		inline vec3 crossMultiply(vec3 a, vec3 b)
		{
			vec3 output = { a.y * b.z - a.z * b.y, a.z * b.x - a.x* b.z, a.x * b.y - a.y * b.x };
			return output;
		}
		/**
		 * A class for creating and managing matrices.  These matrices can be of any size and shape, and allow for
		 * column major and row major tracking.  They also have basic math operations that can performed with them too.
		 */
		template <class T>
		class Matrix
		{
		public:
			Matrix(unsigned int width, unsigned int height);
			/**
			* The operator overload for multiplication.
			*/
			Matrix operator*(Matrix<T> inputMatrix);
			/**
			* The operator overload for addition.
			*/
			Matrix operator+(const Matrix<T> inputMatrix);
			/**
			* The operator overload for setting a matrix.
			*/
			Matrix operator=(const T* inputMatrix);
			/**
			* The operator overload for arrays.
			*/
			T& operator[](const int subscript);
			/**
			* The operator overload for arrays.
			*/
			const T& operator[](const int subscript) const;
			/**
			 * Rotates the matrix by the angles specified in degrees.
			 * @param pitch The amount to rotate on the X axis in degrees.
			 * @param yaw The amount to rotate on the Y axis in degrees.
			 * @param roll The amount to rotate on the Z axis in degrees.
			 */
			void Rotate(T pitch, T yaw, T roll);
			/**
			 * Translates the matrix by a specific amount.
			 *
			 * @param x The amount to translate on the X axis.
			 * @param y The amount to translate on the Y axis.
			 * @param z The amount to translate on the Z axis.
			 */
			void Translate(T x, T y, T z);
			/**
			 * Sets the matrix to an identity matrix.
			 */
			void SetIdentity();
			/**
			 * Gets the matrix as an array of type T.
			 */
			T* getArray();
		protected:
			/**
			 * The actual matrix data.
			 */
			T* matrix;
			/**
			 * A boolean indicating whether the matrix is column major or not.
			 */
			bool columnMajor;
			/**
			 * The width of the matrix.
			 */
			unsigned int width;
			/**
			* The height of the matrix.
			*/
			unsigned int height;
		};
	};
};

#endif
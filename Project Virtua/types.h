#ifndef _TYPES_H_
#define _TYPES_H_

#include <ovr/Include/OVR.h>

using namespace OVR;
using namespace OVR::Util::Render;

/**
 * This is a structure used for storing rotation data.
 *
 * This is a structure used for storing rotation data as Eular angles.
 * It has an X, Y, Z, and W property.
 */
struct rotation_t
{
	float x;
	float y;
	float z;
	float w;
};

/**
 * This is a structure used for orientation data.
 *
 * This is a structure used for orientation data.  The
 * angles are stored as Eular angles.
 */
struct orientation_t
{
	/**
	 * Rotation on the Y axis.
	 */
	float yaw;
	/**
	 * Rotation on the X axis.
	 */
	float pitch;
	/**
	 * Rotation on the Z axis.
	 */
	float roll;
};

/**
 * This is a structure used for orientation data.
 *
 * This is a structure used for orientation data.  The
 * angles are stored as Quartonian angles.
 */
struct orientation_quart_t
{
	/**
	 * A vector with X, Y, and Z variables respresenting the axis as normals that
	 * is being roated around.
	 */
	Vector3f axis;
	/**
	 * The angle of rotation for this orientation.
	 */
	float angle;
};



#endif
#ifndef _KINECT1_H_
#define _KINECT1_H_

#ifndef EXCLUDE_KINECT1
#include <Windows.h>
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>

typedef enum
{
	NotDetected = 0,
	NotEnoughBandwidth = 0,
	Ready = 1
}KinectStatus;

class Kinect1
{
public:
	Kinect1();
	Kinect1(int index);

	const KinectStatus Status() const;

	~Kinect1();
protected:
	INuiSensor* sensor;
	KinectStatus status;

	void InitializeSensor(int index);
};
#endif

#endif


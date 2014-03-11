#ifndef _KINECT_H_
#define _KINECT_H_

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

class Kinect
{
public:
	Kinect();
	Kinect(int index);

	const KinectStatus Status() const;

	~Kinect();
protected:
	INuiSensor* sensor;
	KinectStatus status;

	void InitializeSensor(int index);
};

#endif
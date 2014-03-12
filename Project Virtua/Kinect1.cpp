#include "pv/Kinect1.h"

#ifndef EXCLUDE_KINECT1
Kinect1::Kinect1()
{
	this->status = NotDetected;
	this->InitializeSensor(0);
}

Kinect1::Kinect1(int index)
{
	this->status = NotDetected;
	this->InitializeSensor(index);
}

KinectStatus Kinect1::InitializeSensor(int index)
{
	if(this->status != Ready)
	{
		int totalSensors = 0;
		KinectStatus status = this->status = NotDetected;

		if(NuiGetSensorCount(&totalSensors) > -1 || totalSensors > index)
		{
			if(NuiCreateSensorByIndex(index, &this->sensor) > -1)
			{
				this->sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
				status = this->status = Ready;
			}
			else if(this->sensor == NULL)
			{
				status = this->status = NotEnoughBandwidth;
			}
		}
		return status;
	}
	return Ready;
}



#endif


#include "pv/Kinect.h"

#ifndef EXCLUDE_KINECT
Kinect::Kinect()
{
	this->InitializeSensor(0);
}

Kinect::Kinect(int index)
{
	this->InitializeSensor(index);
}

void Kinect::InitializeSensor(int index)
{
	int totalSensors = 0;
	this->status = NotDetected;

	if(NuiGetSensorCount(&totalSensors) > -1 || totalSensors > index)
	{
		if(NuiCreateSensorByIndex(index, &this->sensor) > -1)
		{
			this->sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
			this->status = Ready;
		}
		else if(this->sensor == NULL)
		{
			this->status = NotEnoughBandwidth;
		}
	}
}



#endif


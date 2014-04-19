#include "pv/Kinect1.h"

namespace PV
{
#ifdef _WIN32
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
		if (this->status != Ready)
		{
			int totalSensors = 0;
			KinectStatus status = this->status = NotDetected;

			if (NuiGetSensorCount(&totalSensors) > -1 || totalSensors > index)
			{
				if (NuiCreateSensorByIndex(index, &this->sensor) > -1)
				{
					this->nextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
					this->sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);
					this->sensor->NuiSkeletonTrackingEnable(this->nextSkeletonEvent, 0);
					status = this->status = Ready;
				}
				else if (this->sensor == NULL)
				{
					status = this->status = NotEnoughBandwidth;
				}
			}
			return status;
		}
		return Ready;
	}

	const KinectStatus Kinect1::Status() const
	{
		return this->status;
	}
	
	const NUI_SKELETON_DATA Kinect1::getMainPerson()
	{
		if (this->status == Ready)
		{
			NUI_SKELETON_FRAME frame;
			this->sensor->NuiSkeletonGetNextFrame(0, &frame);

			const NUI_TRANSFORM_SMOOTH_PARAMETERS VerySmoothParams = { 0.7f, 0.3f, 1.0f, 1.0f, 1.0f };
			this->sensor->NuiTransformSmooth(&frame, &VerySmoothParams);

			for (int i = 0; i < NUI_SKELETON_COUNT; i++)
			{
				this->mainSkeleton = frame.SkeletonData[i];

				if (this->mainSkeleton.eTrackingState == NUI_SKELETON_TRACKING_STATE::NUI_SKELETON_TRACKED)
				{
					return this->mainSkeleton;
				}
			}
		}
	}
#endif

#endif
};


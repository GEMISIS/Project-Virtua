#ifndef _DRAWABLE_OBJECT_H_
#define _DRAWABLE_OBJECT_H_

#include <stdio.h>

namespace PV
{
	class drawableObject
	{
	public:
		virtual void loadGraphics(const char* fileName)
		{
		};
		virtual void draw()
		{
		};
	protected:
	private:
	};
}

#endif


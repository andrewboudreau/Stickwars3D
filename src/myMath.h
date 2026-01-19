#ifndef MYMATH_H
#define MYMATH_H

#include <vector>
#include <iostream>
#include <math.h>
#include <gfx/vec3.h>
#include <fstream>
#include <gfx/gltools.h> 
#include <time.h>

class myMath
{
public:
	float getAngle(float s_x, float s_z, float d_x, float d_z );
	float distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z );

};


#endif
#include "myMath.h"

float myMath::getAngle(float s_x, float s_z, float d_x, float d_z )
{
	float x, z, dist;
	float angle;
	x=( d_x - s_x);
	z=( d_z - s_z);
	angle=atan( z / x )*180.0f/M_PI;
	dist=sqrt(x*x+z*z);
	x/=dist;
	z/=dist;
				
	if(x<0.0)//quads 3 and 4 need to flip
		angle=angle+180.0f;
	return angle;
}

float myMath::distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z )
{
	float x_comp, y_comp;
	x_comp=pointA_X-pointB_X;
	y_comp=pointA_Z-pointB_Z;
	return sqrt(x_comp*x_comp + y_comp*y_comp);
}


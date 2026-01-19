#ifndef SHADOW_H
#define SHADOW_H

#include <vector>
#include <iostream>
#include <math.h>
#include <gfx/vec3.h>
#include <fstream>
#include <gfx/gltools.h>
#include <gfx/script.h>
#include "myMesh.h"
 
#define M_INFINITY 10000000.0f

using namespace std;



class shadow 
{
public:

	GLfloat shadowMatrix[16];
	GLfloat plane[4];

	GLuint shadowDL;

	shadow();
	//void render_shadow(vector< TVec3<float> > vert, TVec3<float> light_pos);
	void render_shadow();
	void draw_shadow();
	void build_shadow_volume(myMesh* mesh, TVec3<float> light_pos, TVec3<float> pos );
	void extendVertex( TVec3<float> &newVert, TVec3<float> lightPos, TVec3<float> vert);
	float getAngle(float s_x, float s_z, float d_x, float d_z );
	float distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z );
	void sDraw(myMesh* mesh, TVec3<float> light_pos, TVec3<float> pos);
	void dMap();

	void findPlane( TVec3<float> v0, TVec3<float> v1, TVec3<float> v2 );
	void buildShadowMatrix(  GLfloat lightPos[4]);
	void projectShadow(TVec3<float> pos, myMesh* mesh);
};

#endif
#ifndef ROBOTMESH_H
#define ROBOTMESH_H

// Include the standard libgfx headers that we need
#include <gfx/gui.h>         // MxGUI framework
#include <gfx/gltools.h>     // Include the OpenGL headers + handy utilities

#include <gfx/raster.h>      // Support for reading/writing image files
#include <time.h>
#include <gfx/quat.h>
#include <gfx/mat4.h>
#include <vector>
#include <fstream>
#include "mesh.h"

using namespace std;

// Structure used to hold each frame's rotation and translation values for our object.
typedef struct robotKeyFrame
{
   float time;				// Start time of this keyframe.
   int currentFrame;		//Tracking CurrentFrame
  
   //UpperBody
   Quat r_shoulder_rot;       
   Quat l_shoulder_rot;
   Quat r_elbow_rot;
   Quat l_elbow_rot;
   
   //LowerBody
   Quat r_hip_rot;
   Quat l_hip_rot;
   Quat r_knee_rot;
   Quat l_knee_rot;

   //Torso
   Quat u_torso_rot;
   Quat l_torso_rot;

   Vec3f torso_tran;

   //Body Angle
   Quat _body;

} robotKeyFrame;


class robotMesh{

public:
	bool built;
	Mesh hand;
	Mesh foot;
	Mesh forearm;
	Mesh bicep;
	Mesh thigh;
	Mesh calf;
	Mesh body;
	Mesh sword;

	vector< vector <robotKeyFrame> > KF;

	robotMesh();

	void DL();
	void import_keyframes_ascii(char * filename, int index);




};

#endif
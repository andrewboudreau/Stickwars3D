#ifndef ROBOT_H
#define ROBOT_H

// Include the standard libgfx headers that we need
#include <gfx/gui.h>         // MxGUI framework
#include <gfx/gltools.h>     // Include the OpenGL headers + handy utilities

#include <gfx/raster.h>      // Support for reading/writing image files
#include <time.h>
#include <gfx/quat.h>
#include <gfx/mat4.h>
#include <vector>
#include <fstream>
//#include "mesh.h"
#include "robotMesh.h"


using namespace std;

/*
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
*/



class Robot 
{
public:
	// Here we list the variables that our program uses.
    float center[2];

    float time;		//Animation Current Time
	bool play;		//Play Mode
	bool edit;		//Edit Mode
	bool loop;		//If playing loop animation.
    bool reverse;	//Loop forwards and backwards.??
	float scale;
	bool sword_out;


	///////////////////////////
	// Robot Angles/Animation
	///////////////////////////
	int currentFrame;
	
	//vector<int> keyframeTime;
	vector< robotKeyFrame > keyFrames;

	//Body
	Quat u_torso;
	Quat l_torso;
	Vec3f torso_tran;

	/////////////////	
	//Right Side
	////////////////
	//arm
	Quat r_elbow;
	Quat r_shoulder;
	Quat r_shoulderYaw;
	Quat r_shoulderTwist;
	//leg
	Quat r_knee;
	Quat r_hip;
	Quat r_hipYaw;
	Quat r_hipTwist;

	/////////////////
	//Left Side
	/////////////////
	//arm
	Quat l_elbow;
	Quat l_shoulder;
	Quat l_shoulderYaw;
	Quat l_shoulderTwist;
	//leg
	Quat l_knee;
	Quat l_hip;
	Quat l_hipYaw;
	Quat l_hipTwist;

	//Body Angle
    Quat _bodyTwist;
    Quat _bodyLean;
    Quat _bodyTilt;

	//Robot Size
	float _bodyScale;

	//A few material settings to make the robot more interesting

	GLfloat lightskin_diffuse[4];
	GLfloat darkskin_diffuse[4]; 
	GLfloat clothing_diffuse[4];

public:
    // Constructor and destructor definitions.
    Robot();
    ~Robot();


	void save_keyframe(float time);
	void clear_keyframes();
	void play_keyframes();
	
	void export_keyframes(char * file);
	void export_keyframes_ascii(char * file);
	void import_keyframes(char * file);
	void import_keyframes_ascii(char * file);
	void import_keyframes_robotMesh(robotMesh* robot, int index);
	void print_keyframes(ofstream& out);
	void print_keyframes();

	Vec3f interp(Vec3f &a, Vec3f &b, float s);

public:
    void draw(robotMesh* robot);
    void update();
	void Robot::toggle_sword() {sword_out = !sword_out; };

//Private Member Functions
private:
	void draw_sphere(float radius);
	void draw_forearm(Quat& q, float width, float length, robotMesh* robot);
	void draw_bicep(Quat& q,  float width, float length, robotMesh* robot);
	void draw_body(float scale, robotMesh* robot);

	//Private Data

	GLUquadricObj *quadratic;					// Storage For Our Quadratic Objects ( NEW )
	
	/*Mesh hand;
	Mesh foot;
	Mesh forearm;
	Mesh bicep;
	Mesh thigh;
	Mesh calf;
	Mesh body;
	bool built;*/

};

#endif //ROBOT_H


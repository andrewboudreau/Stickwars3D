#ifndef AI_H
#define AI_H

#include <vector>
#include <iostream>
#include <math.h>
#include <gfx/vec3.h>
#include <fstream>
#include <gfx/gltools.h> 
#include <time.h>

#include "myMesh.h"
#include "robot.h"
#include "heightMap.h"
#include "shadow.h"
#include "robotMesh.h"


struct keyFrame{	//keyFrames
	bool on;
	int frame;
	int count;
	TVec3<float> angle;
	TVec3<float> translate;
};

struct boundingCylinder{
	TVec3<float> pos;
	float yMin, yMax, radius;
};


	
class AI 
{

public:

	boundingCylinder bound;
	
	Robot rbot;

	int killCount;

	float hitRest;
	int target;
	float targetDistance;
	float health;
	int aiType;  //0=attacker
				 //1=defender

	float walkSpeed;

	float heightOffset;

	TVec3 <float> pos;
	TVec3 <float> angle;
	TVec3 <float> scale;
	TVec3 <float> destination;

	TVec3 <float> aVel;
	TVec3 <float> velocity;
	float max_height;
	float energy;


	int type;		

	int frame;



	int team;	//team number (1 or 2)
	int goalFlag; //current flag to reach

	bool walking; //in motion

	bool dead;		//is it dead

	bool dying;		//do we need its death animation 

	keyFrame death;	//death animation

	AI();
	~AI();


	void setGoal(heightMap *hmap);
	void move();
	void move(heightMap *hmap);
	void deathAnimation(heightMap* hmap);

	float distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z );

	void draw(myMesh* cow);
	void draw(robotMesh* robot);
	void draw();

	void drawShadow(myMesh* cow, TVec3<float> light_pos);

	float getAngle(float s_x, float s_z, float d_x, float d_z );

	float getPosX(){return pos[0];};
	float getPosY(){return pos[1];};
	float getPosZ(){return pos[2];};

	void pathFind(heightMap *hmap);

	void threat(AI* enemy, int x, robotMesh* robot);
	void fight(AI* enemy, robotMesh* robot, int TEAM_KILLS[2]);
	void respawn(robotMesh* robot, heightMap* hmap);
	bool explode(TVec3 <float> missle, float radius, heightMap* hmap);
	bool explode(TVec3 <float> missle, float radius, heightMap* hmap, robotMesh* robot);
	bool rayBoundIntersection(TVec3<float> point, TVec3<float> dir);

};

#endif
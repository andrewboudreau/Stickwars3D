#include <vector>



#include "AI.h"


AI::AI()
{

	//r=new Robot;
	rbot.import_keyframes_ascii("running.kfm");
	rbot.play_keyframes();

	hitRest=0.0f;

	aiType=0;		//attacker
	health=100.0f;
	killCount=0;

	team=1;
	goalFlag=0;

	targetDistance=100.0f;
	target=-1;

	heightOffset=0.0f;
	walkSpeed=1.5f;

//	 srand ( time(NULL) );	//seed random
	 frame=0;
	 type=0;
	dead=true;
	dying=false;
	aiType=0;		//attacker
	death.frame=0;
	death.on=false;

	
		
	int y;
	for(y=0; y<3; y++)//initialize X Y Z positions/angles/scales
	{		
		death.angle[y]=0.0;
		death.translate[y]=0.0;
		//pos[y]=(float)(rand()%100);
		angle[y]=0.0f;
		scale[y]=1.0f;

		destination[y]=0.0;

	}
	bound.pos=pos;
	bound.yMax=10.0f;
	bound.yMin=0.0f;
	bound.radius=5.0f;
	walking=false;
}

AI::~AI()
{
	//delete r;
	//r=NULL;
}




void AI::move(heightMap *hmap)
{
	int i;
	hitRest-=1.0f;

	// set in bounds
	if(pos[0]<0)
		pos[0]=0;
	if(pos[2]<0)
		pos[2]=0;
	if(pos[0]>hmap->getWidth()-hmap->scale)
		pos[0]=hmap->getWidth()-hmap->scale;
	if(pos[2]>hmap->getLength()-hmap->scale)
		pos[2]=hmap->getLength()-hmap->scale;
	if( !(dying&&type==1) )
		pos[1]=hmap->getHeight(pos[0], pos[2])+heightOffset;
	
	if(!dead&&!dying)
	{
		//check to capture flags
		for(i=0; i<hmap->_flag.size(); i++)
		{
			if(distance(pos[0], pos[2], 
				hmap->_flag[i].pos[0], hmap->_flag[i].pos[2])
				<50.0f)
				hmap->_flag[i].teamCap[team-1]++;//count 
		}

		//look for a new goal
		if(target<0)
			setGoal(hmap);

		//movement
		pathFind(hmap);
		//animate
		rbot.update();
	}
	if(dying)
		deathAnimation(hmap);
}

void AI::deathAnimation(heightMap* hmap)
{
	TVec3<float> dir;//temp vars for calculations
	TVec3<float> angle;	
	
	bool flag=false;
	
	int i;

	switch(type)
	{
		case 0: //fall over dead
		default:
			switch( death.frame)
			{
				case 0:
					 death.angle[0]+=3.0;
					 //death.translate[1]-=1.0f/6.0f;
					if( death.angle[0]>=90.0)
						 death.frame++;
					break;
				default:
					 death.frame=0;
					 dying=false;
					break;
			}
			break;
		case 1://blow up
			switch(death.frame)
			{
				case 0:
					rbot.update();
					pos+=velocity;
					velocity[1]-=0.098f;

					death.angle+=aVel;

					if(velocity[1]<=0)
						death.frame++;
					
					if( pos[2] > hmap->getWidth()-hmap->scale  || pos[2] <= 0)
						velocity[2]=-velocity[2];
					if( pos[0] > hmap->getWidth()-hmap->scale  || pos[0] <= 0)
						velocity[0]=-velocity[0];
					break;
				case 1:
					rbot.update();
					pos+=velocity;
					velocity[1]-=0.098f;

					death.angle+=aVel;

					if(pos[1] <=
						hmap->getHeight(pos[0], pos[2]) )
					{
						pos[1] = hmap->getHeight(pos[0], pos[2]) + 0.1f;
						death.frame++;
					}
					

					if( pos[2] > hmap->getWidth()-hmap->scale  || pos[2] <= 0)
						velocity[2]=-velocity[2];
					if( pos[0] > hmap->getWidth()-hmap->scale || pos[0] <= 0)
						velocity[0]=-velocity[0];


					break;
				case 2:
					
					velocity[1]/=3.0f;
					velocity[1] = -velocity[1];

					if(velocity[1]<=0.5f)
						death.frame=3;
					else
						death.frame=0;
					break;
				case 3:
					
					pos[1]=hmap->getHeight(pos[0], pos[2]);

					for(i=0; i<3; i++)
					{
						if(death.angle[i] > 100.0 && death.angle[i] < 180.0 
							|| death.angle[i] > 280.0 && death.angle[i] < 360.0 )
						{
							death.angle[i]-=aVel[i];
							flag=true;
						}
						if(death.angle[i] >= 180.0 && death.angle[i] < 260.0
							|| death.angle[i] > 0.0 && death.angle[i] < 80.0 )
						{
							death.angle[i]+=aVel[i];
							flag=true;
						}
					}
					
					if(!flag)
						death.frame++;
					break;
				case 4:
				default:
					pos[1]=hmap->getHeight(pos[0], pos[2]);
					dying=false;
					death.angle[1]=0.0f;
					death.frame=0;
					break;
			}

	}
	if( dying==false)
	{
		 frame=0;
		 walking=false;
		 dead=true;
	}
}



float AI::distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z )
{
	float x_comp, y_comp;
	x_comp=pointA_X-pointB_X;
	y_comp=pointA_Z-pointB_Z;
	return sqrt(x_comp*x_comp + y_comp*y_comp);
}



void AI::draw(myMesh* cow)
{
		//glStencilFunc( GL_EQUAL, 0, 1 );
		//glEnable( GL_LIGHTING );
		glPushMatrix();
		glTranslatef( pos[0],  pos[1],  pos[2]);
		glRotatef( angle[1], 0.0, 1.0, 0.0);
		glRotatef( angle[0], 1.0, 0.0, 0.0);
		glRotatef( angle[2], 0.0, 0.0, 1.0);
		glScalef( scale[0],  scale[1],  scale[2]);
		
		/*if( dying || dead)
		{
			glTranslatef( death.translate[0],  death.translate[1],  death.translate[2]);
			glRotatef( death.angle[1], 0.0, 1.0, 0.0);
			glRotatef( death.angle[0], 1.0, 0.0, 0.0);
			glRotatef( death.angle[2], 0.0, 0.0, 1.0);
		}*/

		//glRotatef(90.0f, 0.0, 1.0f, 0.0f);

		//r->draw();
		//glRotatef(-90.0f, 0.0, 1.0f, 0.0f);



		glCallList(cow->DL);
				
		glPopMatrix();
}

void AI::draw()
{
		//glStencilFunc( GL_EQUAL, 0, 1 );
		glDisable( GL_LIGHTING );
		glPushMatrix();
		glTranslatef( pos[0],  pos[1],  pos[2]);
		glRotatef( angle[1], 0.0, 1.0, 0.0);
		glRotatef( angle[0], 1.0, 0.0, 0.0);
		glRotatef( angle[2], 0.0, 0.0, 1.0);
		glScalef( scale[0],  scale[1],  scale[2]);
		
		if( dying|| dead)
		{
			glTranslatef( death.translate[0],  death.translate[1],  death.translate[2]);
			glRotatef( death.angle[1], 0.0, 1.0, 0.0);
			glRotatef( death.angle[0], 1.0, 0.0, 0.0);
			glRotatef( death.angle[2], 0.0, 0.0, 1.0);
		}

		switch(team)
		{
			case 1:
				glColor3f(1.0, 0.0, 0.0);
				break;
			case 2:
				glColor3f(0.0, 0.0, 1.0);
				break;
			default:
				glColor3f(0.5, 0.5, 0.5);
				break;
		}

		glRotatef(90.0f, 0.0, 1.0f, 0.0f);

		//r->draw();
		glRotatef(-90.0f, 0.0, 1.0f, 0.0f);



		//glCallList(cow->DL);
				
		glPopMatrix();
		glEnable(GL_LIGHTING);
}


void AI::draw(robotMesh* robot)
{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
		//glStencilFunc( GL_EQUAL, 0, 1 );
		//glDisable( GL_LIGHTING );
		glPushMatrix();
		glTranslatef( pos[0],  pos[1],  pos[2]);
		glRotatef( angle[1], 0.0, 1.0, 0.0);
		glRotatef( angle[0], 1.0, 0.0, 0.0);
		glRotatef( angle[2], 0.0, 0.0, 1.0);
		glScalef( scale[0],  scale[1],  scale[2]);
		
		if( dying|| dead)
		{
			glTranslatef( death.translate[0],  death.translate[1],  death.translate[2]);
			glRotatef( death.angle[0], 1.0, 0.0, 0.0);
			glRotatef( death.angle[2], 0.0, 0.0, 1.0);
			glRotatef( death.angle[1], 0.0, 1.0, 0.0);
		}

		switch(team)
		{
			case 1:
				glColor3f(1.0, 0.0, 0.0);
				break;
			case 2:
				glColor3f(0.0, 0.0, 1.0);
				break;
			default:
				glColor3f(0.5, 0.5, 0.5);
				break;
		}

		glRotatef(90.0f, 0.0, 1.0f, 0.0f);

		rbot.draw(robot);
		glRotatef(-90.0f, 0.0, 1.0f, 0.0f);



		//glCallList(cow->DL);
				
		glPopMatrix();
		glEnable(GL_LIGHTING);
}


void AI::drawShadow(myMesh* cow, TVec3<float> light_pos)
{
		
		
		glPushMatrix();
		glTranslatef( pos[0],  pos[1],  pos[2]);
		glRotatef( angle[1], 0.0, 1.0, 0.0);
		glRotatef( angle[0], 1.0, 0.0, 0.0);
		glRotatef( angle[2], 0.0, 0.0, 1.0);
		glScalef( scale[0],  scale[1],  scale[2]);
		
		if( dying|| dead)
		{
			glTranslatef( death.translate[0],  death.translate[1],  death.translate[2]);
			glRotatef( death.angle[1], 0.0, 1.0, 0.0);
			glRotatef( death.angle[0], 1.0, 0.0, 0.0);
			glRotatef( death.angle[2], 0.0, 0.0, 1.0);
		}

		//glRotatef(90.0f, 0.0, 1.0f, 0.0f);

		//r->draw();
		//glRotatef(-90.0f, 0.0, 1.0f, 0.0f);*/
		TVec3<float> new_light_pos;
		float _distance=distance(light_pos[0], light_pos[2], pos[0], pos[2]);
		
		//new_light_pos[0]=pos[0] - cos( (angle[1] + death.angle[1])*M_PI/180.0f )*_distance;
		//new_light_pos[2]=pos[2] - sin( (angle[1] + death.angle[1])*M_PI/180.0f )*_distance;

		new_light_pos[0]=- cos( (angle[1] + death.angle[1])*M_PI/180.0f )*_distance;
		new_light_pos[2]=- sin( (angle[1] + death.angle[1])*M_PI/180.0f )*_distance;
		new_light_pos[1]=light_pos[1];//-cos((angle[1]+death.angle[1])*M_PI/180.0f)*sqrt(_distance*_distance + (pos[1]-light_pos[1])*(pos[1]-light_pos[1]) );
		
		
		//build_shadow_volume(cow, new_light_pos, pos);
		
		
		//glStencilFunc( GL_EQUAL, 1, 1 );
		//glDisable( GL_LIGHTING );
		//gluLookAt(light_pos[0], light_pos[1], light_pos[2], pos[0], pos[1], pos[2], 0.0, 1.0, 0.0);
		//render_shadow();

		glPopMatrix();
}

float AI::getAngle(float s_x, float s_z, float d_x, float d_z )
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



void AI::setGoal(heightMap *hmap)
{
	int i;
	float closest=100000000.0f;
	float furthest=0.0f;
	float dist;
	switch(aiType)
	{
		default:
		case 0://attacker goes to closest enemy or uncaptured flag
		case 2://attacker turns defender
			if(aiType==2 
			&& hmap->_flag[goalFlag].team==team 
			&& distance(pos[0], pos[2], destination[0], destination[2]) < 50.0f)
					aiType=1;
			else //go for closest flag
			{
				for(i=0; i<hmap->_flag.size(); i++)
				{
					if( hmap->_flag[i].team!=team)
					{
						dist=distance(pos[0], pos[2], 
							hmap->_flag[i].pos[0], hmap->_flag[i].pos[2]);
						if(dist<closest)
						{
							closest=dist;
							goalFlag=i;
						}
					}
				}
			}
			break;
		case 1://defender goes to closest team flag
			for(i=0; i<hmap->_flag.size(); i++)
			{
				if( hmap->_flag[i].team==team)
				{
					dist=distance(pos[0], pos[2], 
						hmap->_flag[i].pos[0], hmap->_flag[i].pos[2]);
					if(dist<closest)
					{
						closest=dist;
						goalFlag=i;
					}
				}
			}
			break;
	}
	if(goalFlag>=0)
	{
		destination[0]=hmap->_flag[goalFlag].pos[0];
		destination[2]=hmap->_flag[goalFlag].pos[2];
	}

}
			

void AI::pathFind(heightMap *hmap)
{
	
	float i;
	float next_x, next_y, temp_x_dir, temp_y_dir;
	float slope, best, next;
	float speed=walkSpeed;

	float bestAngle=-angle[1];

	float tAngle;

	//default is go nowhere
	
	best=1000000000.0f;
	next_x=pos[0];
	next_y=pos[2];

	bool flag=false;

	for(i=-10.0f; i<=10.0f; i+=1.0)
	{
		//to the left(?)
		tAngle=-angle[1]+i;
		
		temp_x_dir=cos(tAngle*M_PI/180.0f);
		temp_y_dir=sin(tAngle*M_PI/180.0f);
		
		slope=( pos[1] - heightOffset - hmap->getHeight( pos[0] 
			+ temp_x_dir, pos[2] + temp_y_dir) );

		speed += slope/4.0;
		

		if(speed<0.0&&speed>-10.0)
			speed=0.05f;
		if(speed<0.0)
			speed=0.0f;
		if(speed>3.0)
			speed=3.0f;

		temp_x_dir *= speed;
		temp_y_dir *= speed;
	
		next=distance(pos[0] + temp_x_dir, pos[2] + temp_y_dir,
			destination[0], destination[2]);
		
		if( next < best && speed>0.0 || !flag)
		{
			flag=true;
			bestAngle=tAngle;
			best=next;
			next_x=pos[0]+temp_x_dir;
			next_y=pos[2]+temp_y_dir;
		}
	}		

	
	if(angle[1]==-bestAngle && pos[0]==next_x && pos[2]==next_y)
	{
		if( -angle[1] < 180.0)
			angle[1]-=10.0f;
		else
			angle[1]+=10.0f;
	}
	else
		angle[1]=-bestAngle;
	if(distance(pos[0], pos[2], destination[0], destination[2]) > 10.0f)
	{
		pos[0]=next_x;
		pos[2]=next_y;
	}
}

void AI::threat(AI* enemy, int x, robotMesh* robot)
{	
	float dist=distance(pos[0], pos[2], enemy->pos[0], enemy->pos[2]);
	
	if(!enemy->dead && !enemy->dying && dist < targetDistance) 
	{
		destination[0]=enemy->pos[0];
		destination[2]=enemy->pos[2];
		target=x;
		targetDistance=dist;
		switch(rand()%3)
		{
			case 0:
				rbot.import_keyframes_robotMesh(robot, 1);
				break;
			case 1:
				rbot.import_keyframes_robotMesh(robot, 2);
				break;
			case 2:
				rbot.import_keyframes_robotMesh(robot, 5);
				break;
			default:
				rbot.import_keyframes_robotMesh(robot, 1);
				break;
		}
		rbot.play_keyframes();
	}
}

void AI::fight(AI* enemy, robotMesh* robot, int TEAM_KILLS[2])
{
	float dist=distance(pos[0], pos[2], enemy->pos[0], enemy->pos[2]);
	if(enemy->health<=0.0||enemy->dead)
	{
		if(!enemy->dead&&!enemy->dying)		//this is when robot kills someone
		{	
			enemy->dying=true;
			enemy->death.frame=0;
			enemy->type=0;
			killCount++;
			TEAM_KILLS[team-1]++;
		}
		target=-1;
		targetDistance=100.0f;
		
		rbot.import_keyframes_robotMesh(robot, 0);
		rbot.play_keyframes();
	}
	else
	{
		destination[0]=enemy->pos[0];
		destination[2]=enemy->pos[2];
		rbot.update();
		if(hitRest<=0.0f)
		{
			hitRest=15.0f;
			if(dist<=50.0f)
				enemy->health-=(float)(rand()%100)/dist;
		}
	}
	
}

void AI::respawn(robotMesh* robot, heightMap* hmap)
{
	int y;
	for(y=0; y<3; y++)
	{
		death.angle[y]=0.0;
		death.translate[y]=0.0;	
		angle[y]=0.0f;
	}
	if(aiType==1)
		aiType=2;
	dead=false;
	target=-1;
	health=100.0f;
	dying=false;
	switch(team)
	{
		case 1:
			pos[0]=(float)(rand()%(int)( hmap->getWidth() ) );
			pos[2]=10.0f;
			break;
		case 2:
			pos[0]=(float)(rand()%(int)( hmap->getWidth() ) );
			pos[2]=hmap->getWidth()-10.0f;
			break;
		default:
			break;
	}
	rbot.import_keyframes_robotMesh(robot, 0);
	rbot.play_keyframes();
}

bool AI::explode(TVec3 <float> missle, float radius, heightMap* hmap)
{
	float dist=distance(pos[0], pos[2], missle[0], missle[2]);
	if(dist>radius)
		return false;

	float power=(radius-dist) / hmap->scale;
	dist /= hmap->scale;

	float tAngle = getAngle(missle[0], missle[2], pos[0], pos[2]);

	//bound distance to limit velocities


	type=1;

	aVel[0]=-sin(tAngle * M_PI / 180.0f) * power;
	aVel[1]=10.0;
	aVel[2]=-cos(tAngle * M_PI / 180.0f) * power;

	
	velocity[0]=cos(tAngle * M_PI / 180.0f) * power;
	velocity[1]=power;
	velocity[2]=sin(tAngle * M_PI / 180.0f) * power;

	dying=true;
	death.frame=0;
	health-=101.0f;
	
	return true;
}

bool AI::explode(TVec3 <float> missle, float radius, heightMap* hmap,robotMesh* robot)
{
	float dist=distance(pos[0], pos[2], missle[0], missle[2]);
	if(dist>radius)
		return false;

	float power=(radius-dist) / hmap->scale;
	dist /= hmap->scale;

	float tAngle = getAngle(missle[0], missle[2], pos[0], pos[2]);

	//bound distance to limit velocities


	type=1;

	aVel[0]=-sin(tAngle * M_PI / 180.0f) * power+1.0f;
	aVel[1]=2.0f*power+1.0f;
	aVel[2]=-cos(tAngle * M_PI / 180.0f) * power+1.0f;

	
	velocity[0]=cos(tAngle * M_PI / 180.0f) * power;
	velocity[1]=power;
	velocity[2]=sin(tAngle * M_PI / 180.0f) * power;

	dying=true;
	death.frame=0;
	health-=101.0f;
	rbot.import_keyframes_robotMesh(robot, 3);
	rbot.play_keyframes();
	return true;
}
/*
bool AI:: rayBoundIntersection(TVec3<float> point, TVec3<float> dir)
{
	float a=dir[0]*dir[0] + dir[2]*dir[2];
	float b=2*(point[0]*dir[0]+point[2]*dir[2]);
	float c=point[0]*point[0]+point[2]*point[2]-bound.radius*bound.radius);
	float rad=b*b-4*a*c;
	
	//doesnt intersect
	if(rad<0.0f)
		return false;
	
	float t_1=( -b+sqrt(b*b-4*a*c) )/(2*a);
	
	//two points
	if(rad>0.0f)
		float t_2=( -b-sqrt(b*b-4*a*c) )/(2*a);

	float t=(t_1+t_2)/2.0f;

	point[1]+=dir[1]*t;
	if(point[1] < yMax && point[1]>=yMin)
	{
		return true;
	}
}*/
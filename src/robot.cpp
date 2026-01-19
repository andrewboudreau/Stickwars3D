#include "robot.h"
using namespace std;


//Dead Robot
Robot::Robot()
{
	keyFrames.resize(0);
	time = 0.0f;
	currentFrame = 0;
	play = false;
	edit = false;
	loop = false;
	reverse = false;
	sword_out = true;
	currentFrame = 0;
	scale=1.0f;

	/*
	bicep.loadObj("bicep.obj");
	forearm.loadObj("forearm.obj");
	hand.loadObj("hand.obj");
	body.loadObj("torso.obj");
	foot.loadObj("foot.obj");
	calf.loadObj("calf.obj");
	thigh.loadObj("thigh.obj");
	built = false;
	*/
}

Robot::~Robot(){}

void Robot::save_keyframe(float time)
{
	
	robotKeyFrame k;
	if(play){
		cout << "Keyframe Not saved Cause animation was playing, Save again" << endl;
		play = false;
		return;
	}

	//Right Keyframe
	k.time = time;
	k.r_shoulder_rot = r_shoulder*r_shoulderYaw*r_shoulderTwist;
	k.r_elbow_rot = r_elbow;
	
	//Left Keyframe
	k.l_shoulder_rot = l_shoulder*l_shoulderYaw*l_shoulderTwist;
	k.l_elbow_rot = l_elbow;

	//Right Lower Body Keyframe
	k.r_hip_rot = r_hip*r_hipYaw*r_hipTwist;
	k.r_knee_rot = r_knee;

	//Left Lower Body Keyframe
	k.l_hip_rot = l_hip*l_hipYaw*l_hipTwist;
	k.l_knee_rot = l_knee;

	//Torso Stuff
	k.u_torso_rot = u_torso;
	k.l_torso_rot = l_torso;
	k.torso_tran = torso_tran;

	k._body = _bodyTwist*_bodyLean*_bodyTilt;
	
	//Save the keyframes
	keyFrames.push_back(k);

	std::cout << "Keyframe Saved keyframes.size()=" << keyFrames.size();
	std::cout << " Time = " << time << endl;

}

void Robot::clear_keyframes()
{
	play = false;
	keyFrames.resize(0);

	cout << "Keyframe cleared" << endl;
	
}

void Robot::play_keyframes()
{
	play= true;
	currentFrame = 0;
	time = 0;

}

void Robot::print_keyframes(ofstream& out)
{
	out << keyFrames.size()	<< endl;
	for(int i = 0 ; i < keyFrames.size() ; i++)
	{
		
		out << keyFrames[i].time				<< " ";
		out << keyFrames[i].r_shoulder_rot		<< " ";
		out << keyFrames[i].r_elbow_rot			<< " ";
		out << keyFrames[i].l_shoulder_rot		<< " ";
		out << keyFrames[i].l_elbow_rot			<< " ";
		out << keyFrames[i].r_hip_rot			<< " ";
		out << keyFrames[i].r_knee_rot			<< " ";
		out << keyFrames[i].l_hip_rot			<< " ";
		out << keyFrames[i].l_knee_rot			<< " ";
		out << keyFrames[i].u_torso_rot			<< " ";
		out << keyFrames[i].l_torso_rot			<< " ";
		out << keyFrames[i].torso_tran			<< " "; 
		out << keyFrames[i]._body				<<"\n";
	}
}
void Robot::print_keyframes()
{
	for(int i = 0 ; i < keyFrames.size() ; i++)
	{
		cout << keyFrames[i].time				<< " ";
		cout << keyFrames[i].r_shoulder_rot		<< " ";
		cout << keyFrames[i].r_elbow_rot		<< " ";
		cout << keyFrames[i].l_shoulder_rot		<< " ";
		cout << keyFrames[i].l_elbow_rot		<< " ";
		cout << keyFrames[i].r_hip_rot			<< " ";
		cout << keyFrames[i].r_knee_rot			<< " ";
		cout << keyFrames[i].l_hip_rot			<< " ";
		cout << keyFrames[i].l_knee_rot			<< " ";
		cout << keyFrames[i].u_torso_rot		<< " ";
		cout << keyFrames[i].l_torso_rot		<< " ";
		cout << keyFrames[i].torso_tran			<< " "; 
		cout << keyFrames[i]._body				<<"\n";
	}
}

void Robot::export_keyframes_ascii(char * filename){
	//Binary files are much easier with structs and arrays.
	ofstream file (filename);
	int size = keyFrames.size();
	
	print_keyframes(file);
	file.close();
	cout << "Keyframes Saved (" <<keyFrames.size() << "): " << filename << endl;


}

void Robot::import_keyframes_robotMesh(robotMesh* robot, int index)
{
	int i;
	if(index>robot->KF.size())
	{
		cout<<"Index out of bounds"<<endl;
		return;
	}
	keyFrames.resize(0);
	for(i=0; i<robot->KF[index].size(); i++)
		keyFrames.push_back( robot->KF[index][i] );
}


void Robot::import_keyframes_ascii(char * filename){
	
	robotKeyFrame k;
	keyFrames.resize(0);

	ifstream file (filename);
	int num,i;
	char temp[512];

	file >> num;
	file.getline(temp,512);

	for(i = 0 ; i < num ; i++){
		file >> k.time;
		file >> k.r_shoulder_rot;
		file >> k.r_elbow_rot;
		file >> k.l_shoulder_rot;
		file >> k.l_elbow_rot;
		file >> k.r_hip_rot;	
		file >> k.r_knee_rot;	
		file >> k.l_hip_rot;		
		file >> k.l_knee_rot;	
		file >> k.u_torso_rot;
		file >> k.l_torso_rot;
		file >> k.torso_tran[0] >> k.torso_tran[1] >> k.torso_tran[2] ;
		file >> k._body;
		file.getline(temp,512);

		keyFrames.push_back(k);
	}
	//	cout << "keyframes loaded " << keyFrames.size() << endl;
	//	print_keyframes();
	file.close();

}
void Robot::export_keyframes(char * filename)
{
	//Binary files are much easier with structs and arrays.
	fstream file (filename, ios::out | ios::binary);
	int size = keyFrames.size();
	
	//Write Keyframe size
	file.write ((char*)(&size), sizeof(size));
	
	//Write out keyframes
	for(int i = 0; i <= keyFrames.size(); i++)
		file.write ((char*)(&keyFrames[i]), sizeof(keyFrames[i]));

	file.close();
	cout << "Keyframes Saved (" <<keyFrames.size() << "): " << filename << endl;
}

void Robot::import_keyframes(char * filename)
{
	// read in the keyframes
	robotKeyFrame temp_keyframe;
	
	int size;
	ifstream fin(filename, ios::in | ios::binary);

	//Read in keyframe size
	fin.read((char *)(&size), sizeof(size));
	
	//Read in keyframes
	keyFrames.resize(size);
	for(int i = 0 ; i < size; i++)
		fin.read((char *)(&keyFrames[i]), sizeof(keyFrames[i]));
	
	fin.close();
	cout << "Keyframes Loaded (" << keyFrames.size()  << "): " << filename << endl;

}

void Robot::update()
{
	if(!play || keyFrames.empty())
		return;

	time++;
	//cout <<" Time = " << time << endl;
	//Increase Frame
	if(time >= keyFrames[currentFrame+1].time && currentFrame<keyFrames.size()-2)
		currentFrame++;

	//Restart Animation
	if(time >= keyFrames[keyFrames.size()-1].time )
	{
		time = 0;
		currentFrame=0;
	}

	//cout << "Time = "<< time << " CurrentFrame = " << currentFrame << endl;
}
void Robot::draw(robotMesh* robot)
{

	/*//Build the display lists on the first render.
	if(!built){
		bicep.createDL();
		forearm.createDL();
		hand.createDL();
		body.createDL();
		foot.createDL();
		calf.createDL();
		thigh.createDL();
		built = true;
	}*/
	
	glEnable(GL_NORMALIZE);

	//This will speed up material changes
	//glColorMaterial(GL_FRONT, GL_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);
	
	//Setup Some temp vars
	Quat Temp;
	Mat4 M;
	float s;
	glPushMatrix();

	//scale = _bodyScale;
	glScalef(scale, scale, scale);
	

	if(play)
	{	

		//Setup what to interpolate between
		int nextFrame = currentFrame+1;	 
		s = (time-keyFrames[currentFrame].time) / (float)(keyFrames[nextFrame].time-keyFrames[currentFrame].time);
		

		////////////////////
		// Draw Body
		////////////////////
		//Actualy Robot Position
		Vec3f t = interp(keyFrames[currentFrame].torso_tran, keyFrames[nextFrame].torso_tran, s);
		
		//body translation
		glTranslatef(t[0], t[1], t[2] );
		
		//Twist, Tilt, Lean
		Temp = slerp(keyFrames[currentFrame]._body, keyFrames[nextFrame]._body, s);
		unitize(Temp);  
		M=unit_quat_to_matrix(Temp);
		glMultMatrixd(M);

		/////////////////////////////////////////////////
		//This is to correct for height and lean and tilt.
		// Now Hordan will will lean and tilt around his feet.
		/////////////////////////////////////////////////
		glTranslatef(0.0f, 6.5f, 0.0f);
		
		//Apply Upper Torso Twist
		glPushMatrix();
		Temp = slerp(keyFrames[currentFrame].u_torso_rot, keyFrames[nextFrame].u_torso_rot, s);
		unitize(Temp);  
		M=unit_quat_to_matrix(Temp);
		glMultMatrixd(M);
		//Draw the head & Body
		robot->body.draw();
		

		////////////////////
		//RightArm
		////////////////////
		glPushMatrix();
			//Shoulder
			glTranslatef(-1.2f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].r_shoulder_rot, keyFrames[nextFrame].r_shoulder_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->bicep.draw();
			
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

			//Elbow
			glTranslatef(-1.75f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].r_elbow_rot, keyFrames[nextFrame].r_elbow_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->forearm.draw();
			
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

			//Hand
			glTranslatef(-1.5f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			
			robot->hand.draw();
			
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glPopMatrix();



		///////////////
		//LeftArm
		///////////////
		glPushMatrix();
			//Shoulder
			glTranslatef(1.2f, 0.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].l_shoulder_rot, keyFrames[nextFrame].l_shoulder_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->bicep.draw();
			
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

			//Elbow
			glTranslatef(1.75f, 0.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].l_elbow_rot, keyFrames[nextFrame].l_elbow_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->forearm.draw();
			
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

			//Hand
			glTranslatef(1.5f, 0.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
			
			robot->hand.draw();
		
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			
			if(sword_out)
			{
				glScalef(0.25f, 0.25f, 0.25f);
				glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
				robot->sword.draw();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				glScalef(4.0f, 4.0f, 4.0f);
			}

		glPopMatrix();



		/////////////////////
		//Pelvis
		// bottomhalf Independent from upper half so we pop.
		////////////////////
		glPopMatrix();
		
		//Apply Lower Body Twist
		Temp = slerp(keyFrames[currentFrame].l_torso_rot, keyFrames[nextFrame].l_torso_rot, s);
		unitize(Temp);
		M=unit_quat_to_matrix(Temp);
		glMultMatrixd(M);

		//////////////////////
		//Right Leg
		/////////////////////
		glPushMatrix();
			//Thigh
			glTranslatef(0.75f, -3.0f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].r_hip_rot, keyFrames[nextFrame].r_hip_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->thigh.draw();

			//Knee
			glTranslatef(0.0f, -1.5f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].r_knee_rot, keyFrames[nextFrame].r_knee_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->calf.draw();

			//Foot
			glRotatef(90.0f, 0.0, 1.0f, 0.0f);
			glTranslatef(0.0f, -1.75f, 0.0f);
			
			robot->foot.draw();

		glPopMatrix();


		//////////////////////
		//Left Leg
		/////////////////////
		glPushMatrix();
			//Thigh
			glTranslatef(-0.75f, -3.0f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].l_hip_rot, keyFrames[nextFrame].l_hip_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->thigh.draw();

			//Knee
			glTranslatef(0.0f, -1.5f, 0.0f);
			Temp = slerp(keyFrames[currentFrame].l_knee_rot, keyFrames[nextFrame].l_knee_rot, s);
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			
			robot->calf.draw();
		
			//Foot
			glRotatef(90.0f, 0.0, 1.0f, 0.0f);
			glTranslatef(0.0f, -1.75f, 0.0f);
			
			robot->foot.draw();

		glPopMatrix();

	}else{

		//body translation
		glTranslatef(torso_tran[0], torso_tran[1], torso_tran[2] );
		
		Temp =_bodyTwist*_bodyLean*_bodyTilt;  //Actually Body Rotation
		unitize(Temp);
		M=unit_quat_to_matrix(Temp);
		glMultMatrixd(M);
		
		/////////////////////////////////////////////////
		//This is to correct for height and lean and tilt.
		// Now Hordan will will lean and tilt around his feet.
		/////////////////////////////////////////////////
		glTranslatef(0.0f, 6.5f, 0.0f);
		
		//Upper Torso
		glPushMatrix();
		unitize(u_torso);
		M=unit_quat_to_matrix(u_torso);
		glMultMatrixd(M);
		//Draw the head & Body
		
		robot->body.draw();
		
		//RightArm
		glPushMatrix();
			//Shoulder
			glTranslatef(-1.2f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			Temp = r_shoulder*r_shoulderYaw*r_shoulderTwist;
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			robot->bicep.draw();
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

			//Elbow
			glTranslatef(-1.75f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			unitize(r_elbow);
			M=unit_quat_to_matrix(r_elbow);
			glMultMatrixd(M);
			robot->forearm.draw();
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

			//Hand
			glTranslatef(-1.5f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			robot->hand.draw();
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glPopMatrix();

		//LEFT ARM
		glPushMatrix();
			//Shoulder
			glTranslatef(1.2f, 0.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			Temp = l_shoulder*l_shoulderYaw*l_shoulderTwist;
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			robot->bicep.draw();
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

			//Elbow
			glTranslatef(1.75f, 0.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			unitize(l_elbow);
			M=unit_quat_to_matrix(l_elbow);
			glMultMatrixd(M);
			robot->forearm.draw();
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

			//Hand
			glTranslatef(1.5f, 0.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
			robot->hand.draw();
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

			if(sword_out)
			{
				glScalef(0.25f, 0.25f, 0.25f);
				glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
				robot->sword.draw();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				glScalef(4.0f, 4.0f, 4.0f);
			}

		glPopMatrix();


		//bottomhalf Independent from upper half so we pop.
		glPopMatrix();
		
		unitize(l_torso);  //Adjust lower body twist
		M=unit_quat_to_matrix(l_torso);
		glMultMatrixd(M);
		
		//Right Leg
		glPushMatrix();
			//Thigh
			glTranslatef(0.75f, -3.0f, 0.0f);
			Temp = r_hip*r_hipYaw*r_hipTwist;
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			robot->thigh.draw();

			//Knee
			glTranslatef(0.0f, -1.5f, 0.0f);
			unitize(r_knee);
			M=unit_quat_to_matrix(r_knee);
			glMultMatrixd(M);
			robot->calf.draw();

			//Foot
			glRotatef(90.0f, 0.0, 1.0f, 0.0f);
			glTranslatef(0.0f, -1.75f, 0.0f);
			robot->foot.draw();

		glPopMatrix();


		//Left leg
		glPushMatrix();
			//Thigh
			glTranslatef(-0.75f, -3.0f, 0.0f);
			Temp = l_hip*l_hipYaw*l_hipTwist;
			unitize(Temp);
			M=unit_quat_to_matrix(Temp);
			glMultMatrixd(M);
			robot->thigh.draw();

			//Knee
			glTranslatef(0.0f, -1.5f, 0.0f);
			unitize(l_knee);
			M=unit_quat_to_matrix(l_knee);
			glMultMatrixd(M);
			robot->calf.draw();
		
			//Foot
			glRotatef(90.0f, 0.0, 1.0f, 0.0f);
			glTranslatef(0.0f, -1.75f, 0.0f);
			robot->foot.draw();

		glPopMatrix();


	}
	glPopMatrix();
}


void Robot::draw_sphere(float radius)
{

	
	glScalef(radius, radius, radius);
	
	//sphere.draw();
	glScalef(1.0/radius, 1.0/radius, 1.0/radius);

}

void Robot::draw_forearm(Quat& q, float width, float length, robotMesh* robot)
{

	
	//glTranslatef(0.0, 0.0, width);
	unitize(q);
	const Mat4 M=unit_quat_to_matrix(q);
	glMultMatrixd(M);

	//gluDisk(quadratic,0.0f, width, 10, 10);
	//gluCylinder(quadratic,width, width, length, 10, 10);
	//cylindar.draw();
	robot->forearm.draw();
	//glTranslatef(0.0, 0.0, length);
	//gluDisk(quadratic,0.0f, width, 10, 10);
	//glTranslatef(0.0, 0.0, width);
	//gluSphere(quadratic, width, 32, 32);
	//sphere.draw();

	

}

void Robot::draw_bicep(Quat& q,  float width, float length, robotMesh* robot)
{

	

	//glTranslatef(0.0, 0.0, width);
	//gluSphere(quadratic, width, 32, 32);
	//sphere.draw();

	const Mat4 M=unit_quat_to_matrix(q);

	glMultMatrixd(M);

	//glRotatef(angle2, 0.0, 1.0, 0.0);
	//gluDisk(quadratic,0.0f, width, 10, 10);
	//gluCylinder(quadratic,width, width, length, 10, 10);
	//glScalef(0.5f, 0.2f, 0.5f);
	//cylindar.draw();
	robot->bicep.draw();
	//glTranslatef(0.0, 0.0, length);
	//gluDisk(quadratic,0.0f, width, 10, 10);
	//glTranslatef(0.0, 0.0, width);
	//gluSphere(quadratic, width, 32, 32);
	//sphere.draw();


}

void Robot::draw_body(float scale, robotMesh* robot)
{

	
	glScalef(1.4f*scale, 2.6f*scale, 1.4f*scale);
	glTranslatef( 0.0f,  -1.0f  , 0.0f );
	robot->body.draw();
	glScalef(1.0f/(1.4f*scale), 1.0f/(2.6f*scale), 1.0f/(1.4f*scale));

}

Vec3f Robot::interp(Vec3f &a, Vec3f &b, float s)
{
	return ((1-s)*a)+(b*s);

}
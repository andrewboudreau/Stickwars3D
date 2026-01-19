#include "robotMesh.h"


robotMesh::robotMesh()
{
	built=false;
	KF.resize(0);
}

void robotMesh::DL()
{
	//Build the display lists on the first render.
	if(built)
		return;
	cout<<"Robot Loaded!"<<endl;

	bicep.loadObj("./robot/bicep.obj");
	forearm.loadObj("./robot/forearm.obj");
	hand.loadObj("./robot/hand.obj");
	body.loadObj("./robot/torso.obj");
	foot.loadObj("./robot/foot.obj");
	calf.loadObj("./robot/calf.obj");
	thigh.loadObj("./robot/thigh.obj");
	sword.loadObj("./robot/sword.obj");

	bicep.createDL();
	forearm.createDL();
	hand.createDL();
	body.createDL();
	foot.createDL();
	calf.createDL();
	thigh.createDL();
	sword.createDL();

	built=true;
}

void robotMesh::import_keyframes_ascii(char * filename, int index){
	
	robotKeyFrame k;
	if(index>=KF.size())
		KF.resize(index+1);
	KF[index].resize(0);

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

		KF[index].push_back(k);
	}
	//	cout << "keyframes loaded " << keyFrames.size() << endl;
	//	print_keyframes();
	file.close();

}

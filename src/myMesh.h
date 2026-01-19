#ifndef MYMESH_H
#define MYMESH_H

#include <vector>
#include <iostream>
#include <math.h>
#include <gfx/vec3.h>
#include <fstream>
#include <gfx/gltools.h>
#include <gfx/script.h>


 

using namespace std;



class myMesh
{
private:
       vector< Vec3f > vertices;
       vector< vector<int> > polygons;
       vector< Vec3f > normals;
	
	   
	   
       int width;
       int length;

	 
       
public:
       
	
	
		myMesh();
       ~myMesh();
	   
		//bounding box 0=min 1=max
	   vector< TVec3<float> > box;

	   //Display List
	   GLuint DL;
	   
	   void displayList();
       void draw() { glCallList(DL); };
       //Import/Exporting
       void loadObj(char* file);
       void saveObj(char* file);
       
	   //data
	   TVec3<float> getVertex(int x) { return vertices[x]; };
	   int numVertices() { return vertices.size(); };
	   


	   //Normals
       void calcNormals();
       
	  friend struct myOBJReader;
       
       
};



struct myOBJReader : public CmdEnv
{
    myMesh& obj;
    myOBJReader(myMesh& m0);

    int vertex(const CmdLine& cmd);
    int face(const CmdLine& cmd);
	int normal(const CmdLine& cmd);
	int texture(const CmdLine& cmd);
};


#endif //MESH_H
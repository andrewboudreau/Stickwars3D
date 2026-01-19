#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include <iostream>
#include <math.h>
#include <gfx/vec3.h>
#include <fstream>
#include <gfx/gltools.h> 
#include <gfx/raster.h>

#include "myMesh.h"
#include "shadow.h"


using namespace std;
//typedef TVec3<float>  Vec3f;

typedef struct
{
  TVec3 <float> pos;
  int team;
  int teamCap[2];	//captures per team for count
  
} flags;


class heightMap : public shadow
{
private:

	   //flag
	   myMesh flag;
	   

       vector< TVec3<float> > vertices;
       vector< TVec3<int> > triangles;
       vector< TVec3<float> > normals;
       vector< TVec3< TVec2<float> > > texcoords;


       int width;
       int length;
	   ByteRaster *ground;
	   //ByteRaster *grass;
	   //ByteRaster *grass_snow;

	   

       
public:

       heightMap();
       ~heightMap();

	   //flags
	   vector<flags> _flag;
	   int teamFlag[2];//num of flags per team
	   

	   float scale;

       //Drawing
	   void draw(GLfloat light_pos[4], bool FLAG);
       //void draw();
       void drawEdges();
       void drawNormals();
       
       //Import/Exporting
       void loadObj(char* file);
       void saveObj(char* file);
       
       //Normals
       void calculateNormals();
       
       //Data
       int getX(){return length;};
	   int getZ(){return width;};
       float getLength() {return (float)length*scale;};
       float getWidth() {return (float)width*scale;};
       int polygonCount() {return triangles.size();};
       int vertexCount() {return vertices.size();};
	   float getScale() {return scale; };
       
       //heightmap stuff
//	   float max(float x, float y);
	   void makeHill(int rad, int xOrig, int yOrig, float smooth);
	   void makePyramid(int rad, int xOrig, int yOrig);
	   void explodeHill(int rad, int xOrig, int yOrig);
       void initHeightMap(int width, int length);
       void randomHeight();
	   void addHeight(float x, float z);

       //Compiler complains, i don't know.
       float getHeight(int x, int z);
       float getHeight(float x, float z);
       float getHeight(double x, double z);

	   bool mapCollision(TVec3<float> from, float h_angle, float v_angle, float maxDist);

	   float distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z);
      
	   void bulletHit(float x, float z, float strength);
	 
       
       
};
#endif //HEIGHTMAP_H

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <iostream>
#include <math.h>
#include <string>
#include <gfx/vec3.h>
#include <gfx/vec4.h>
#include <fstream>
#include <gfx/gltools.h>
#include <gfx/script.h>
 

using namespace std;
//typedef TVec3<float>  Vec3f;


typedef struct material
{
  string name;                   // Material Name 
  Vec3f diffuse;           // diffuse component 
  Vec3f ambient;           // ambient component 
  Vec3f specular;          // specular component 
  Vec3f emmissive;         // emmissive component 
  Vec3f shininess;            // specular exponent 
} material;


class Mesh
{
private:
       vector< Vec3f > vertices;
       vector< vector<int> > polygons;
       vector< Vec3f > normals;
	   vector<string> matID;
	   bool t_reverseWinding; //Clockwise(reverseWinding) and Counter-Clockwise
	   bool _has_normals;

	   //display list
	  
	   bool built;

	   //Keeps track of current Material Settings.
	   material currentMat;
       
	   string materialLibrary;
       string currentMaterial;
	   
	   int width;
       int length;
       
public:
	 GLuint DL;

	   vector< material> materials;
       Mesh();
       Mesh(const Mesh& m);
       ~Mesh();

       //Drawing
       void draw();
	   GLuint createDL();
	   void displayList();
       void drawEdges();
       void drawNormals();
	   void load_material(string& material);
       
       //Import/Exporting
       void loadObj(char* file);
       void saveObj(char* file);
       
       //Normals
       void calculateNormals();
	   void reverseWinding();
	   void has_normals();
       
       //Data
       int getLength() {return length;};
       int getWidth() {return width;};
       int polygonCount() {return polygons.size();};
       int vertexCount() {return vertices.size();};
	   int materialCount() {return materials.size();};

	   //data
	   TVec3<float> getVertex(int x) { return vertices[x]; };
	   int numVertices() { return vertices.size(); };


       
	  friend struct OBJReader;
       
       
};

/* Garlnds Mesh Struct
struct Mesh
{
    std::vector<Vec3f> vertices;
    std::vector< vector<int> >   faces;
};
*/


struct OBJReader : public CmdEnv
{
    Mesh& obj;
    OBJReader(Mesh& m0);

	//Processing obj files
    int vertex(const CmdLine& cmd);
    int face(const CmdLine& cmd);
	int mtl(const CmdLine& cmd);
	int mtllib(const CmdLine& cmd);
	int g(const CmdLine& cmd);
	int comment(const CmdLine& cmd);
	int l(const CmdLine& cmd);

	//Processing mtl files
	int newmtl(const CmdLine& cmd);
	int illum(const CmdLine& cmd);
	int kd(const CmdLine& cmd);
	int ka(const CmdLine& cmd);
	int tf(const CmdLine& cmd);
	int ni(const CmdLine& cmd);
	int ks(const CmdLine& cmd);
	int ns(const CmdLine& cmd);
	int p (const CmdLine& cmd);
	int vn(const CmdLine& cmd);

};


#endif //MESH_H

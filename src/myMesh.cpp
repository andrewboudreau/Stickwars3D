#include "myMesh.h"
using namespace std;
myMesh::myMesh()
{
	polygons.resize(0);
    vertices.resize(0);
    normals.resize(0);
	box.resize(2);

	

	int j;
			//0=min 1=max
	for(j=0; j<3; j++)	//0, 1, 2::x, y, z
	{
		box[0][j]=1000000.0;
		box[1][j]=-1000000.0;
	}

}


myMesh::~myMesh(){}


void myMesh::displayList()//create a display list for the object
{
	

	 DL=glGenLists(1);
	 glNewList(DL, GL_COMPILE);
     for(int x=0; x<polygons.size(); x++)
     {
         glBegin(GL_POLYGON);
		 for(int y=0 ; y<polygons[x].size(); y++)
		 {
             glNormal3fv( normals[polygons[x][y]] );          
             glVertex3fv( vertices[polygons[x][y]] );
			 
		 } 
         glEnd();
     }  
     glEndList();

}


void myMesh::calcNormals()
{
	 TVec3<float> cur;
     TVec3<float> temp;
	 TVec3<float> v1, v2;//the vertices from whcih to calculate the normal
     normals.resize(vertices.size());//need to calculate a normal per vertex
	 int x, y;//for loops

     for(x=0; x<polygons.size(); x++)
     {
		 v1=vertices[polygons[x][0]] - vertices[polygons[x][1]];
		 v2=vertices[polygons[x][0]] - vertices[polygons[x][2]];
         
         temp = v1^v2;

         unitize(temp);

         for(y=0; y<3; y++)
	        normals[polygons[x][y]] += temp;
     }
     
     for(x=1; x<normals.size(); x++)
             unitize(normals[x]);      
}

void myMesh::loadObj(char* file)
{

	myOBJReader reader(*this);

	vertices.resize(0);
	polygons.resize(0);
	normals.resize(0);
	reader.do_file(file);

	normals.resize(vertices.size());

	cout << "Vertices=" << vertices.size() << endl;
	
	calcNormals();
	displayList();
    return;
     
}


myOBJReader::myOBJReader(myMesh& m0)
    : obj(m0)
{
    register_method("v", this, &myOBJReader::vertex);
    register_method("f", this, &myOBJReader::face);
	register_method("vn", this, &myOBJReader::normal);
	register_method("vt", this, &myOBJReader::texture);
}

int myOBJReader::vertex(const CmdLine& cmd)
{
    TVec3<float> v;

    cmd.collect_as_numbers(v, Vec3f::dim());
    obj.vertices.push_back(v);

    return SCRIPT_OK;
}

int myOBJReader::face(const CmdLine& cmd)
{
    vector<int> f;
    cmd.collect_as_numbers(f);

    // OBJ files use 1-based indexing rather than 0-based indexing.
    // Fix this before adding this face to the face list.
    for(int i=0; i<f.size(); ++i)   f[i] = f[i]-1;

    obj.polygons.push_back(f);

    return SCRIPT_OK;
}

int myOBJReader::normal(const CmdLine& cmd)
{
    TVec3<float> vn;

    cmd.collect_as_numbers(vn, Vec3f::dim());
    obj.normals.push_back(vn);

    return SCRIPT_OK;
}

int myOBJReader::texture(const CmdLine& cmd)
{
	//we read and ignore these

    return SCRIPT_OK;
}





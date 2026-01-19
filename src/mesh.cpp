
#include "mesh.h"
using namespace std;



Mesh::Mesh()
{
   //Remember that you starting at 0;
   
   vertices.resize(0);
   polygons.resize(0);
   normals.resize(0);
   materials.resize(0);
   matID.resize(0);
   t_reverseWinding = false;
   _has_normals = false;

   //Display List Vars
   DL = -1;
   built = false;


   
}

/*Mesh::Mesh(const Mesh& m) {
    vertices = m.vertices;
    polygons = m.polygons;
    normals = m.normals;
}*/

Mesh::~Mesh(){}

void Mesh::loadObj(char* file)
{
  
	OBJReader reader(*this);

	vertices.resize(0);
	polygons.resize(0);
	normals.resize(0);
	materials.resize(0);

	reader.do_file(file);
	
	
	if(materialLibrary != ""){//Adjust for relative dir.
		cout << "Using material lrary: " << materialLibrary << endl;
		reader.do_file("models/"+materialLibrary);
	}


	normals.resize(vertices.size());

	cout << "Vertices.size: "	<< vertices.size()	<< endl;
	cout << "Polygons.size: "	<< polygons.size()	<< endl;
	cout << "Materials.size: "  << materials.size() << endl;
	cout << "matID.size: "		<< matID.size()		<< endl;
	
	//cout << "calculating normals" << endl;

	if(!_has_normals)
		calculateNormals();

    cout << "Mesh Loaded. " << endl;
    return;
     
}

void Mesh::saveObj(char* file)
{
     cout << "Saving ... " << file << endl;
     
     ofstream fout(file); 
     
     for(int i = 1; i<=vertices.size(); i++)
     {
             fout << "v" <<  " " <<vertices[i][0] << " " <<vertices[i][1] << " " <<vertices[i][2] << "\r\n";
             
     }
     
     for(i = 0; i<polygons.size(); i++)
     {
             fout << "f" << " " << polygons[i][0] << " " <<polygons[i][1] << " " <<polygons[i][2] << "\r\n";
             
     }
     
     fout.flush();
     fout.close();
     return;
     
}
void Mesh::displayList()
{
	createDL();
}

GLuint Mesh::createDL()
{
	 cout << "Creating Display List:" << endl;
	 
	 //calculateNormals();
	 DL = glGenLists(1);
	 glNewList(DL, GL_COMPILE);

     //cout <<"Drawing polygons"<<  polygons.size() << endl;
     for(int i = 0; i < polygons.size(); i++)
     {
		 //cout << i << endl;
		 if(!matID.empty())
			load_material(matID[i]);
		 
         // The current face might be a quad, polygon, or a triangle so we must
         // begin rendering based on the number of vertex points there are.
		 if(polygons[i].size() == 3) glBegin(GL_TRIANGLES);
		 else if(polygons[i].size() == 4) glBegin(GL_QUADS);
		 else glBegin(GL_POLYGON);
		 for(int j = 0 ; j < polygons[i].size();j++)
		 {
             glNormal3fv( normals[ polygons[i][j] ] );          
             glVertex3fv( vertices[ polygons[i][j] ] );
		 } 
         glEnd();
  
     }  
      glEndList();
     //cout <<"Drawing Done normals"<<  normals.size() << endl;
	  built = true;
     return(DL);
}


void Mesh::draw()
{
   
   if(built)
   {
      glCallList(DL);

   }
   else
   {
     //cout <<"Drawing polygons"<<  polygons.size() << endl;
     for(int i = 0; i < polygons.size(); i++)
     {
		 // The current face might be a quad, polygon, or a triangle so we must
         // begin rendering based on the number of vertex points there are.
		 if(polygons[i].size() == 3) glBegin(GL_TRIANGLES);
		 else if(polygons[i].size() == 4) glBegin(GL_QUADS);
		 else glBegin(GL_POLYGON);

		 for(int j = 0 ; j < polygons[i].size();j++)
		 {
             glNormal3fv(normals[ polygons[i][j] ] );          
             glVertex3fv(vertices[ polygons[i][j] ] );
		 } 
         glEnd();
     }       
     //cout <<"Drawing Done normals"<<  normals.size() << endl;
   }
   

}
void Mesh::drawEdges()
{
     for(int i = 0; i < polygons.size(); i++)
     {
         glBegin(GL_LINE_LOOP);
		 for(int j = 0 ; j < polygons[i].size();j++)
		 {
             glVertex3fv(vertices[ polygons[i][j] ]);
		 }	  
         glEnd();
     }       
}


void Mesh::calculateNormals()
{
	 cout <<"CalculateNormals()"<< endl;
     TVec3<float> a;
     TVec3<float> b;
     TVec3<float> temp;
     normals.resize(vertices.size());

     for(int i = 0 ; i < polygons.size(); i++)
     {
             a = vertices[polygons[i][0]] - vertices[polygons[i][1]];
             b = vertices[polygons[i][0]] - vertices[polygons[i][2]];
             
             if(t_reverseWinding)
			 {
				temp = b^a;
			 }else{
				temp = a^b;
           	 } 
			 
			 unitize(temp);
			 
          
			 //Update all vertices of polygon
			 for(int j = 0 ; j < polygons[i].size(); j++)
				normals[ polygons[i][j] ] += temp;
     }
     
     for(i = 0 ; i < normals.size(); i++)
     {
             unitize(normals[i]);
     }
     
     cout << "Normals.size() = " << normals.size() <<endl;
             
 }

void Mesh::reverseWinding()
{
	t_reverseWinding = !t_reverseWinding;
	//cout <<" REVERSED" << endl;

}

void Mesh::has_normals()
{
	_has_normals = !_has_normals;
	//cout <<" REVERSED" << endl;

}

void Mesh::drawNormals()
{
     for(int i = 0; i < vertices.size(); i++)
     {
         glBegin(GL_LINES);
           glColor3f(1.0,0.0,0.0);
           glVertex3f(vertices[i][0],vertices[i][1],vertices[i][2]);
           glColor3f(0.0,1.0,0.0);
           glVertex3f(vertices[i][0]+normals[i][0]/10.0,vertices[i][1]+normals[i][1]/10.0,vertices[i][2]+normals[i][2]/10.0);
         glEnd();
     }
}

void Mesh::load_material(string &materialName)
{

	if(currentMaterial == materialName)
		return;
	else
		currentMaterial = materialName;

	
	for(int i = 0; i < materials.size(); i++)
	{	
		if(materials[i].name == materialName)
		{
				
			   	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materials[i].diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materials[i].ambient);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materials[i].specular);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materials[i].emmissive);
			//	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materials[i].shininess[0]);
				currentMaterial = materials[i].name;
				//cout << "Material" << i << " Changed to :" << materials[i].name << endl;
				return;
		}
		
	}
	return;

}



OBJReader::OBJReader(Mesh& m0)
    : obj(m0)
{
	//For obj files
    register_method("v", this, &OBJReader::vertex);
    register_method("f", this, &OBJReader::face);
	register_method("usemtl", this, &OBJReader::mtl);
	register_method("mtllib", this, &OBJReader::mtllib);
	
	//Processing mtl files
	register_method("newmtl", this, &OBJReader::newmtl);
	register_method("illum", this, &OBJReader::illum);
	register_method("Kd", this, &OBJReader::kd);
	register_method("Ka", this, &OBJReader::ka);
	register_method("Tf", this, &OBJReader::tf);
	register_method("Ni", this, &OBJReader::ni);
	register_method("Ks", this, &OBJReader::ks);
	register_method("Ns", this, &OBJReader::ns);
	register_method("#",  this, &OBJReader::comment);
	register_method("g",  this, &OBJReader::g);
	register_method("l",  this, &OBJReader::l);
	register_method("p",  this, &OBJReader::l);
	register_method("vn",  this, &OBJReader::vn);
 

}

int OBJReader::mtllib(const CmdLine& cmd)
{
    std::vector<std::string> s;

    cmd.collect_as_strings(s);
	obj.materialLibrary = s[0];
	std::cout <<"Using Material:"<< s[0] << endl;

    return SCRIPT_OK;
}

int OBJReader::mtl(const CmdLine& cmd)
{
    std::vector<std::string> s;

    cmd.collect_as_strings(s);
	obj.currentMaterial = s[0];


    return SCRIPT_OK;
}

int OBJReader::vertex(const CmdLine& cmd)
{
    TVec3<float> v;

    cmd.collect_as_numbers(v, Vec3f::dim());
    obj.vertices.push_back(v);

    return SCRIPT_OK;
}

int OBJReader::face(const CmdLine& cmd)
{
    vector<int> f;
    cmd.collect_as_numbers(f);

    // OBJ files use 1-based indexing rather than 0-based indexing.
    // Fix this before adding this face to the face list.
    for(int i=0; i<f.size(); ++i)   f[i] = f[i]-1;

	if(obj.currentMaterial != "")
		obj.matID.push_back(obj.currentMaterial);
    
	obj.polygons.push_back(f);

    return SCRIPT_OK;
}


///////////////////////
// Material Registers
///////////////////////
int OBJReader::newmtl(const CmdLine& cmd)
{
    std::vector<std::string> s;
	
    cmd.collect_as_strings(s);
	//cout << "newmtl: " << s[0] << endl;
	
	obj.materials.resize(obj.materials.size()+1);
	obj.materials[obj.materials.size()-1].name = s[0];


    return SCRIPT_OK;
}

int OBJReader::illum(const CmdLine& cmd)
{
	//Just skip over illum since we don't need it
    return SCRIPT_OK;
}

int OBJReader::kd(const CmdLine& cmd)
{
    Vec3f f;

    cmd.collect_as_numbers(f, Vec3f::dim());
	obj.materials[obj.materials.size()-1].diffuse = f;

    return SCRIPT_OK;
}

int OBJReader::ka(const CmdLine& cmd)
{
    Vec3f f;
	//cout << "KA" << endl;
    cmd.collect_as_numbers(f, Vec3f::dim());
	obj.materials[obj.materials.size()-1].ambient = f;

    return SCRIPT_OK;
}

int OBJReader::tf(const CmdLine& cmd)
{
	//Just skip over illum since we don't need it
    return SCRIPT_OK;
}

int OBJReader::ni(const CmdLine& cmd)
{
    Vec3f f;
    cmd.collect_as_numbers(f, Vec3f::dim());

    return SCRIPT_OK;
}
int OBJReader::ks(const CmdLine& cmd)
{
    Vec3f f;
    cmd.collect_as_numbers(f, Vec3f::dim());
	obj.materials[obj.materials.size()-1].specular = f;

    return SCRIPT_OK;
}

int OBJReader::ns(const CmdLine& cmd)
{
    Vec3f f;;
    cmd.collect_as_numbers(f, Vec3f::dim());

	//cout << "Ns = " <<  endl;
	obj.materials[obj.materials.size()-1].shininess = f[0];
    return SCRIPT_OK;
}
int OBJReader::comment(const CmdLine& cmd)
{
    std::vector<std::string> s;
    cmd.collect_as_strings(s);
	cout << s[0] << endl;
    return SCRIPT_OK;
}

int OBJReader::g(const CmdLine& cmd)
{
    //Skip grouping
    return SCRIPT_OK;
}

int OBJReader::l(const CmdLine& cmd)
{
    //Skip l
    return SCRIPT_OK;
}

int OBJReader::p(const CmdLine& cmd)
{
    //Skip p
    return SCRIPT_OK;
}

int OBJReader::vn(const CmdLine& cmd)
{
    Vec3f f;
	
    cmd.collect_as_numbers(f, Vec3f::dim());
	obj.normals.push_back(f);

    return SCRIPT_OK;
}
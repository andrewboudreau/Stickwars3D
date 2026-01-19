
#include "heightMap.h"

static void load_tex(ByteRaster *img)
{
    if( img )
    {
		
        glEnable(GL_TEXTURE_2D);
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        // Figure out what format the texture image is in.
        GLenum format;
        switch( img->channels() )
        {
        case 1:  format = GL_LUMINANCE;       break;
        case 2:  format = GL_LUMINANCE_ALPHA; break;
        case 3:  format = GL_RGB;             break;
        case 4:  format = GL_RGBA;            break;
        default: abort();
        }

        // And load the texture image.  This will be the current
        // texture, applied to all polygons, until we tell OpenGL
        // otherwise.
        glTexImage2D(GL_TEXTURE_2D, 0,
                img->channels(), img->width(), img->height(), 0,
                format, GL_UNSIGNED_BYTE, img->head());
    }
    else
        glDisable(GL_TEXTURE_2D);
}


using namespace std;
heightMap::heightMap()
{
   //Well the obj file starts index of 1 instead of 0;
   
   vertices.resize(0);
   triangles.resize(0);
   normals.resize(0);
   texcoords.resize(0);
   _flag.resize(9);
   //grass = read_image("./textures/grass.png");
   //ground = read_image("./textures/grounds_low.jpg");
   //ground = read_image("./textures/grounds2.jpg");
   //grass_snow = read_image("./textures/grass-snow.jpg");

}

heightMap::~heightMap(){

	//if(ground){delete ground; ground=NULL;}
	//if(grass) { delete grass; grass=NULL; }
	//if(grass_snow){delete grass_snow; grass_snow=NULL; }

}

void heightMap::loadObj(char* file)
{
     char type;
     TVec3<float> vertex;
     TVec3<int> index;
     cout << "Loading " << file << endl;
     
     width=0;
     length=0;
     
     triangles.resize(0);
     vertices.resize(1);
     
     ifstream fin(file); 
     while (!fin.eof())
     {
           fin >> type;
           if( type == 'v')
           {
             fin >> vertex[0] >> vertex[1] >> vertex[2];
             vertices.push_back(vertex);
           }
           else if( type == 'f')
           {
             fin >>  index[0] >> index[1] >> index[2];
             triangles.push_back(index);
             //cout << triangles[3] << endl;
           }
           else
           {
               fin >> type; 
           }
     }
     
     normals.resize(vertices.size());
     cout << "Calculating Normals ... " << endl;
     //calculateNormals();
     //cout << "Vertex Size =" << vertices.size() << endl;
           
     fin.close();
     cout << "heightMap Loaded. " << endl;
     return;
     
}

void heightMap::saveObj(char* file)
{
     cout << "Saving ... " << file << endl;
     
     ofstream fout(file); 
     //int i;
     /*for(i = 0; i < vertices.size(); i++)
     {
             fout << "v" <<  " " <<vertices[i][0] << " " <<vertices[i][1] << " " <<vertices[i][2] << "\r\n";
             
     }
     
     for(i = 0; i < triangles.size(); i++)
     {
             fout << "f" << " " << triangles[i][0]+1 <<"/"<<triangles[i][0]+1 <<" " <<triangles[i][1]+1 <<"/"<<triangles[i][1]+1<< " " <<triangles[i][2]+1 <<"/"<<triangles[i][2]+1 <<"\r\n";
             
     }
	 for(i = 0; i < texcoords.size(); i++)
     {
             fout << "vt" << " " << texcoords[i][0] << " " << texcoords[i][1] << "\r\n";
             
     }*/
     
     fout.flush();
     fout.close();
     return;
     
}

void heightMap::calculateNormals()
{
	 TVec3<float> current;
     TVec3<float> a;
     TVec3<float> b;
     TVec3<float> temp;
     int i;
     for(i = 0 ; i < triangles.size(); i++)
     {
             a = vertices[triangles[i][0]] - vertices[triangles[i][1]];
             b = vertices[triangles[i][0]] - vertices[triangles[i][2]];
             
             temp = a^b;
             unitize(temp);
             
             normals[ triangles[i][0] ] += temp;
             normals[ triangles[i][1] ] += temp;
             normals[ triangles[i][2] ] += temp;
             
     }
     
     for( i = 1 ; i < normals.size(); i++)
     {
             unitize(normals[i]);
     } 
             
}

void heightMap::draw(GLfloat light_pos[4], bool shadowFlag)
{
	int c =0;
	int i;
	
    
	for(i = 0; i < triangles.size(); i++)
     {
        
		
		 glBegin(GL_TRIANGLES);
			 
		 
		     glTexCoord2f(texcoords[i] [0] [0],texcoords[i] [0] [1]);
			 glNormal3f(normals[ triangles[i][0] ][0], normals[ triangles[i][0] ][1], normals[ triangles[i][0] ][2] );          
             glVertex3f(vertices[ triangles[i][0] ][0], vertices[ triangles[i][0] ][1], vertices[ triangles[i][0] ][2] );
             
			 glTexCoord2f(texcoords[i] [1] [0],texcoords[i] [1] [1]);
			 glNormal3f(normals[ triangles[i][1] ][0], normals[ triangles[i][1] ][1], normals[ triangles[i][1] ][2] ) ;  
             glVertex3f(vertices[ triangles[i][1] ][0], vertices[ triangles[i][1] ][1], vertices[ triangles[i][1] ][2] );
             
			 glTexCoord2f(texcoords[i] [2] [0],texcoords[i] [2] [1]);
			 glNormal3f(normals[ triangles[i][2] ][0], normals[ triangles[i][2] ][1], normals[ triangles[i][2] ][2] );
             glVertex3f(vertices[ triangles[i][2] ][0], vertices[ triangles[i][2] ][1], vertices[ triangles[i][2] ][2] );
         glEnd();
     }       
     
	 //draw the flags
	 glDisable(GL_TEXTURE_2D);
	 glDisable(GL_LIGHTING);
	for(i=0; i<_flag.size(); i++)
	{
		_flag[i].pos[1]=getHeight(_flag[i].pos[0], _flag[i].pos[2]);

		if(shadowFlag)
		{
			//draw the flag shadow
			glPushMatrix();
			glScalef(1/scale, 1/scale, 1/scale);
			buildShadowMatrix(light_pos);
			projectShadow(_flag[i].pos, &flag);
			glPopMatrix();
		}

		switch(_flag[i].team)
		{
			case 0:
				glColor3f(0.5, 0.5, 0.5);
				break;
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
	
		
		//draw the flags
		 glEnable(GL_LIGHTING);
		glPushMatrix();
		glScalef(1/scale, 1/scale, 1/scale);
		//sDraw(&flag, light_pos, _flag[i].pos); 
		glTranslatef(_flag[i].pos[0], _flag[i].pos[1], _flag[i].pos[2]);
		flag.draw();
		
		glPopMatrix();
		
		
		
	}

    
}


void heightMap::drawEdges()
{
     for(int i = 0; i < triangles.size(); i++)
     {
         glBegin(GL_LINE_LOOP);
             glVertex3f(vertices[ triangles[i][0] ][0], vertices[ triangles[i][0] ][1], vertices[ triangles[i][0] ][2] );
             glVertex3f(vertices[ triangles[i][1] ][0], vertices[ triangles[i][1] ][1], vertices[ triangles[i][1] ][2] );
             glVertex3f(vertices[ triangles[i][2] ][0], vertices[ triangles[i][2] ][1], vertices[ triangles[i][2] ][2] );
         glEnd();
     }       
}

void heightMap::initHeightMap(int w, int l)
{
     TVec3<float> a;
     TVec3<float> b;
     TVec3<int> temp;



	 /*  SNOW SETTING
	 TVec2<float> tex10(0.5f, 0.5f); //-0.5, 0.0
	 TVec2<float> tex00(0.0f, 0.5f); //
	 TVec2<float> tex11(0.5f, 1.0f );
	 TVec2<float> tex01(0.0f, 1.0f);
	 */

	 ///GRASS TEXTURE
	 TVec2<float> tex10(1.0f, 0.5f);
	 TVec2<float> tex00(0.5f, 0.5f);
	 TVec2<float> tex11(1.0f, 1.0f );
	 TVec2<float> tex01(0.5f, 1.0f);


     width = w;
     length = l;
     vertices.resize(width*length);
	 

     triangles.resize(0);
     //vertices.resize(width*length);
     //triangles.resize( ((width-1)*(length-1)) *2);
     normals.resize(width*length+1);
	 texcoords.resize(0);
     int j;
     int i; 
     
     for (i = 0; i < length; i++){
         for(j = 0; j < width; j++){
            vertices[ (j+(i)*length) ][0] = ( i ) ;       
            vertices[ (j+(i)*length) ][1] = ( 0.0 );
            vertices[ (j+(i)*length) ][2] = ( j );

         }
     }
     
     
      for (i = 1; i<length-1; i++){
         for(j = 0; j <width-2; j++){
            
            temp[2] = (j+1)+((i-1)*length);
            temp[1] = j+((i-1)*length);
            temp[0] = j+(i*length);
            
            triangles.push_back(temp);
			
            
            temp[2] = (j+1)+((i-1)*length);
            temp[1] = j+(i*length);
            temp[0] = (j+1)+(i*length);
            
            triangles.push_back(temp);
         }
     }
     
	texcoords.resize(triangles.size());
	int c =0;

    for(i = 0; i < triangles.size(); i++){
        c++;
		if(c%2==0){
			texcoords[i] [0]   = tex11;
			texcoords[i] [1]   = tex00;
			texcoords[i] [2]   = tex01;
		}else{
			texcoords[i] [0]   = tex10;
			texcoords[i] [1]   = tex00;
			texcoords[i] [2]   = tex11;

		}
			
	}//

	//set scale to 10
	//scale=10.0f;


	//set up flags
	cout<<_flag.size()<<endl;
	for(i=0; i<_flag.size(); i++)
	{
		_flag[i].team=0;
		_flag[i].pos[1]=0.0f;
	}
	//team 1 base
	_flag[0].pos[0]=(float)(4)*(scale);
	_flag[0].pos[2]=(float)(4)*(scale);

	//middle
	_flag[1].pos[0]=getLength()/2.0f;
	_flag[1].pos[2]=getWidth()/2.0f;

	//corner
	_flag[2].pos[0]=(float)(4)*(scale);
	_flag[2].pos[2]=(float)(getZ()-4)*(scale);

	//corner
	_flag[3].pos[0]=(float)(getX()-4)*(scale);
	_flag[3].pos[2]=(float)(4)*(scale);

	//middle flags
	_flag[4].pos[0]=getLength()/4.0f;
	_flag[4].pos[2]=getWidth()/4.0f;

	_flag[5].pos[0]=getLength()/4.0f;
	_flag[5].pos[2]=3.0f*getWidth()/4.0f;

	_flag[6].pos[0]=3.0f*getLength()/4.0f;
	_flag[6].pos[2]=getWidth()/4.0f;

	_flag[7].pos[0]=3.0f*getLength()/4.0f;
	_flag[7].pos[2]=3.0f*getWidth()/4.0f;
	//end middle

	//team 2 base
	_flag[8].pos[0]=(float)(getX()-4)*(scale);
	_flag[8].pos[2]=(float)(getZ()-4)*(scale);

	flag.loadObj("./obj/flag.obj");

	//TVec2<int> tex1(1,0);
	//TVec2<int> tex2(0,0);
	//TVec2<int> tex3(1,1);
	//TVec2<int> tex4(0,1);

    cout << "There are " << triangles.size() << " triangles" << endl;
    cout << "Vertex Size =" << vertices.size() << endl;
	cout << "TexCoords Size =" << texcoords.size() << endl;
    calculateNormals();
     
     
     return;
         
}

/*
void heightMap::randomHeight()
{
     
     for(int i = 1; i <= vertices.size(); i++)
     {
             vertices[i][1] +=  (float) rand()/RAND_MAX;
     }       
}*/

void heightMap::makeHill(int rad, int xOrig, int yOrig, float smooth)
{

	int vertices_x_max=2*(rad-1)+xOrig;
	int vertices_y_max=2*(rad-1)+yOrig;
	//bool crater=rand()%2;
	float radius=rad;
	float height;
	for(int x=0;x<rad;x++)
	{
		for(int y=0;y<rad;y++)
		{
			height=(radius*radius-((x-rad)*(x-rad)+(y-rad)*(y-rad)))/smooth;
			//	-(float) ((x+y)/rad)*log10(radius)*(rand()%10000)/10000.0;
			if(height>0)
				height=sqrt(height);
			else
				height=0;
  //          if(crater)
    //            height=-height;
			if(xOrig+x>0 && xOrig+x<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (xOrig+x) * length + (y+yOrig) ][1] += height ;
			}
			
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{	
				vertices[ (vertices_x_max-x+1)*length + (vertices_y_max-y+1) ][1] +=height;
			
			
			}
			if (xOrig+x>0 && xOrig+x<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{
				vertices[ (xOrig+x)*length + (vertices_y_max-y+1) ][1] += height;
			}
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (vertices_x_max-x+1)*length + (yOrig+y) ][1] +=  height;
			}
		}
	}
		 /*  SNOW SETTING
	 TVec2<float> tex10(0.5f, 0.5f); //-0.5, 0.0
	 TVec2<float> tex00(0.0f, 0.5f); //
	 TVec2<float> tex11(0.5f, 1.0f );
	 TVec2<float> tex01(0.0f, 1.0f);
	 */
	int i;
	calculateNormals();
	for(i = 0 ; i < triangles.size() ; i++)
	{
		if( vertices[ triangles[i][2] ][1] >15.0)
		//if( vertices[ triangles[i][0] ][1]+vertices[ triangles[i][1] ][1]+vertices[ triangles[i][2] ][1] > 45.0)
		{
			texcoords[i][0][0]=0.5;
			texcoords[i][0][1]=0.0;
			texcoords[i][1][0]=0.5;
			texcoords[i][1][1]=0.5;
			texcoords[i][2][0]=1.0;
			texcoords[i][2][1]=0.5;
		}
	}
}    

void heightMap::makePyramid(int rad, int xOrig, int yOrig)
{
	int vertices_x_max=2*(rad-1)+xOrig;
	int vertices_y_max=2*(rad-1)+yOrig;
	float height;
	int x,y;
	//crater=round(mod(100*rand(1)+100, 10));
	float slope=rand()%1000/2000.0;
	for(int step=0;step<rad;step++)
	{
		for(x=step;x<rad;x++)
		{
	        y=step;
		    height=slope*step;
			//if(crater==1)
			//	height=-height;
	        if(xOrig+x>0 && xOrig+x<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (xOrig+x) * length + (y+yOrig) ][1] += height ;
			}
			
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{	
				vertices[ (vertices_x_max-x+1)*length + (vertices_y_max-y+1) ][1] +=height;
			
			
			}
			if (xOrig+x>0 && xOrig+x<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{
				vertices[ (xOrig+x)*length + (vertices_y_max-y+1) ][1] += height;
			}
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (vertices_x_max-x+1)*length + (yOrig+y) ][1] +=  height;
			}
		}				
	    for(y=step+1;y<rad;y++)
		{
			x=step;
			height=slope*step;
			//if(crater==1)
            //    height=-height;
	        if(xOrig+x>0 && xOrig+x<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (xOrig+x) * length + (y+yOrig) ][1] += height ;
			}
			
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{	
				vertices[ (vertices_x_max-x+1)*length + (vertices_y_max-y+1) ][1] +=height;
			
			
			}
			if (xOrig+x>0 && xOrig+x<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{
				vertices[ (xOrig+x)*length + (vertices_y_max-y+1) ][1] += height;
			}
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (vertices_x_max-x+1)*length + (yOrig+y) ][1] +=  height;
			}
		} 		
	}
	int i;
	for(i = 0 ; i < triangles.size() ; i++)
	{
		if( vertices[ triangles[i][2] ][1]>15.0)
		{
			texcoords[i][0][0]=0.5;
			texcoords[i][0][1]=0.0;
			texcoords[i][1][0]=0.5;
			texcoords[i][1][1]=0.5;
			texcoords[i][2][0]=1.0;
			texcoords[i][2][1]=0.5;
		}
	}
	calculateNormals();
}

void heightMap::explodeHill(int rad, int xOrig, int yOrig)
{

	xOrig -= rad;
	yOrig -= rad;
	int vertices_x_max=2*(rad-1)+xOrig;
	int vertices_y_max=2*(rad-1)+yOrig;
	//bool crater=rand()%2;
	float radius=rad;
	float height;
	for(int x=0;x<rad;x++)
	{
		for(int y=0;y<rad;y++)
		{
			height=(radius*radius-((x-rad)*(x-rad)+(y-rad)*(y-rad)))/10.0f;
			//	-(float) ((x+y)/rad)*log10(radius)*(rand()%10000)/10000.0;
			if(height>0)
				height=sqrt(height);
			else
				height=0;
  //          if(crater)
    //            height=-height;
			if(xOrig+x>0 && xOrig+x<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (xOrig+x) * length + (y+yOrig) ][1] -= height ;
			}
			
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{	
				vertices[ (vertices_x_max-x+1)*length + (vertices_y_max-y+1) ][1] -=height;
			
			
			}
			if (xOrig+x>0 && xOrig+x<length && vertices_y_max-y+1>0 && vertices_y_max-y+1<width)
			{
				vertices[ (xOrig+x)*length + (vertices_y_max-y+1) ][1] -= height;
			}
			if (vertices_x_max-x+1>0 && vertices_x_max-x+1<length && y+yOrig>0 && y+yOrig<width)
			{
				vertices[ (vertices_x_max-x+1)*length + (yOrig+y) ][1] -=  height;
			}
		}
	}
	calculateNormals();
	int i;
	for(i = 0 ; i < triangles.size() ; i++)
	{
		if( vertices[ triangles[i][2] ][1]<=15.0)
		{
			texcoords[i][0][0]=0.5;
			texcoords[i][0][1]=0.5;
			texcoords[i][1][0]=0.5;
			texcoords[i][1][1]=1.0;
			texcoords[i][2][0]=1.0;
			texcoords[i][2][1]=0.5;
		}
	}
}   




void heightMap::drawNormals()
{
     for(int i = 1; i <= vertices.size(); i++)
     {
         glBegin(GL_LINES);
           glColor3f(1.0,0.0,0.0);
           glVertex3f(vertices[i][0]*scale, vertices[i][1]*scale, vertices[i][2]*scale);
           glColor3f(0.0,1.0,0.0);
           glVertex3f( (vertices[i][0]+normals[i][0])*scale, (vertices[i][1]+normals[i][1])*scale, (vertices[i][2]+normals[i][2])*scale );
         glEnd();
     }
}

float heightMap::getHeight(double x, double z)
{
      if(x > width || x<-width || z>length || z<-length)
        return 0.0;
      
      //average height of surronding vertices
      float avgHeight;
      avgHeight = getHeight((int) floor(x/scale),(int) floor(z/scale));
      avgHeight += getHeight((int) floor(x/scale),(int) ceil(z/scale));
      avgHeight += getHeight((int) ceil(x/scale),(int) floor(z/scale));
      avgHeight += getHeight((int) ceil(x/scale),(int) ceil(z/scale));
      avgHeight /= 4.0;
      //cout << "avgHeight=" << avgHeight << endl;
      
      return avgHeight;
}

float heightMap::getHeight(float x, float z)
{
	  float totalDistance;
	  vector<float> _distance;//distance from point to each vertex, 
	  _distance.resize(4);//0=low,low 1=low, high 2=high, high 3=high,low
	  
      if(x > width*scale || x<0.0*scale || z>length*scale || z<0.0*scale)
        return 0.0;
      float max=sqrt(2.0);

	  
	  _distance[0]=max-distance( floor(x/scale), floor(z/scale), x/scale, z/scale );
	  _distance[1]=max-distance( floor(x/scale), ceil(z/scale), x/scale, z/scale );
	  _distance[2]=max-distance( ceil(x/scale), ceil(z/scale), x/scale, z/scale );
	  _distance[3]=max-distance( ceil(x/scale), floor(z/scale), x/scale, z/scale );
	  
	  totalDistance=_distance[0]+_distance[1]+_distance[2]+_distance[3];
	  
	  int i=0;
	  for(i=0; i<4; i++)
			_distance[i]/=totalDistance;

      //average height of surronding vertices
      float avgHeight;
      avgHeight = getHeight((int) floor(x/scale),(int) floor(z/scale))*_distance[0];
      avgHeight += getHeight((int) floor(x/scale),(int) ceil(z/scale))*_distance[1];
      avgHeight += getHeight((int) ceil(x/scale),(int) ceil(z/scale))*_distance[2];
      avgHeight += getHeight((int) ceil(x/scale),(int) floor(z/scale))*_distance[3];
      //cout << "avgHeight=" << avgHeight << endl;
      
      return avgHeight*scale;
}
float heightMap::getHeight(int x, int z)
{
      if(x > width || x<-width || z>length || z<-length)
       return 0.0;
       
      return vertices[x*length+z][1];
}

float heightMap::distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z )
{
	float x_comp, y_comp;
	x_comp=pointA_X-pointB_X;
	y_comp=pointA_Z-pointB_Z;
	return sqrt(x_comp*x_comp + y_comp*y_comp);
}

bool heightMap::mapCollision(TVec3<float> from, float h_angle, float v_angle, float maxDist)
{


	TVec3<float> temp;
	temp[0]=-cos(h_angle*M_PI/180.0);
	temp[1]=sin(v_angle*M_PI/180.0);
	temp[2]=-sin(h_angle*M_PI/180.0);
	
	float y=from[1];
	float x=from[0];
	float z=from[2];

	while(x>=0.0&&x<=width*scale&&z>=0.0&&z<=length*scale)
	{
		y+=temp[1];
		x+=temp[0];
		z+=temp[2];
		if(y<=getHeight(x, z))
		{
			if(distance(from[0], from[2], x, z)<maxDist)
			{
				bulletHit(x,z, 0.5);
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

void heightMap::bulletHit(float x, float z, float strength)
{
	
	  float totalDistance;
	  vector<float> _distance;//distance from point to each vertex, 
	  _distance.resize(4);//0=low,low 1=low, high 2=high, high 3=high,low
	  
      float max=sqrt(2.0);

	  
	  _distance[0]=max-distance( floor(x/scale), floor(z/scale), x/scale, z/scale );
	  _distance[1]=max-distance( floor(x/scale), ceil(z/scale), x/scale, z/scale );
	  _distance[2]=max-distance( ceil(x/scale), ceil(z/scale), x/scale, z/scale );
	  _distance[3]=max-distance( ceil(x/scale), floor(z/scale), x/scale, z/scale );


	  totalDistance=_distance[0]+_distance[1]+_distance[2]+_distance[3];
	  
	  int i=0;
	  for(i=0; i<4; i++)
			_distance[i]/=totalDistance;

      //average height of surronding vertices
	  if(floor(x/scale)>0&&floor(x/scale)<length*scale&&
		floor(z/scale)>0&&floor(z/scale)<length*scale)
	      vertices[(int) floor(x/scale)*length+(int) floor(z/scale)]-=strength*_distance[0];
      
	  if(floor(x/scale)>0&&floor(x/scale)<length*scale&&
		ceil(z/scale)>0&&ceil(z/scale)<length*scale)
			vertices[(int) floor(x/scale)*length+(int) ceil(z/scale)]-=strength*_distance[1];
      
	  if(ceil(x/scale)>0&&ceil(x/scale)<length*scale&&
		ceil(z/scale)>0&&ceil(z/scale)<length*scale)
			vertices[(int) ceil(x/scale)*length+(int) ceil(z/scale)]-=strength*_distance[2];
      
	  if(ceil(x/scale)>0&&ceil(x/scale)<length*scale&&
		floor(z/scale)>0&&floor(z/scale)<length*scale)
		  vertices[(int) ceil(x/scale)*length+(int) floor(z/scale)]-=strength*_distance[3];
	  
	  calculateNormals();
}

void heightMap::addHeight(float x, float z)
{
	float totalDistance;
	  vector<float> _distance;//distance from point to each vertex, 
	  _distance.resize(4);//0=low,low 1=low, high 2=high, high 3=high,low
	  
      if(x > width*scale || x<0.0*scale || z>length*scale || z<0.0*scale)
        return;

      float max=sqrt(2.0);

	  
	  _distance[0]=max-distance( floor(x/scale), floor(z/scale), x/scale, z/scale );
	  _distance[1]=max-distance( floor(x/scale), ceil(z/scale), x/scale, z/scale );
	  _distance[2]=max-distance( ceil(x/scale), ceil(z/scale), x/scale, z/scale );
	  _distance[3]=max-distance( ceil(x/scale), floor(z/scale), x/scale, z/scale );
	  
	  totalDistance=_distance[0]+_distance[1]+_distance[2]+_distance[3];
	  
	  int i=0;
	  for(i=0; i<4; i++)
			_distance[i]/=(totalDistance*scale*scale*scale);

      //average height of surronding vertices
 
	  vertices[ ( (int) floor(x/scale) )*length + ( (int) floor(z/scale) ) ][1] += _distance[0];
      vertices[ ( (int) floor(x/scale) )*length + ( (int) ceil(z/scale) ) ][1] += _distance[1];
      vertices[ ( (int) ceil(x/scale) )*length + ( (int) ceil(z/scale) ) ][1] += _distance[2];
      vertices[ ( (int) ceil(x/scale) )*length + ( (int) floor(z/scale) ) ][1] += _distance[3];
}
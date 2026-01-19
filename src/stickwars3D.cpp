/************************************************************************

  This is a simple program which demonstrates the use of the MxGUI
  minimalist GUI framework.  The application presents a window with a
  rotating square that can be moved around with the mouse.  An image
  will be painted on the square via OpenGL texture mapping.

  by Michael Garland, 2003.
  
  $Id: planets.cxx,v 1.3 2004/02/03 20:08:26 garland Exp $

 ************************************************************************/

// Include the standard libgfx headers that we need
#include <gfx/gui.h>         // MxGUI framework
#include <gfx/gltools.h>     // Include the OpenGL headers + handy utilities

#include <gfx/raster.h>      // Support for reading/writing image files
#include <gfx/Vec3.h> 
#include <gfx/Vec4.h> 
#include <time.h>
#include <GL/glut.h>
#include <time.h>
#include <windows.h>
#include <string>

// We'll also want to make use of FLTK's file selector widget
#include <FL/Fl_File_Chooser.H>
#include "myMesh.h"
#include "myMath.h"
#include "mesh.h"
#include "heightMap.h"
#include "AI.h"
#include "robot.h"
#include "robotMesh.h"
#include "wavebox.h"
#include "bitmap_fonts.h"


//#include "./robot/robot.h"
//#include <GL/glut.h>

//#include <GL/glext.h> // Your local header file
#include "glext.h"

// GL_ARB_point_parameters
PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB  = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;

float g_x, g_y, g_z;

typedef struct
{
  TVec3<float> pos;
  float x,y,z;
  float speed;
  TVec3<float> velocity;
  TVec3<float> accel;
  TVec3<float> gravity;
  float mass;
  int life;
  float color[4];
  float radius;
  float h_angle;
  float v_angle;
  float rot;
  bool dead;
  
} coord;

typedef struct
{
  TVec3<float> pos;
  float speed;
  TVec3<float> velocity;
  TVec3<float> accel;
  TVec3<float> gravity;
  int life;
  TVec4<float> color;
  float rot;
  bool dead;
  int type;
  float radius;
  
} particle;

    GLuint      m_textureID;
   
using namespace std;

//game variables

float teamSize=7;
float numTeams=2; 
int respawn=0;
int maxMissles=100;
int numMissles=0;
int MAP_X=100;
int MAP_Z=100;
int MAP_SCALE=10;
int TEAM_KILLS[2];
int PARTICLE_COUNT = 2000;
int YOURKILLS;
bool END_GAME;
int STAT_COUNT[2];


enum SOUNDS {KNIFE, HANDGUN, RIFLE, RIFLE2, M4, ROCKET, EXPLODE, REGROUP, GO, 
			COVERME, MOVEOUT, ROGERTHAT, 
			KILLING}; 




    //Lighting variables
	GLfloat terrain_diffuse[] = { 0.2f, 0.4f, 0.4f, 1.0f };
   	
   	GLfloat mat_diffuse[] = { 0.2f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };

    GLfloat lm_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };

	//GLfloat light0_ambient[] = { 0.48, 0.51f, 0.62f, 1.0f };
	//GLfloat light0_diffuse[] = { 0.48, 0.51f, 0.62f, 1.0f };
	//GLfloat light0_specular[] = { 0.48, 0.51f, 0.62f, 1.0f };
	GLfloat light0_position[] = { 500.0, 500.0, 500.0, 1.0 };

	
	GLfloat light1_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light1_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light1_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light1_position[] = { 0.0f, 6.0f, 0.0f, 1.0f };
	GLfloat light1_direction[] = { 1.0f, 0.0f, 0.0f };
	GLfloat light1_exponent =  50.0f;
	GLfloat light1_cutoff =  180.0f;

	//////FIELD LIGHTS///////////////
	GLfloat light4_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat light4_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light4_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	//GLfloat light4_direction[] = { 0.0f, -1.0f, 0.0f };
	//GLfloat light4_exponent =  50.0f;
	//GLfloat light4_cutoff =  180.0f;
	
	GLfloat light5_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat light5_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light5_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
//	GLfloat light5_direction[] = { 0.0f, -1.0f, 0.0f };
//	GLfloat light5_exponent =  50.0f;
//	GLfloat light5_cutoff =  180.0f;
	
	GLfloat light6_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat light6_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light6_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
//	GLfloat light6_direction[] = { 0.0f, -1.0f, 0.0f };
//	GLfloat light6_exponent =  50.0f;
//	GLfloat light6_cutoff =  180.0f;

	////////////////////////////////////////////////////S


	GLfloat light2_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat light2_diffuse[] = { 0.2f, 0.4f, 0.4f, 1.0f };
	GLfloat light2_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light2_position[] = { 0.0f, 500.0f, 0.0f, 1.0f };
	GLfloat light2_direction[] = { 1.0f, -1.0f, 1.0f };
	GLfloat light2_exponent =  100.0f;
	GLfloat light2_cutoff =  5.0f;

	GLfloat light3_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat light3_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light3_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light3_position[] = { 500.0, 1000.0 , 500.0, 1.0f };
	GLfloat light3_direction[] = { 0.0f, -1.0f, 0.0f };
	GLfloat light3_exponent =  1.0f;
	GLfloat light3_cutoff =  90.0f;
	


 
void unit_square();
float getRandomMinMax( float fMin, float fMax );
Vec3f getRandomVector( void );


 
// The libgfx library provides a minimalist GUI framework encapsulated in the
// MxGUI class.  To use this framework, you simply construct a subclass
// derived from MxGUI, overriding selected virtual functions.
class GUI : public MxGUI
{
public:
    // Here we list the variables that our program uses.
    
	float h_angle, v_angle, opt_theta, center[2];

    bool dragging;
    bool fog; //New Added
	bool built; //new
	bool shadowFlag;
	int tick;
	//meshes
	//scene

	CWaveBox w;


	heightMap *hmap;
	
	robotMesh robot;

	

	//hands and items
	myMesh rightHand;
	myMesh leftHand;
	myMesh rightHandFlashlight;
	myMesh rightHandHammer;
	myMesh rightHandKnife;
	myMesh rightHandBat;
	myMesh rightHandGun;
	myMesh rightHandUzi;
	myMesh rightHandRifle;
	myMesh leftHandUzi;
	myMesh rightHandSniper;
	myMesh bazooka;
	myMesh tree;

	Mesh missle;
	Mesh rock;
	


	vector<AI> trees;

	vector<AI> players;
	//vector <Robot> rbot;

	//Robot rbot;

	vector<keyFrame> kF;
    
	

    //Camera Variables
    TVec3<float> lookFrom;
    TVec3<float> lookAt;
    TVec3<float> up;
	TVec3<float> cam;

	//for mouse functions
	TVec2<int> oldMouse;


	bool background;	//turn scene off if too slow
	bool grow;			//make teh map grow
		
	bool leftHandOut;
	bool rightHandOut;
	bool flashlightOut;
	bool hammerOut;
	bool knifeOut;
	bool batOut;
	bool gunOut;
	bool uziOut;
	bool dualUzi;
	bool rifleOut;
	bool sniperOut;
	bool bazookaOut;
	bool hud;
	bool helpScreen;
	bool quake_particles;
	bool lockHeight;
	bool rocket_window;

	bool scopeOn;

	
	
	int numOfClaps;


	float hitBox;
	
	

    float zoom;
	float aspect;
	int wheel;
	
	float walk_speed;
	ByteRaster *grass; //New Added
	ByteRaster *s0;
	ByteRaster *s1;
	ByteRaster *s2;
	ByteRaster *s3;
	ByteRaster *s4;

	vector<coord> missles;
	vector<particle> particles;
	vector<particle> debris;



public:
    // Constructor and destructor definitions.
    GUI();
    virtual ~GUI();

    // We override the MxGUI initialize() method so that we can load the
    // texture image.  
    void initialize(int argc, char **argv);

public:
    // These are the event handlers, defined by MxGUI, that we override here.
    // See the libgfx documentation, and the comments below, for information
    // on when they are called.

    virtual void setup_for_drawing();
    virtual void draw_contents();
    virtual void update_animation();

    virtual bool mouse_down(int *where, int which);
    virtual bool mouse_up(int *where, int which);
    virtual bool mouse_drag(int *where, int *last, int which);

    virtual bool key_press(int key);
	void GUI::skybox();
	void GUI::draw_yourself();
	void GUI::Reset();
	void GUI::draw_all();
	void GUI::draw_min();
	void GUI::weaponsOff();
};

GUI::GUI()
{

	STAT_COUNT[0]=0;
	STAT_COUNT[1]=0;

	hmap=new heightMap;
	//cow=new myMesh;

	int x=0;// for for loops
	int y=0;
	
	srand ( time(NULL) );	//seed random
    // It's a good idea to always initialize member variables to
    // reasonable values in the class constructor.
    opt_theta = 10.0f;
    v_angle =  0.0f;
	h_angle = 215.0f;
    dragging = false;
    center[0] = center[1] = 50.0f;

	background=true;	//have it on initially

	grow=false;		//dont grow map at first
	shadowFlag=true;

	//hand stuff not out
	weaponsOff();
	leftHandOut = false;
	rightHandOut = false;

	//menu items
	hud = true;
	helpScreen = false;
	quake_particles = true;
	lockHeight = true;

	hitBox=1.5;

	scopeOn=false;

	

	numOfClaps=0;

	//Set Up Keyframes
	kF.resize(3);
	for(x=0; x<kF.size(); x++)
	{
		kF[x].on=false;
		kF[x].frame=0;
		kF[x].count=0;
		for(y=0; y<3; y++)
		{
			kF[x].translate[y]=0.0;
			kF[x].angle[y]=0.0;
		}
	}

	
    
    //Camera Setup
    lookFrom[0] = 10.0;
    lookFrom[1] = 10.0;
    lookFrom[2] = 10.0;
    lookAt[0] = 0.0;
    lookAt[1] = 10.0;
    lookAt[2] = 0.0;
    
    up[0] = 0.0;
    up[1] = 1.0;
    up[2] = 0.0;
    
	cam[0] =10.0;
    cam[1] =10.0;
	cam[2] =10.0;

	zoom = 60.0f;
	aspect=1.0f;
	wheel=0;
	
	walk_speed=2.0;		//lower=faster

	//set up teams

	Reset();

	fog = false; //new added
	built = false; //new
	rocket_window = true;

	//hmap=new heightMap;

	//r = new Robot;

	missles.resize(0);
	particles.resize(0);
	debris.resize(0);
	tick=0;


	
}

GUI::~GUI()
{

	delete grass;
	grass = NULL;
	delete s0;
	s0 = NULL;
	delete s1;
	s1 = NULL;
	delete s2;
	s2 = NULL;
	delete s3;
	s3 = NULL;
	delete s4;
	s4 = NULL;


	delete hmap;
	hmap=NULL;
	//delete r;
	//r = NULL;



}

static void load_texture(ByteRaster *img)
{
    if( img )
    {
        glEnable(GL_TEXTURE_2D);

        // Select an appropriate texture mode.  Experiment with the
        // options other than DECAL.
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//NewAdded

        // Standard boilerplate stuff.  You probably won't need to
        // change this.
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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

// This procedure is called everytime the OpenGL context of the window needs
// to be initialized.  For the most part, this occurs when the window is first
// created and whenever it is resized.
void GUI::setup_for_drawing()
{


	//FIRST
	//initialize the heightmap
	//we need the size and scale for lighting
	hmap->scale=MAP_SCALE;
	hmap->initHeightMap(MAP_X, MAP_Z);

	GLfloat light4_position[] = { 50.0f, 500.0, hmap->getWidth()-50.0f, 1.0 };
	GLfloat light5_position[] = { hmap->getLength()-50.0f, 500.0, hmap->getWidth()-50.0f, 1.0 };
	GLfloat light6_position[] = { hmap->getLength()-50.0f, 500.0, 50.0, 1.0 };


    ///////////////////
   // Setup Lighting
   ///////////////////
	//glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	
   // glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
   // glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
   
    
	glLightfv(GL_LIGHT1, GL_AMBIENT,		light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,		light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR,		light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION,		light1_position);
	glLightf (GL_LIGHT1, GL_SPOT_CUTOFF,	10.0);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
    glLightf (GL_LIGHT1, GL_SPOT_EXPONENT,	100.0);

    //glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
    //glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
    //glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2);


	glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
    //glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, light2_cutoff);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_direction);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, light2_exponent);

   	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glLightfv(GL_LIGHT4, GL_AMBIENT,		light4_ambient);
    glLightfv(GL_LIGHT4, GL_DIFFUSE,		light4_diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR,		light4_specular);
    glLightfv(GL_LIGHT4, GL_POSITION,		light4_position);
	//glLightf (GL_LIGHT4, GL_SPOT_CUTOFF,	30.0);
    //glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, light4_direction);
    //glLightf (GL_LIGHT4, GL_SPOT_EXPONENT,	100.0);
	//glLightf (GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1.5);

	glLightfv(GL_LIGHT5, GL_AMBIENT,		light5_ambient);
    glLightfv(GL_LIGHT5, GL_DIFFUSE,		light5_diffuse);
    glLightfv(GL_LIGHT5, GL_SPECULAR,		light5_specular);
    glLightfv(GL_LIGHT5, GL_POSITION,		light5_position);
	//glLightf (GL_LIGHT5, GL_SPOT_CUTOFF,	60.0);
    //glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, light5_direction);
    //glLightf (GL_LIGHT5, GL_SPOT_EXPONENT,	100.0);

	glLightfv(GL_LIGHT6, GL_AMBIENT,		light6_ambient);
    glLightfv(GL_LIGHT6, GL_DIFFUSE,		light6_diffuse);
    glLightfv(GL_LIGHT6, GL_SPECULAR,		light6_specular);
    glLightfv(GL_LIGHT6, GL_POSITION,		light6_position);
	//glLightf (GL_LIGHT6, GL_SPOT_CUTOFF,	90.0);
   // glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, light6_direction);
    //glLightf (GL_LIGHT6, GL_SPOT_EXPONENT,	100.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	glEnable(GL_LIGHT6);
	

   //glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
   
   
	glShadeModel(GL_SMOOTH);
	
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	
    // We'll make the background a mid-level gray
    glClearColor(0.65f, 0.65f, 0.65f, 0.0f);
    
    //Depth Testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // And we'll set up standard alpha compositing.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    float aspect = (float)canvas->h() / (float)canvas->w();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, aspect, 0.1, 100000.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(lookFrom[0], lookFrom[1], lookFrom[2], lookAt[0], lookAt[1], lookAt[2], 0.0, 1.0, 0.0);
	
    animate(true);
	glEnable(GL_NORMALIZE);


	if(!robot.built)
		robot.DL();
	
	if(!built){
		built = !built;
		
		robot.import_keyframes_ascii("./animations/running.kfm", 0);
		robot.import_keyframes_ascii("./animations/chop.kfm", 1);
		robot.import_keyframes_ascii("./animations/sword.kfm", 2);
		robot.import_keyframes_ascii("./animations/explode.kfm", 3);
		robot.import_keyframes_ascii("./animations/die.kfm", 4);
		robot.import_keyframes_ascii("./animations/stab.kfm", 5);
		
		//get the playerss
		//cow->loadObj("./obj/cow.obj");
		//camel.loadObj("./obj/camel.obj");
		//spaceShip.loadObj("./obj/spaceShip.obj");

		//get the other stuff
	
		rightHand.loadObj("./obj/rightHand.obj");//its.... YEP! a right hand
		//rightHand.createDL();

		leftHand.loadObj("./obj/leftHand.obj");//its.... YEP! a left hand
		//leftHand.createDL();

		rightHandFlashlight.loadObj("./obj/rightHandFlashlight.obj");//flashlight
		//rightHandFlashlight.createDL();

		rightHandHammer.loadObj("./obj/rightHandHammer.obj");
	//	rightHandHammer.createDL();

		rightHandKnife.loadObj("./obj/rightHandKnife.obj");
		//rightHandKnife.createDL();

		rightHandBat.loadObj("./obj/rightHandBat.obj");
		//rightHandBat.createDL();

		rightHandGun.loadObj("./obj/rightHandGun.obj");
		//rightHandGun.createDL();

		rightHandUzi.loadObj("./obj/rightHandUzi.obj");
		//rightHandUzi.createDL();

		leftHandUzi.loadObj("./obj/leftHandUzi.obj");
		//leftHandUzi.createDL();

		rightHandRifle.loadObj("./obj/rightHandRifle.obj");
		//rightHandRifle.createDL();

		rightHandSniper.loadObj("./obj/rightHandSniper.obj");
		//rightHandSniper.createDL();
		bazooka.loadObj("./obj/bazooka.obj");

		missle.loadObj("missle.obj");
		missle.createDL();

		rock.loadObj("./obj/rock.obj");
		rock.createDL();

	}

	//Setup the address for the required function
	glPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
    glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");



    
}



// This procedure is called whenever the contents of the drawing canvas need
// to be redrawn.  This may be for a variety of reasons, including when the
// canvas has been covered by another window that has now been removed or in
// response to an animation update.
void GUI::draw_contents()
{
   
    
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glViewport(0, 0, canvas->w(), canvas->h());
	draw_all();


	//////////////////////
	// DRAW ROCKET WINDOW
	//////////////////////////

	if(rocket_window)									// If We Are Drawing The Second Scene
	{
		
			glDisable(GL_DEPTH_TEST);
			glScissor(0, 0, 128, 128);
			glEnable(GL_SCISSOR_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode (GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity ();							// Reset The Projection Matrix
			glViewport(0, 0, 128, 128);
			
			// Set The Viewport To The Top Right.  It Will Take Up Half The Screen Width And Height
			glMatrixMode (GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity ();							// Reset The Projection Matrix
			// Set Up Perspective Mode To Fit 1/4 The Screen (Size Of A Viewport)
			gluPerspective( 45.0, (float)canvas->h() / (float)canvas->w(), 0.1f, 5000.0 );
			glMatrixMode (GL_MODELVIEW);

			if(missles.size() > 0 ){
				gluLookAt(missles[0].pos[0]+missles[0].velocity[0]*0.5f, missles[0].pos[1],missles[0].pos[2]+missles[0].velocity[2]*0.5f, 
					missles[0].pos[0]+missles[0].velocity[0], missles[0].pos[1]+(missles[0].velocity[1]), missles[0].pos[2]+missles[0].velocity[2], 
					0.0, 1.0, 0.0);
			}else{
				gluLookAt(lookFrom[0], lookFrom[1], lookFrom[2], lookAt[0], lookAt[1], lookAt[2], 0.0, 1.0, 0.0);
			}
				draw_min();
				glDisable(GL_SCISSOR_TEST);
				glEnable(GL_DEPTH_TEST);
			
	}

}


// This method is called when animation is activated, and the canvas needs to
// be updated for the next frame.  The variable gui.default_fps determines how
// many times per second this method will be called.
//
// The GUI framework will call draw_contents() after this procedure finishes.
// You should NEVER call draw_contents() yourself.
void GUI::update_animation()
{


	int x=0;		//for loops
	int y=0;
	int i;
	TVec3<float> dir;//temp vars for calculations
	TVec3<float> angle;	

	int randVal;

	if(players.size()>0)
	{
		tick++;
		STAT_COUNT[0]++;
		if(STAT_COUNT[0]>30)
		{
			STAT_COUNT[0]=0;
			STAT_COUNT[1]++;
			if( STAT_COUNT[1] >= players.size() )
				STAT_COUNT[1]=0;
		}
	}

	respawn--;//count down to spawn

	if(respawn<=0.)
	{
		for(i=0; i<players.size(); i++)
			if(players[i].dead)
				players[i].respawn(&robot, hmap);
		respawn=500;
	}

	///////////////////////
	//CHECK MISSLES
	//////////////////////
	for( i = 0 ; i < missles.size() ; i++)
	{
		//missles[i].velocity += missles[i].gravity;
		if(!missles[i].dead)
		{
			missles[i].pos += missles[i].velocity;
			
			////////////////////////
			//AddSmoke
			////////////////////////
			particle p;

			p.velocity = Vec3f((float) rand()/RAND_MAX/10.0f, 0.2f, (float) rand()/RAND_MAX/10.0f);
			p.gravity = Vec3f(0.0f, 0.0f, 0.0f);
			p.pos = missles[i].pos;
			p.dead = false;
			p.rot = 0.0f;
			p.life = rand()%30+30;
			//p.color = TVec4<float>(1.0f, rand()%2 - 0.5, 0.0f, 1.0f);
			p.color = TVec4<float>(1.0f, 1.0f, 1.0f, 1.0f);

			for(float offset=0.0f; offset<10.0f; offset+=1.0f)
			{
				p.pos = missles[i].pos - (missles[i].velocity/10.0f)*offset;
				p.pos[0]+=(float)(rand()%3);
				p.pos[1]+=(float)(rand()%3);
				p.pos[2]+=(float)(rand()%3);

				particles.push_back(p);
			}
			//particles.push_back(p);
			//particles.push_back(p);
			////////////////////////////

			//Check for collision
			if(missles[i].pos[1] < hmap->getHeight(missles[i].pos[0], missles[i].pos[2]))
			{
			

				////////////////////////
				//Add Debris Explosion
				///////////////////////
				particle p;
				
				p.velocity = Vec3f((float) rand()/RAND_MAX-0.5f, 1.5f, (float) rand()/RAND_MAX-0.5f);
				p.gravity = Vec3f(0.0f, -0.0098f, 0.0f);
				
				p.dead = false;
				p.rot = 0.0f;
				p.life = rand()%75+70;
				p.color = TVec4<float>(1.0f, rand()%2 - 0.5, 0.0f, 1.0f);
				p.radius = (float) rand()/RAND_MAX;
				//p.color = TVec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
				for( offset=0.0f; offset<1000.0f; offset+=1.0f)
				{
					p.color = TVec4<float>(1.0f, rand()%2 - 0.5, 0.0f, 1.0f);
					p.pos = missles[i].pos;
					//p.velocity = Vec3f((float) rand()/RAND_MAX-0.5f, (float) rand()/RAND_MAX, (float) rand()/RAND_MAX-0.5f);
					p.velocity = getRandomVector();
					//p.radius = 1.0f+(float) rand()/RAND_MAX;
					//p.pos = missles[i].pos - (missles[i].velocity/10.0f)*offset;
					//p.pos[0]+=(float)(rand()%5+5);
					//p.pos[1] = hmap->getHeight(missles[i].pos[0], missles[i].pos[2]);
					//p.pos[2]+=(float)(rand()%5+5);
					p.pos[0] +=(float)(rand()%10-5);
					p.pos[2] +=(float)(rand()%10-5);
					p.pos[1] = hmap->getHeight(p.pos[0], p.pos[2]);
				
					debris.push_back(p);
				}
				//
				////////////////////////////

				randVal=(float)(rand()%50+50)/hmap->scale;
				if(randVal<3.0)
					randVal=3.0;
				hmap->explodeHill( randVal, missles[i].pos[0]/hmap->scale, missles[i].pos[2]/hmap->scale);
				missles[i].dead = true;
				w.Play(EXPLODE);
				numMissles--;

				
				//Explode players
				for(x=0; x<players.size(); x++)
				{
					if( players[x].explode(missles[i].pos, 
						randVal*hmap->scale, hmap, &robot) )
						YOURKILLS++;
				}

			
			}

			//out of bounds
			if(missles[i].pos[0] < 0.0f || missles[i].pos[2] < 0.0f
				|| missles[i].pos[0] >= hmap->getWidth() 
				|| missles[i].pos[2] >= hmap->getLength() )
			{
				missles[i].dead=true;
				numMissles--;
			}
		}

	}
	//dequeue
	if(missles.size()>0)
	{
		if(missles[missles.size()-1].dead)
			missles.pop_back();

	}
	
	///////////////////////END MISSLES//////////////////////////////


	////////////////////////
	//Update Smoke Particles
	////////////////////////
	for( i = 0 ; i < particles.size() ; i++)
	{
		//
		//Update Paticles
		particles[i].life--;
		particles[i].pos += particles[i].velocity;
		particles[i].velocity += particles[i].gravity;
		particles[i].color[4] /= 1.1f;
		//particles[i].rot += (float) rand()/RAND_MAX;

		/*if(!particles[i].dead)
		{
			
		}*/
		//Good Riddens to them.
		if(particles[i].pos[0] < 0.0f || particles[i].pos[2] < 0.0f
				|| particles[i].pos[0] >= hmap->getWidth() 
				|| particles[i].pos[2] >= hmap->getLength() 
				|| particles[i].life <= 0 
				|| particles[i].pos[1] < hmap->getHeight(particles[i].pos[0], particles[i].pos[2]) )
			{
				particles[i].dead=true;

			}

		if(particles.size()>0)
		{
			if(particles[particles.size()-1].dead)
				particles.pop_back();
		}

	}

	//////////////////////////
	//Update Debris Particles
	//////////////////////////
	for( i = 0 ; i < debris.size() ; i++)
	{
		//
		//Update Paticles
		debris[i].life--;
		debris[i].pos += debris[i].velocity;
		debris[i].velocity += debris[i].gravity;
		//debris[i].color[4] /= 1.1f;
		//particles[i].rot += (float) rand()/RAND_MAX;

		/*if(!particles[i].dead)
		{
			
		}*/
		//Good Riddens to them.
		if(debris[i].pos[0] < 0.0f || debris[i].pos[2] < 0.0f
				|| debris[i].pos[0] >= hmap->getWidth() 
				|| debris[i].pos[2] >= hmap->getLength() 
				|| debris[i].life <= 0 
				|| debris[i].pos[1] < hmap->getHeight(debris[i].pos[0], debris[i].pos[2]) )
			{
				debris[i].dead=true;

			}

		if(debris.size()>0)
		{
			if(debris[debris.size()-1].dead)
				debris.pop_back();
		}

	}


	///////////////////END PARTICLES///////////////////////

	//////////////////////////////
	///TERRAIN GROW
	//////////////////////////////
	if(grow)
	{
		randVal=(rand()%(int)(hmap->getX()/5.0)+5);
		if(rand()%10!=0)
			hmap->makeHill(rand()%randVal, rand()%(hmap->getX()-randVal), rand()%(hmap->getZ()-randVal), (float) (rand()%(randVal))+randVal );
		else
			hmap->makePyramid(rand()%randVal, rand()%(hmap->getX()-randVal), rand()%(hmap->getZ()-randVal) );
	}

	///////////////////////END TERRAIN GROW//////////////////////////////


	//gun Shot
	if(kF[2].on)
	{
		switch(kF[2].frame)
		{
			case 0:
				if(rifleOut)
				{
					v_angle+=2.0;
					if(v_angle>90)
						v_angle=90.0;
					kF[2].angle[0]+=3.0;
					kF[2].frame++;
				}
				else if(sniperOut)
				{
					v_angle+=1.0;
					if(v_angle>90)
						v_angle=90.0;
					kF[2].angle[0]+=1.0;
					kF[2].frame++;
				}
				else if(gunOut)
				{
					v_angle+=6.0;
					if(v_angle>90)
						v_angle=90.0;
					kF[2].angle[0]+=6.0;
					kF[2].frame++;
				}
				else if(uziOut||dualUzi)
				{
					v_angle+=1.0;
					if(v_angle>90)
						v_angle=90.0;
					kF[2].angle[0]+=1.0;
					if(!gunOut)
						kF[2].frame++;
				}
				else if(bazookaOut)
				{
					v_angle+=1.0;
					if(v_angle>90)
						v_angle=90.0;
					kF[2].angle[0]+=1.0;
					kF[2].frame++;
				}
				if(dualUzi&&uziOut)
				{
					v_angle+=0.5;
					if(v_angle>90)
						v_angle=90.0;
					kF[2].angle[0]+=0.5;
				}
				break;
			case 5:
				kF[2].angle[0]=0.0;
				kF[2].frame++;
				break;
			case 2:
				if(!uziOut&&!dualUzi)
				{	
					kF[2].frame++;
					break;
				}
			case 15:
				if(!gunOut&&!uziOut&&!dualUzi&&!rifleOut)
				{
					kF[2].frame++;
					break;
				}
			case 25:
				kF[2].on=false;
				kF[2].frame=0;
				kF[2].angle[0]=0.0;
				break;
			default:
				kF[2].frame++;
				break;
		}
	}

	//swing
	if(kF[1].on)
	{
		switch(kF[1].frame)
		{
			case 0:
				kF[1].translate[2]+=2.0;
				if(kF[1].translate[2]>=4.0)
					kF[1].frame++;
				break;
			case 1:
				kF[1].angle[0]+=6.0;
				if(kF[1].angle[0]>=30.0)
				{
					kF[1].frame++;
				}
				break;
			case 2:
				kF[1].angle[0]-=10.0;
				if(kF[1].angle[0]<=-40.0)
				{
					kF[1].frame++;
					if(knifeOut)
						w.Play(KNIFE);//("./sounds/knife_hit1.wav", NULL, SND_ASYNC|SND_FILENAME);
				}
				break;
			case 3:
				kF[1].angle[0]+=6.0;
				if(kF[1].angle[0]>=0.0)
					kF[1].frame++;
				break;
			case 4:
				kF[1].translate[2]-=2.0;
				if(kF[1].translate[2]<=0.0)
					kF[1].frame++;
				break;
			case 5:
				kF[1].on=false;
				kF[1].angle[0]=0.0;
				for(x=0; x<3; x++)
					kF[1].translate[x]=0.0;
				kF[1].frame=0;
				for(x=0; x<players.size(); x++)
				{
					dir[0]=(lookFrom[0]-players[x].pos[0]);
					dir[2]=(lookFrom[2]-players[x].pos[2]);
					angle[1]=atan( dir[2]/dir[0])*180.0/M_PI;
					dir[1]=sqrt(dir[0]*dir[0]+dir[2]*dir[2]);
					
					if(dir[0]<0.0)//quads 3 and 4 need to flip
						angle[1]=angle[1]+180.0;
					
					if(angle[1]>360.0)
						angle[1]-=360.0;
					if(angle[1]<0.0)
						angle[1]+=360.0;
					
					if(dir[1]<=30.0)
					{
						if(abs(h_angle-angle[1])<=180.0/dir[1]&&!players[x].dying)
						{
							//players[x].angle[0]=180.0;
							players[x].dying=true;;
							YOURKILLS++;
						}
					}
				}
				break;
			default:
				break;
		}
	}

	//clap
	if(kF[0].on)
	{
		switch(kF[0].frame)
		{
			case 2:
				if(kF[0].count>=numOfClaps-1)
					kF[0].frame=4;
				else
					kF[0].frame=0;
				kF[0].count++;
			case 0:
				kF[0].translate[0]+=2.0;
				if(kF[0].translate[0]>=20.0)
					kF[0].frame++;
				break;
			case 1:
			case 3:
				kF[0].translate[0]-=2.0;
				if(kF[0].translate[0]<=5.0)
					kF[0].frame++;
				break;
			case 4:
			default:
				kF[0].translate[0]=0.0;
				kF[0].frame=0;
				kF[0].on=false;
				kF[0].count=0;
			   for( x=0; x<players.size(); x++)
				{
				   players[x].angle[0]=0.0;
				   players[x].death.frame=0;
				   players[x].walking=true;
				   players[x].dead=false;
				   players[x].dying=false;

					for(y=0; y<3; y++)
					{
						//reset death animations while we're at it
						players[x].death.angle[y]=0.0;
						players[x].death.translate[y]=0.0;
						players[x].dead=false;
					}
					players[x].setGoal(hmap);
					break;
						
			   }
			    w.Play(ROGERTHAT);//PlaySound("./sounds/roger_that.wav", NULL, SND_ASYNC|SND_FILENAME);
				break;
		}
	}

	//set up for flag colors
	for(i=0; i<hmap->_flag.size(); i++)
		for(x=0; x<numTeams; x++)
			hmap->_flag[i].teamCap[x]=0;

	//move the players
	for(x=0; x<players.size(); x++)
	{
		players[x].move(hmap);
		if(!players[x].dying&&!players[x].dead)
		{
			//are we fighting
			if(players[x].target>=0)
			{
				players[x].fight( &players[ players[x].target ], &robot, TEAM_KILLS);
			}
			else if(!players[x].dying)//look for enemies
			{
				if(x<teamSize)
				{
					for(i=teamSize; i<players.size(); i++)
						if(!players[i].dead && !players[i].dying)
							players[x].threat(&players[i], i, &robot);
				}
				else
				{
					for(i=0; i<teamSize; i++)
						if(!players[i].dead && !players[i].dying)
							players[x].threat(&players[i], i, &robot);
				}
			}			
		}
	}


	hmap->teamFlag[0]=0;
	hmap->teamFlag[1]=0;

	//set flag color
	for(i=0; i<hmap->_flag.size(); i++)
	{
		if(hmap->_flag[i].teamCap[0] > hmap->_flag[i].teamCap[1])
			hmap->_flag[i].team=1;
		else if(hmap->_flag[i].teamCap[0] < hmap->_flag[i].teamCap[1])
			hmap->_flag[i].team=2;
		else if(hmap->_flag[i].teamCap[0] > 0 &&
			hmap->_flag[i].teamCap[1] > 0)
			hmap->_flag[i].team=0;
		
		if(hmap->_flag[i].team>0)		//give them the flag count
			hmap->teamFlag[hmap->_flag[i].team-1]++;
	}
	

    //key press and hold
    if(Fl::event_key(FL_Shift_L))//run
	{
		walk_speed/=1.5;
		if(walk_speed<=0.3)
			walk_speed=0.3;
	}
	else
		walk_speed=4.0;

	TVec3<float> temp(0.0, 0.0, 0.0);
	float temp2;

    if( Fl::event_key('w') )//walk forward
	{
			temp2=lookAt[1];
			//lookAt[1]=lookFrom[1];;
            temp = lookFrom-lookAt;
			//lookAt[1]=temp2;
            lookAt -= temp/walk_speed;
            lookFrom -= temp/walk_speed;
    }
    if( Fl::event_key('s') )//go backward
	{
			temp2=lookAt[1];
			//lookAt[1]=lookFrom[1];
            temp = lookFrom-lookAt;
			//lookAt[1]=temp2;
			lookAt += temp/walk_speed;
            lookFrom +=temp/walk_speed;
    } 
    if( Fl::event_key('d') )//strafe right
	{
			temp2=lookAt[1];
			lookAt[1]=lookFrom[1];
            temp=(lookAt-lookFrom)^up;
			lookAt[1]=temp2;
            lookAt += temp/walk_speed;
            lookFrom += temp/walk_speed;
    }
    if( Fl::event_key('a') )//strafe left
	{
			temp2=lookAt[1];
			lookAt[1]=lookFrom[1];
            temp=(lookAt-lookFrom)^up;
			lookAt[1]=temp2;
            lookAt-=temp/walk_speed;
            lookFrom-=temp/walk_speed;
    }   


	if( Fl::event_key('i') && zoom<80 && !scopeOn)//zoom out
	{
		zoom++;
	
	}
	if( Fl::event_key('m') && zoom>10 && !scopeOn)//zoom in
	{
		zoom--;
	}

	//Swing
   if(Fl::event_key(' ') && !kF[1].on && rightHandOut &&!gunOut&&!uziOut&&!rifleOut&&!sniperOut&&!dualUzi&&!bazookaOut)
   {
			
	   kF[1].on=true;
   }

   //Gun Shot
   if(Fl::event_key(' ') && (gunOut||uziOut||dualUzi||rifleOut||sniperOut)&&!kF[2].on )
   {
	   if(gunOut)
			w.Play(HANDGUN);//PlaySound("./sounds/fiveseven-1.wav", NULL, SND_ASYNC|SND_FILENAME);
	   else if(rifleOut)
		   w.Play(RIFLE);//PlaySound("./sounds/rifle1.wav", NULL, SND_ASYNC|SND_FILENAME);
	   else if(sniperOut)
			w.Play(RIFLE2);//PlaySound("./sounds/rifle2.wav", NULL, SND_ASYNC|SND_FILENAME);
	   else if(uziOut||dualUzi)
		   w.Play(M4);//PlaySound("./sounds/m4a1-1.wav", NULL, SND_NOSTOP|SND_ASYNC|SND_FILENAME);
	   int closest=-1;
	   float curClose=10000000.0;
		for(x=0; x<players.size(); x++)
		{
			float zMax, xMax;
			
			//side bounds
			dir[0]=(lookFrom[0]-players[x].pos[0]);
			dir[2]=(lookFrom[2]-players[x].pos[2]);
			xMax=atan( dir[2]/dir[0])*180.0/M_PI;
			dir[1]=sqrt(dir[0]*dir[0]+dir[2]*dir[2]);
				
			if(dir[0]<0.0)//quads 3 and 4 need to flip
				xMax=xMax+180.0;
					
			if(xMax>360.0)
				xMax-=360.0;
			if(xMax<0.0)
				xMax+=360.0;

			//top/bottom bounds
			
			dir[0]=(lookFrom[1]-players[x].pos[1]);//z height
			zMax=atan( dir[0]/dir[1])*180.0/M_PI;
		
			if(dir[1]<0.0)//quads 3 and 4 need to flip
			zMax=zMax+180.0;

			if(dir[1]==0.0)
				dir[1]=0.1;
			

			if(abs(h_angle-xMax)<=180.0/(dir[1]/hitBox) && !players[x].dying&&!players[x].dead&&dir[1]<curClose)// && abs(angle[2]-xMax)<=10.0)
			{
				//players[x].dying=true;
				if( abs(-v_angle-zMax)<=180.0/(dir[1]/(2.0+hitBox)) )
				{closest=x;
				curClose=dir[1];}
			}
			hitBox=1.00;
		}
		if(hmap->mapCollision(lookFrom, h_angle, v_angle, curClose))
		{}
		else if(closest>=0)
		{
			//PlaySound("bullet_hit1.wav", NULL, SND_NOSTOP|SND_ASYNC|SND_FILENAME);
			players[closest].dying=true;
			YOURKILLS++;
		}
		kF[2].on=true;
   }

   if(Fl::event_key(' ') && bazookaOut && !kF[2].on && numMissles < 2 && missles.size() < 2)
	{// Shoot Missle
			

	    coord cor;

		cor.speed = 2.0f;
		cor.velocity = Vec3f(-cos(h_angle*M_PI/180.0), sin(v_angle*M_PI/180.0), -sin(h_angle*M_PI/180.0));
		cor.velocity *= 10.0f;
		cor.gravity = Vec3f(0.0f, 0.00f, 0.0f);
		cor.mass = 1.0f;
		cor.radius = 3.0f;
		cor.pos = lookFrom;
		cor.pos[0] +=-cos(h_angle*M_PI/180.0)*1.0f;
		cor.pos[2] +=-sin(h_angle*M_PI/180.0)*1.0f;
		cor.h_angle = h_angle;
		cor.v_angle = v_angle;
		cor.dead = false;
		cor.rot = 0.0f;

		missles.push_back(cor);

		numMissles++;

		w.Play(ROCKET);//PlaySound("./sounds/rocket_launcher.wav", NULL, SND_NOSTOP|SND_ASYNC|SND_FILENAME);

		kF[2].on=true;

		//cout << "MISSLES " << numMissles << "QUEUE SIZE " << missles.size() <<  endl;
	}

 


}

static
bool center_on_click(float *ctr, int *where)
{
    double world[3];

    unproject_pixel(where, world);
    ctr[0] = (float)world[0];
    ctr[1] = (float)world[1];

    return true;
}

// This method will be called whenever the user clicks down on a mouse button
// in the OpenGL canvas.  The 'where' argument holds the pixel location of the
// click and the 'which' argument is the number of the button (1, 2, or 3).
//
// The window will be redraw if you return true from this handler, otherwise
// it will be left unchanged.  You should NEVER call draw_contents() yourself.


// This operates in the same manner as mouse_down() but it called when
// the mouse button is released.
bool GUI::mouse_down(int *where, int which)
{
    // At the bottom of the application window is a status bar for displaying
    // text messages.  You can set the text being displayed with the status()
    // method.  Notice that it works exactly like printf().
	if( which==1||which==2||which==3)
    {
		oldMouse[0]=Fl::event_x();
		oldMouse[1]=Fl::event_y();

	dragging = true;
	return true;
    }
    else return false;
}

// This operates in the same manner as mouse_down() but it called when
// the mouse button is released.
bool GUI::mouse_up(int *where, int which)
{

    dragging = false;

    return (which==1||which==2||which==3);
}

// Similar to mouse_down(), this method is called when the user drags the
// mouse with a pointer depressed.  'where' is the current mouse location,
// 'last' is the location of the mouse when the last drag event was generated,
// and 'which' tells you that number of the button that is depressed.
bool GUI::mouse_drag(int *where, int *last, int which)
{
	bool scratchL=false;//are we scratching
	bool scratchR=false;
	float scratches=0.0;//which scratch
     float temp=1.0;
	 if(scopeOn)
		 temp=10.0;
    if( which==1 )
    {
		if(Fl::event_x()-oldMouse[0]!=0)
			h_angle+=(float)(Fl::event_x()-oldMouse[0])/temp;
		if(Fl::event_y()-oldMouse[1]!=0)
			v_angle-=(float)(Fl::event_y()-oldMouse[1])/temp;
		if(v_angle>90.0)
			v_angle=90.0;
		if(v_angle<-90.0)
			v_angle=-90.0;
		if(h_angle>360.0)
			h_angle-=360.0;
		if(h_angle<0.0)
			h_angle+=360.0;
		oldMouse[0]=Fl::event_x();
		oldMouse[1]=Fl::event_y();
		return true;
    }
    else if(which==2)
	{
		if(Fl::event_y()-oldMouse[1]!=0)//fly up
		{
            lookAt[1]-=(Fl::event_y()-oldMouse[1])/10.0;
            lookFrom[1]-=(Fl::event_y()-oldMouse[1])/10.0;
		}
		oldMouse[0]=Fl::event_x();
		oldMouse[1]=Fl::event_y();
		return true;
	}


	else
		return false;
}


// This handler is called whenever a key is pressed.  It should return true if
// the given key press has been handled, and false otherwise.
// 
// NOTE: The "key" value represents the unshifted key value.  For example,
// hitting 'h' and 'Shift-h' will produce the same code.  You must manually
// check whether the Shift, Ctrl, ... keys have been pressed.  See the
// documentation for Fl::event_key() and Fl::event_state() at
// http://www.fltk.org.
bool GUI::key_press(int key)
{

    TVec3<float> temp;
	TVec3<float> dir;
	TVec3<float> angle;
     
   if(Fl::event_key('z')&&!kF[0].on)//take out/put away left hand
   {	   
	   weaponsOff();
	   leftHandOut=!leftHandOut;
   }

   if(Fl::event_key('x')&&!kF[0].on&&!kF[1].on)	//take out/put away right hand
   {
		weaponsOff();	
	   rightHandOut=!rightHandOut;
   }
	 
   switch(key)
   {
	   case 'p':
			grow=!grow;
			break;
       case 'q'://where am i
		   Reset();
			break;

	   case '1':	//hammer
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
			   hammerOut=true;
			}
			break;
	   case '2':	//knife
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
			   knifeOut=true;
			}
			break;
	   case '3':	//bat
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
			   batOut=true;
			}
			break;
	   case '4':	//pistol
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
			   gunOut=true;
			}
			break;
	   case '5'://right hand uzi
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
			   uziOut=true;
			}
			break;
	   case '6'://left hand uzi
			if(!kF[2].on)
			{
				weaponsOff();
				dualUzi=true;
				uziOut=true;
			}
			break;
		case '7'://rifle
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
			   rifleOut=true;
			}
			break;
		case '8'://sniper rifle
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
				sniperOut=true;
			}
			break;
		case '9'://bazooka
			if(!kF[0].on&&!kF[1].on)
			{
				weaponsOff();
				bazookaOut=true;
			}
			break;

	   case 'f':	//flashlight
		   if(!kF[0].on&&!kF[1].on)
		   {
			   weaponsOff();
			   flashlightOut=true;
		   }
		   break;

		case 't':	//flashlight
		   g_x += 0.01f;
		   break;
		case 'y':	//flashlight
		   g_y += 0.01f;
		   break;
		case 'u':	//flashlight
		   g_z += 0.01f;
		   break;
		case 'i':
			cout << g_x << " " << g_y << " " << g_z << endl;
			break;


	   case 'r'://scope it
			if(sniperOut)
			{
				scopeOn=!scopeOn;
				if(scopeOn)
					zoom=7;
				else
					zoom=60;
			}
			break;


	   case 'c':	//single clap

		   if(rightHandOut&&leftHandOut&&!kF[0].on&&!kF[1].on)
			{
			   w.Play(REGROUP);//PlaySound("./sounds/regroup.wav", NULL, SND_NOSTOP|SND_ASYNC|SND_FILENAME);
			   numOfClaps=1;
			   kF[0].on=true;
		   }
		   break;
	   case 'v':	//double clap - scatter
		   if(rightHandOut&&leftHandOut&&!kF[0].on&&!kF[1].on)
		   {
			   w.Play(GO);//PlaySound("./sounds/go.wav", NULL, SND_NOSTOP|SND_ASYNC|SND_FILENAME);
			   numOfClaps=2;
			   kF[0].on=true;
		   }
		   break;
		case 'b':	//triple clap - circle
		   if(rightHandOut&&leftHandOut&&!kF[0].on&&!kF[1].on)
		   {
			   w.Play(COVERME);//PlaySound("./sounds/coverme.wav", NULL, SND_NOSTOP|SND_ASYNC|SND_FILENAME);
			   numOfClaps=3;
			   kF[0].on=true;
		   }
		   break;
		case 'n':	//quad clap - moveout
		   if(rightHandOut&&leftHandOut&&!kF[0].on&&!kF[1].on)
		   {
			   w.Play(MOVEOUT);//PlaySound("./sounds/moveout.wav", NULL, SND_NOSTOP|SND_ASYNC|SND_FILENAME);
			   numOfClaps=4;
			   kF[0].on=true;
		   }
		   break;
		/*case 'g':	//add an players
			if(numOfplayerss < 300)
				numOfplayerss++;
			break;
		case 'h':	//get rid an players
			if(numOfplayerss>0)
				numOfplayerss--;
			break;*/
		case 'k':	//turn on/off background
			background=!background;
			break;
		
		//case 'u':	//turn on/off background
			//hmap->saveObj("terrain.obj");
			//break;
       default:
            return false;
            break;
   }
   return true;
     

	return false;
}


void GUI::initialize(int argc, char **argv)
{
    // First, we need to call the MxGUI initialize() method.  This will
    // have FLTK initialize the window system.  We need to do this
    // because we wouldn't be able to create a file selector otherwise.
    MxGUI::initialize(argc, argv);
	
	grass = read_image("./textures/grounds_low.jpg");//New Added
	s0 = read_image("./textures/SkyBox0.bmp");//New Added
	s1 = read_image("./textures/SkyBox1.bmp");//New Added
	s2 = read_image("./textures/SkyBox2.bmp");//New Added
	s3 = read_image("./textures/SkyBox3.bmp");//New Added
	s4 = read_image("./textures/particle.bmp");//New Added


	w.Load("./sounds/knife_hit1.wav");
    w.Load("./sounds/fiveseven-1.wav");
    w.Load("./sounds/rifle1.wav");
    w.Load("./sounds/rifle2.wav");
    w.Load("./sounds/m4a1-1.wav");
    w.Load("./sounds/rocket_launcher.wav");
    w.Load("./sounds/rocket_explode.wav");
    w.Load("./sounds/regroup.wav");
    w.Load("./sounds/go.wav");
	w.Load("./sounds/coverme.wav");
    w.Load("./sounds/moveout.wav");
	w.Load("./sounds/roger_that.wav");
	w.Load("./sounds/killing.wav");
	//w.Load("./sounds/gentle_wind.wav");
	//w.Load("./sounds/gushing_wind.wav");

}

////////////////////////////////////////////////////////////////////////
//
// Program entry point
//
int main(int argc, char **argv)
{
    GUI gui;
    // This will initialize the GUI and create all the structures representing
    // the windows, but nothing will be shown on the screen yet.
    gui.initialize(argc, argv);
    //
    // We change the default animation rate to 10 frames per second.
    gui.default_fps = 30;
    //
    // And give the window a meaningful name.
    gui.toplevel->label("STICKWARS 3D");
	//gui.resize_canvas(1024, 768);
	gui.resize_canvas(800, 600);
	//gui.lock_size();
	gui.add_toggle_menu("Options/Fog", int('F'), gui.fog, 0); //new added
	gui.add_toggle_menu("Options/shadowFlag", int('G'), gui.shadowFlag, 0); //new added
	gui.add_toggle_menu("Options/Lock Height", int('L'), gui.lockHeight, 0); //new added
	gui.add_toggle_menu("Options/Display HUD", int('H'), gui.hud, 0); //new added
	gui.add_toggle_menu("Options/Display Help", int('?'), gui.helpScreen, 0); //new added
	gui.add_toggle_menu("Window/Rocket Cam", int('R'), gui.rocket_window, 0); //new added
	gui.add_toggle_menu("Options/Quake Paritcles", int('Q'), gui.quake_particles, 0); //new added
	gui.add_toggle_menu("Terrain/Grow Terrain", int('P'), gui.grow, 0); //new added


	gui.canvas->mode(FL_RGB8|FL_DOUBLE|FL_DEPTH|FL_ALPHA);

    // This will actually present the application window on the screen, and
    // begin the event loop.  Note that this function *never* returns.  We
    // only 'return' its value to keep Visual C++ from complaining about
    // main() not returning any value.
    return gui.run();
}


void GUI::skybox()
{

	//Draw Front
	load_texture(s0);
    glBegin(GL_POLYGON);
      glTexCoord2f(0, 0);
      glVertex3f(-0.5, 0, -0.5);

      glTexCoord2f(0, 1);
      glVertex3f(-0.5, 0, 0.5);

      glTexCoord2f(1, 1);
      glVertex3f(0.5, 0, 0.5);

      glTexCoord2f(1, 0);
      glVertex3f(0.5, 0, -0.5);
    glEnd();


	///Draw Back;
	load_texture(s1);
    glBegin(GL_POLYGON);
      glTexCoord2f(0, 0);
      glVertex3f(0.0f, 0, 0.0f);

      glTexCoord2f(0, 1);
      glVertex3f(0.0f, 100.0f, 0.0f);

      glTexCoord2f(1, 1);
      glVertex3f(100.0f, 100.0f, 100.0f);

      glTexCoord2f(1, 0);
      glVertex3f(100.0f, 0.0f, 100.0f);
    glEnd();


	///Draw right

	load_texture(s2);
    glBegin(GL_POLYGON);
      glTexCoord2f(0, 0);
      glVertex3f(-0.5, 0, -0.5);

      glTexCoord2f(0, 1);
      glVertex3f(-0.5, 0, 0.5);

      glTexCoord2f(1, 1);
      glVertex3f(0.5, 0, 0.5);

      glTexCoord2f(1, 0);
      glVertex3f(0.5, 0, -0.5);
    glEnd();

	///Draw left
	load_texture(s3);
    glBegin(GL_POLYGON);
      glTexCoord2f(0, 0);
      glVertex3f(-0.5, 0, -0.5);

      glTexCoord2f(0, 1);
      glVertex3f(-0.5, 0, 0.5);

      glTexCoord2f(1, 1);
      glVertex3f(0.5, 0, 0.5);

      glTexCoord2f(1, 0);
      glVertex3f(0.5, 0, -0.5);
    glEnd();


	//Draw Top
	load_texture(s4);
    glBegin(GL_POLYGON);
      glTexCoord2f(0, 0);
      glVertex3f(-0.5, 0, -0.5);

      glTexCoord2f(0, 1);
      glVertex3f(-0.5, 0, 0.5);

      glTexCoord2f(1, 1);
      glVertex3f(0.5, 0, 0.5);

      glTexCoord2f(1, 0);
      glVertex3f(0.5, 0, -0.5);
    glEnd();
}

void GUI::draw_yourself(){

	glColor3f(0.803921568f, 0.63921568f, 0.56470588f); 
		//Draw the Right Hand
//	if(rightHandOut)
	{
		glPushMatrix();
		glTranslatef(lookFrom[0]-(5.0)*cos((h_angle+(20.0-kF[0].translate[0]))*M_PI/180.0), (lookFrom[1]+4.0*sin(v_angle*M_PI/180.0))-2.0, lookFrom[2]-(5.0)*sin((h_angle+(20.0-kF[0].translate[0]))*M_PI/180.0));
		glRotatef(-h_angle-90.0, 0.0, 1.0, 0.0);
		glRotatef(-v_angle, 1.0, 0.0, 0.0 );
		if (knifeOut&&!kF[1].on)
		{
			glRotatef(35.0, 0.0, 0.0, 1.0);
			glRotatef(-40.0, 0.0, 1.0, 0.0);
			glRotatef(-55.0, 1.0, 0.0, 0.0 );
		}

		if(batOut&&!kF[1].on)
		{
			glRotatef(-20.0, 1.0, 0.0, 0.0);
			glRotatef(10.0, 0.0, 0.0, 1.0);
		}
		else if(batOut&&kF[1].on)
		{
			glRotatef(10.0, 1.0, 0.0, 0.0);
			glRotatef(30.0, 0.0, 0.0, 1.0);
		}

		if(kF[0].on)	//clap
		{
			if(!(!uziOut&&dualUzi))
			{
				glRotatef(-30.0, 1.0, 0.0, 0.0 );
			
				glRotatef(90.0, 0.0, 0.0, 1.0 );
			}
		}
		if(kF[1].on)	//swing
		{
			glRotatef(-kF[1].angle[0], 1.0, 0.0, 0.0);
			glTranslatef(kF[1].translate[0], kF[1].translate[1], kF[1].translate[2]);
		}
		if(kF[2].on )
		{
			glRotatef(-kF[2].angle[0], 1.0, 0.0, 0.0);
		}
		if(flashlightOut)
			rightHandFlashlight.draw();
		else if(hammerOut)
			rightHandHammer.draw();
		else if (knifeOut)
			rightHandKnife.draw();
		else if (batOut)
			rightHandBat.draw();
		else if (gunOut)
			rightHandGun.draw();
		else if (uziOut)
			rightHandUzi.draw();
		else if (rifleOut)
			rightHandRifle.draw();
		else if (sniperOut)
			rightHandSniper.draw();
		else if (bazookaOut)
			bazooka.draw();
		else if(rightHandOut)
			rightHand.draw();
		glPopMatrix();
	}


	//Draw the Left Hand
//	if(leftHandOut&&!rifleOut&&!sniperOut&&!bazookaOut)
	{
		glPushMatrix();
	
		glTranslatef(lookFrom[0]-(5.0)*cos((h_angle-(20.0-kF[0].translate[0]))*M_PI/180.0), (lookFrom[1]+4.0*sin(v_angle*M_PI/180.0))-2.0, lookFrom[2]-(5.0)*sin((h_angle-(20.0-kF[0].translate[0]))*M_PI/180.0));
		if( (gunOut||uziOut) && !dualUzi)
			glTranslatef(0.0, 1.0, 0.0);
		glRotatef(-h_angle-90.0, 0.0, 1.0, 0.0);
		glRotatef(-v_angle, 1.0, 0.0, 0.0 );
		if(kF[0].on)	//clap
		{
			glRotatef(30.0, 1.0, 0.0, 0.0 );
			if(!gunOut&&!uziOut)
				glRotatef(-90.0, 0.0, 0.0, 1.0 );
		}
		if(kF[2].on && dualUzi)
		{
			glRotatef(-kF[2].angle[0], 1.0, 0.0, 0.0);
		}
		if(dualUzi)
			leftHandUzi.draw();
		else if(leftHandOut)
			leftHand.draw();
		glPopMatrix();
	}
}

static void unit_square()
{
    glBegin(GL_POLYGON);
      glTexCoord2f(0, 0);
      glVertex3f(-0.5, 0, -0.5);

      glTexCoord2f(0, 1);
      glVertex3f(-0.5, 0, 0.5);

      glTexCoord2f(1, 1);
      glVertex3f(0.5, 0, 0.5);

      glTexCoord2f(1, 0);
      glVertex3f(0.5, 0, -0.5);
    glEnd();
}

void GUI::Reset()
{
	
	char ans;
	int i, x, y;

	cout<<"Start A New Game? <y/n>"<<endl;
	cin>>ans;
	if(ans!='y')//say no
	{
		for(i=0; i<9; i++)
			hmap->_flag[i].team=0;
		END_GAME=false;
		return;
	}


	cout<<"Team Size? <1-500>"<<endl;
	cin >> teamSize;
	
	cout<<"Map Size? <50-1000>"<<endl;
	cin>>MAP_X;
	MAP_Z=MAP_X;
	
	cout<<"Map Scale? <1-100>"<<endl;
	cin>>MAP_SCALE;

	TEAM_KILLS[0]=0;
	TEAM_KILLS[1]=0;
	
	delete hmap;
	
	hmap=new heightMap;
	
	hmap->scale=MAP_SCALE;


	hmap->initHeightMap(MAP_X, MAP_Z);

	//set up teams

	players.resize(teamSize*numTeams);//how many

	//initialize players
	for(i=0; i<numTeams; i++)
	{
		for(x=0; x<teamSize; x++)
		{	
			if(i==0)
				players[x+teamSize*i].goalFlag=0;
			else
				players[x+teamSize*i].goalFlag=2;

			players[x+teamSize*i].team=i+1;

			if(x<2*teamSize/3)
				players[x+teamSize*i].aiType=0;	//attack
			else
				players[x+teamSize*i].aiType=2; //attack/defenders

			switch(players[x+teamSize*i].team)//set starting position
			{
				case 1://team 1
					players[x+teamSize*i].pos[0]=(float)(rand()%(int)(hmap->getLength()))+50.0f;
					players[x+teamSize*i].pos[2]=(float)(rand()%50);
					break;
				case 2://team 2
					players[x+teamSize*i].pos[0]=(float)(rand()%(int)(hmap->getLength()))+50.0f;
					players[x+teamSize*i].pos[2]=(float)(rand()%50)+hmap->getWidth();
					break;
				default:
					for(y=0; y<3; y++)
						players[x+teamSize*i].pos[y]=(float)(rand()%50)+hmap->getLength()/2.0f;
					break;
			}	
		}
	}
	trees.resize(10);
/*	for(i=0; i<trees.size; i++)
	{
		trees[i].pos[0]=rand*/

	respawn=0;
	YOURKILLS=0;
	END_GAME=false;
	cout<<"GAME HAS COMMENCED!!!"<<endl;
}

//-----------------------------------------------------------------------------
// Name: getRandomMinMax()
// Desc: Gets a random number between min/max boundaries
//-----------------------------------------------------------------------------
float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

//-----------------------------------------------------------------------------
// Name: getRandomVector()
// Desc: Generates a random vector where X,Y, and Z components are between
//       -1.0 and 1.0
//-----------------------------------------------------------------------------
Vec3f getRandomVector( void )
{
	Vec3f vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector[2] = getRandomMinMax( -1.0f, 1.0f );
    
    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector[2] * vVector[2]);
    
    // Pick a random point on a circle.
    float t = getRandomMinMax( -M_PI, M_PI );

    // Compute matching X and Y for our Z.
    vVector[0] = (float)cosf(t) * radius;
    vVector[1] = (float)sinf(t) * radius;

	return vVector;
}

void GUI::draw_all(){
	int x,i;	//for for loops
	string marquee = " ";

    aspect=1.5;
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, aspect, 0.1, 100000.0);

	
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    glScalef(10.0, 10.0, 10.0);

	//flashlight
	if(flashlightOut)
		glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);


	//check bounds for player
	if(lookFrom[0]<0)
		lookFrom[0]=0;
	if(lookFrom[2]<0)
		lookFrom[2]=0;
	if(lookFrom[0]>hmap->getWidth()-hmap->scale)
		lookFrom[0]=hmap->getWidth()-hmap->scale ;
	if(lookFrom[2]>hmap->getLength()-hmap->scale )
		lookFrom[2]=hmap->getLength()-hmap->scale ;

	if(lockHeight)
		lookFrom[1]=hmap->getHeight(lookFrom[0], lookFrom[2])+10.0;

	 //camera
	lookAt[0]=lookFrom[0]-1.0*cos(h_angle*M_PI/180.0);
	lookAt[1]=lookFrom[1]+1.0*( sin(v_angle*M_PI/180.0) );//+cos(v_angle*M_PI/180.0) );
	lookAt[2]=lookFrom[2]-1.0*sin(h_angle*M_PI/180.0);
	
	gluLookAt(lookFrom[0], lookFrom[1], lookFrom[2], lookAt[0], lookAt[1], lookAt[2], 0.0, 1.0, 0.0);
   
	
	//flashlight position
	light1_position[0]=lookFrom[0]-5.0*cos((h_angle+15.0)*M_PI/180.0);
	light1_position[1]=(lookFrom[1]+4.0*sin((v_angle+kF[1].angle[0])*M_PI/180.0))-2.0;
	light1_position[2]=lookFrom[2]-5.0*sin((h_angle+15.0)*M_PI/180.0);

	//flashlight direction
	light1_direction[0]=-cos(h_angle*M_PI/180.0);
	light1_direction[1]=sin((v_angle+kF[1].angle[0])*M_PI/180.0);
	light1_direction[2]=-sin(h_angle*M_PI/180.0);
    

	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);

	//glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
    //glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light3_direction);

	//glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	//glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
	//glLightfv(GL_LIGHT5, GL_POSITION, light5_position);
	//glLightfv(GL_LIGHT6, GL_POSITION, light6_position);


	
	//enable lighting
    glEnable(GL_LIGHTING);

    //shading   
    glShadeModel(GL_SMOOTH);	
	
	//draw the map
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, terrain_diffuse);
	glColor4fv(terrain_diffuse);
	glScalef(hmap->scale, hmap->scale, hmap->scale);
	load_texture(grass);
	hmap->draw(light0_position, shadowFlag);
	glScalef(1.0/hmap->scale, 1.0/hmap->scale, 1.0/hmap->scale);
	glDisable(GL_TEXTURE_2D);
	

	draw_yourself();
	
	////////draw the players/////////////////////
	
	for(x=0; x<players.size(); x++)
	{
		glColor4fv(mat_diffuse);
		players[x].draw(&robot);
		//STAT_COUNT[1], players[STAT_COUNT[1]].killCount, players[STAT_COUNT[1]].health
		//marquee += "Robot ";
		//marquee = x;
		//+" :  [ Kills " + (string)players[x].killCount + " ] [ Health " + (string)players[x].health +" ] ";
		//marquee = 12;
	}
	//////////end players////////////////////////////

	////////////////
	//Draw Rockets
	/////////////////
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	//
	

	for( i = 0 ; i < missles.size() ; i++)
	{
		glPushMatrix();
		if(!missles[i].dead)
		{
			glTranslatef(missles[i].pos[0], missles[i].pos[1], missles[i].pos[2]);

			glRotatef(-missles[i].h_angle, 0.0f, 1.0f, 0.0f);
			glRotatef(-missles[i].v_angle, 0.0f, 0.0f, 1.0f);

			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			glRotatef(missles[i].rot+=5, 0.0f, 1.0f, 0.0f);
			missle.draw();
			glPopMatrix();
		}
			
	}
	glPopMatrix();

	////////////////////////
	//Draw particles
	////////////////////////
	if(quake_particles){
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_TEXTURE_2D);
	}else{
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		load_texture(s4);
	}

	// This is how will our point sprite's size will be modified by 
    // distance from the viewer
    //float quadratic[] =  { g_x, g_y, g_z };
	float quadratic[] =  { 0.0f, 0.0f, 0.0005f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

	// Query for the max point size supported by the hardware
    float maxSize = 400.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );

    // Clamp size to 100.0f or the sprites could get a little too big on some  
    // of the newer graphic cards. My ATI card at home supports a max point 
    // size of 1024.0f!
    //if( maxSize > 400.0f )
	//	float maxSize = 400.0f;
	//cout << maxSize << endl;

    glPointSize( maxSize );
	// The alpha of a point is calculated to allow the fading of points 
    // instead of shrinking them past a defined threshold size. The threshold 
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
    // the minimum and maximum point sizes.
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 100.0f );
	// Specify point sprite texture coordinate replacement mode for each 
    // texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
	glEnable( GL_POINT_SPRITE_ARB );

	glBegin( GL_POINTS );
        for(i = 0; i < particles.size(); ++i )
        {
			if(!particles[i].dead){
				glColor4fv( particles[i].color);
				glVertex3f( particles[i].pos[0], particles[i].pos[1], particles[i].pos[2]);
			}
		}
	glEnd();

	//quadratic[0] = 0.04;
	//quadratic[2] = 0.00001f ;
    //glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

	glBegin( GL_POINTS );
        for(i = 0; i < debris.size(); ++i )
        {
			if(!debris[i].dead){
				glColor4fv( debris[i].color);
				glVertex3f(debris[i].pos[0], debris[i].pos[1], debris[i].pos[2]);
				//glTranslatef(debris[i].pos[0], debris[i].pos[1], debris[i].pos[2]);
				//glScalef(2.0f,2.0f,2.0f);
				//glScalef(debris[i].radius, debris[i].radius, debris[i].radius);
				//rock.draw();
				//glScalef(-debris[i].radius, -debris[i].radius, -debris[i].radius);
				//glTranslatef(-debris[i].pos[0], -debris[i].pos[1], -debris[i].pos[2]);
				
			}
		}
	glEnd();


	glDisable( GL_POINT_SPRITE_ARB );
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	quadratic[0] = 1.0f;
	quadratic[2] = 0.0001f;
	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );


	glPointSize(3.5);

	//////////END PARTICELS///////////////

	if(fog) //new added
	{
	  glEnable(GL_FOG);
		GLfloat fogColor[4] = { 0.65f, 0.65f, 0.65f, 1.0f};
		GLint fogMode = GL_LINEAR; //GL_LINEAR, GL_EXP2
		glFogi(GL_FOG_MODE, fogMode);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 0.0015f);
		glHint(GL_FOG_HINT, GL_FASTEST);
		glFogf(GL_FOG_START, 10.0f);
		glFogf(GL_FOG_END, 5000.0f);
	}else{
		glDisable(GL_FOG);
	}
    
	//skybox();
	glPopMatrix();

	////Draw Rocket Cam




	/////////////////////
	//DRAW HUD
	/////////////////////
	if (hud) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-5, 5, -5, 5);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

		/////////////////////
		///WRITE TEXT TO HUD
		////////////////////////

		// Push back and cache the current state of pixel alignment.
		glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
		glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
		glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
		glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		char s[256];
		/*sprintf(s,"Position: %f, %f, %f",lookFrom[0], lookFrom[1], lookFrom[2]);
		renderText( -5, 4.5, BITMAP_FONT_TYPE_HELVETICA_18, s );
		
		sprintf(s,"Particles.size() = %d",particles.size()+debris.size());
		renderText( -5, 3.5, BITMAP_FONT_TYPE_HELVETICA_12, s );*/
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		sprintf(s,"FLAGS : %d",hmap->teamFlag[0]);
		renderText( -5, 4.5, BITMAP_FONT_TYPE_HELVETICA_18, s );
		sprintf(s,"KILLS : %d",TEAM_KILLS[0]);
		renderText( -5, 4.0, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		sprintf(s,"FLAGS : %d",hmap->teamFlag[1]);
		renderText( -5, 3.5, BITMAP_FONT_TYPE_HELVETICA_18, s );
		sprintf(s,"KILLS : %d",TEAM_KILLS[1]);
		renderText( -5, 3.0, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		sprintf(s,"Your Kills : %d",YOURKILLS);
		renderText( -5, 2.5, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(1.0f, 0.0f, 0.5f, 1.0f);
		sprintf(s,"Total Kills : %i",TEAM_KILLS[0]+TEAM_KILLS[1]+YOURKILLS);
		renderText( -5, 2.0, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		sprintf(s,"RESPAWN : %i",(respawn));
		renderText( 3.8, 3.7, BITMAP_FONT_TYPE_HELVETICA_12, s );

		if(players.size())
		{
			
			//robot stats
			if(players[STAT_COUNT[1]].team==1)
				glColor3f(1.0f, 0.0f, 0.0f);
			else
				glColor3f(0.0f, 0.0f, 1.0f);

			sprintf(s,"ROBOT %i < KILLS: %i > < HEALTH: %f >"
				,STAT_COUNT[1]
				,players[STAT_COUNT[1]].killCount, players[STAT_COUNT[1]].health);
			renderText( -2.8, 4.5, BITMAP_FONT_TYPE_HELVETICA_18, s );
			
			//sprintf(s,"Robot %i : [ Kills %i ] [ Health %f ]", STAT_COUNT[1], players[STAT_COUNT[1]].killCount, players[STAT_COUNT[1]].health);
			//if(tick+40 > marquee.size())
			//	tick = 0;
			//marquee = marquee.substr(tick,40);
			//cout << marquee << endl;
			
			//renderText( -1.5f, 4.5f, BITMAP_FONT_TYPE_HELVETICA_18, marquee );
			
		}
		
		if(helpScreen)
		{
			sprintf(s,"CONTROLS");
			renderText( -2.0, 4.0, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"Z, X       : Bring out players left and right hands.");
			renderText( -2.0, 3.8, BITMAP_FONT_TYPE_HELVETICA_12, s );
			
			sprintf(s,"W, A, S, D : Move Player (HOLD SHIFT FOR RUN)");
			renderText( -2.0, 3.6, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"Number 1-9 : Load Weapons");
			renderText( -2.0, 3.4, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"Space      : Fire Weapons (R ZOOMS RIFLES)");
			renderText( -2.0, 3.2, BITMAP_FONT_TYPE_HELVETICA_12, s );
			
			sprintf(s,"I, M       : Zoom");
			renderText( -2.0, 3.0, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"H          : Toggle Help Display");
			renderText( -2.0, 2.8, BITMAP_FONT_TYPE_HELVETICA_12, s );
		
		
		}

		for(i=0; i<numTeams; i++)
		{
			if(hmap->teamFlag[i]==9)
			{
				if(i==0)
					glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
				else
					glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
				sprintf(s,"TEAM  %i WINS!!!!!!!",i+1);
				renderText( -1, 3.0, BITMAP_FONT_TYPE_TIMES_ROMAN_24, s );

				if(END_GAME)
					Reset();
				END_GAME=true;
			}
		}
		



		//////////////////END WRITE TEXT//////////////////////////////
		//Psuedo map
		glBegin(GL_POLYGON);
			glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
			glVertex2f(5.0f, 5.0f);
			glVertex2f(4.0f, 5.0f);
			glVertex2f(4.0f, 4.0f);
			glVertex2f(5.0f, 4.0f);
		glEnd();

		//draw robot locations
		for(i = 0 ; i<players.size(); i++){
			if(players[i].team==1)
				glColor3f(1.0f, 0.0f, 0.0f);
			else
				glColor3f(0.0f, 0.0f, 1.0f);
			glBegin(GL_POINTS);
				//glVertex2f(players[i].getPosX()/hmap->getLength()+9.0f, players[i].getPosZ()/hmap->getWidth()+9.0f);
				glVertex2f(players[i].getPosX()/hmap->getLength()+4.0f, players[i].getPosZ()/hmap->getWidth()+4.0f);
			glEnd();
			//cout << " X = " << players[i].getPosX() << " Z = " << players[i].getPosZ() << endl;

		}
		
		//draw flag locations
		for(i = 0 ; i<hmap->_flag.size(); i++){
			if(hmap->_flag[i].team==1)
				glColor3f(1.0f, 1.0f, 0.0f);
			else if(hmap->_flag[i].team==2)
				glColor3f(0.0f, 1.0f, 1.0f);
			else
				glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_POINTS);
				//glVertex2f(players[i].getPosX()/hmap->getLength()+9.0f, players[i].getPosZ()/hmap->getWidth()+9.0f);
				glVertex2f(hmap->_flag[i].pos[0]/hmap->getLength()+4.0f, hmap->_flag[i].pos[2]/hmap->getWidth()+4.0f);
			glEnd();
			//cout << " X = " << players[i].getPosX() << " Z = " << players[i].getPosZ() << endl;
		}

		//draw your location

		glBegin(GL_POINTS);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(lookFrom[0]/hmap->getLength()+4.0f, lookFrom[2]/hmap->getWidth()+4.0f);
		glEnd();

		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}
}

void GUI::draw_min(){
	int x,i;	//for for loops

    aspect=1.5;
	//glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective(zoom, aspect, 0.1, 100000.0);

	
    //glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    //glLoadIdentity();


    //glScalef(10.0, 10.0, 10.0);


	
	 //camera
	//lookAt[0]=lookFrom[0]-1.0*cos(h_angle*M_PI/180.0);
	//lookAt[1]=lookFrom[1]+1.0*( sin(v_angle*M_PI/180.0) );//+cos(v_angle*M_PI/180.0) );
	//lookAt[2]=lookFrom[2]-1.0*sin(h_angle*M_PI/180.0);
	
	//gluLookAt(lookFrom[0], lookFrom[1], lookFrom[2], lookAt[0], lookAt[1], lookAt[2], 0.0, 1.0, 0.0);
   
	
	

	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);

	//glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
    //glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light3_direction);

	//glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	//glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
	//glLightfv(GL_LIGHT5, GL_POSITION, light5_position);
	//glLightfv(GL_LIGHT6, GL_POSITION, light6_position);


	
	//enable lighting
    glEnable(GL_LIGHTING);

    //shading   
    glShadeModel(GL_SMOOTH);	
	
	//draw the map
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, terrain_diffuse);
	glColor4fv(terrain_diffuse);
	glScalef(hmap->scale, hmap->scale, hmap->scale);
	load_texture(grass);
	hmap->draw(light0_position, shadowFlag);
	glScalef(1.0/hmap->scale, 1.0/hmap->scale, 1.0/hmap->scale);
	glDisable(GL_TEXTURE_2D);
	

	
	
	////////draw the players/////////////////////
	
	for(x=0; x<players.size(); x++)
	{
		glColor4fv(mat_diffuse);
		players[x].draw(&robot);
		//STAT_COUNT[1], players[STAT_COUNT[1]].killCount, players[STAT_COUNT[1]].health
		//marquee += "Robot ";
		//marquee = x;
		//+" :  [ Kills " + (string)players[x].killCount + " ] [ Health " + (string)players[x].health +" ] ";
		//marquee = 12;
	}
	//////////end players////////////////////////////

	
	////////////////
	//Draw Rockets
	/////////////////
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	//
	

	for( i = 0 ; i < missles.size() ; i++)
	{
		glPushMatrix();
		if(!missles[i].dead)
		{
			glTranslatef(missles[i].pos[0], missles[i].pos[1], missles[i].pos[2]);

			glRotatef(-missles[i].h_angle, 0.0f, 1.0f, 0.0f);
			glRotatef(-missles[i].v_angle, 0.0f, 0.0f, 1.0f);

			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			glRotatef(missles[i].rot+=5, 0.0f, 1.0f, 0.0f);
			missle.draw();
			glPopMatrix();
		}
			
	}
	glPopMatrix();

	/*////////////////////////
	//Draw particles
	////////////////////////
	if(quake_particles){
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDisable(GL_TEXTURE_2D);
	}else{
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		load_texture(s4);
	}

	// This is how will our point sprite's size will be modified by 
    // distance from the viewer
    //float quadratic[] =  { g_x, g_y, g_z };
	float quadratic[] =  { 0.0f, 0.0f, 0.0005f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

	// Query for the max point size supported by the hardware
    float maxSize = 400.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );

    // Clamp size to 100.0f or the sprites could get a little too big on some  
    // of the newer graphic cards. My ATI card at home supports a max point 
    // size of 1024.0f!
    //if( maxSize > 400.0f )
	//	float maxSize = 400.0f;
	//cout << maxSize << endl;

    glPointSize( maxSize );
	// The alpha of a point is calculated to allow the fading of points 
    // instead of shrinking them past a defined threshold size. The threshold 
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
    // the minimum and maximum point sizes.
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );

    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 100.0f );
	// Specify point sprite texture coordinate replacement mode for each 
    // texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
	glEnable( GL_POINT_SPRITE_ARB );

	glBegin( GL_POINTS );
        for(i = 0; i < particles.size(); ++i )
        {
			if(!particles[i].dead){
				glColor4fv( particles[i].color);
				glVertex3f( particles[i].pos[0], particles[i].pos[1], particles[i].pos[2]);
			}
		}
	glEnd();

	//quadratic[0] = 0.04;
	//quadratic[2] = 0.00001f ;
    //glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

	glBegin( GL_POINTS );
        for(i = 0; i < debris.size(); ++i )
        {
			if(!debris[i].dead){
				glColor4fv( debris[i].color);
				glVertex3f(debris[i].pos[0], debris[i].pos[1], debris[i].pos[2]);
				//glTranslatef(debris[i].pos[0], debris[i].pos[1], debris[i].pos[2]);
				//glScalef(2.0f,2.0f,2.0f);
				//glScalef(debris[i].radius, debris[i].radius, debris[i].radius);
				//rock.draw();
				//glScalef(-debris[i].radius, -debris[i].radius, -debris[i].radius);
				//glTranslatef(-debris[i].pos[0], -debris[i].pos[1], -debris[i].pos[2]);
				
			}
		}
	glEnd();


	glDisable( GL_POINT_SPRITE_ARB );
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	quadratic[0] = 1.0f;
	quadratic[2] = 0.0001f;
	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );


	glPointSize(3.5);
*/
	//////////END PARTICELS///////////////

	if(fog) //new added
	{
	  glEnable(GL_FOG);
		GLfloat fogColor[4] = { 0.65f, 0.65f, 0.65f, 1.0f};
		GLint fogMode = GL_LINEAR; //GL_LINEAR, GL_EXP2
		glFogi(GL_FOG_MODE, fogMode);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 0.0015f);
		glHint(GL_FOG_HINT, GL_FASTEST);
		glFogf(GL_FOG_START, 10.0f);
		glFogf(GL_FOG_END, 5000.0f);
	}else{
		glDisable(GL_FOG);
	}
    
	//skybox();
	glPopMatrix();

	////Draw Rocket Cam



/*
	/////////////////////
	//DRAW HUD
	/////////////////////
	if (hud) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-5, 5, -5, 5);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

		/////////////////////
		///WRITE TEXT TO HUD
		////////////////////////

		// Push back and cache the current state of pixel alignment.
		glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
		glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
		glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
		glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		char s[256];
		//sprintf(s,"Position: %f, %f, %f",lookFrom[0], lookFrom[1], lookFrom[2]);
		//renderText( -5, 4.5, BITMAP_FONT_TYPE_HELVETICA_18, s );
		
		//sprintf(s,"Particles.size() = %d",particles.size()+debris.size());
		//renderText( -5, 3.5, BITMAP_FONT_TYPE_HELVETICA_12, s );
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		sprintf(s,"FLAGS : %d",hmap->teamFlag[0]);
		renderText( -5, 4.5, BITMAP_FONT_TYPE_HELVETICA_18, s );
		sprintf(s,"KILLS : %d",TEAM_KILLS[0]);
		renderText( -5, 4.0, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		sprintf(s,"FLAGS : %d",hmap->teamFlag[1]);
		renderText( -5, 3.5, BITMAP_FONT_TYPE_HELVETICA_18, s );
		sprintf(s,"KILLS : %d",TEAM_KILLS[1]);
		renderText( -5, 3.0, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		sprintf(s,"Your Kills : %d",YOURKILLS);
		renderText( -5, 2.5, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(1.0f, 0.0f, 0.5f, 1.0f);
		sprintf(s,"Total Kills : %i",TEAM_KILLS[0]+TEAM_KILLS[1]+YOURKILLS);
		renderText( -5, 2.0, BITMAP_FONT_TYPE_HELVETICA_18, s );

		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		sprintf(s,"RESPAWN : %i",(respawn));
		renderText( 3.8, 3.7, BITMAP_FONT_TYPE_HELVETICA_12, s );

		if(players.size())
		{
			
			//robot stats
			if(players[STAT_COUNT[1]].team==1)
				glColor3f(1.0f, 0.0f, 0.0f);
			else
				glColor3f(0.0f, 0.0f, 1.0f);


			
			//sprintf(s,"Robot %i : [ Kills %i ] [ Health %f ]", STAT_COUNT[1], players[STAT_COUNT[1]].killCount, players[STAT_COUNT[1]].health);
			if(tick+40 > marquee.size())
				tick = 0;
			marquee = marquee.substr(tick,40);
			//cout << marquee << endl;
			
			renderText( -1.5f, 4.5f, BITMAP_FONT_TYPE_HELVETICA_18, marquee );
			
		}
		
		if(helpScreen)
		{
			sprintf(s,"CONTROLS");
			renderText( -2.0, 4.0, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"Z, X       : Bring out players left and right hands.");
			renderText( -2.0, 3.8, BITMAP_FONT_TYPE_HELVETICA_12, s );
			
			sprintf(s,"W, A, S, D : Move Player (HOLD SHIFT FOR RUN)");
			renderText( -2.0, 3.6, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"Number 1-9 : Load Weapons");
			renderText( -2.0, 3.4, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"Space      : Fire Weapons (R ZOOMS RIFLES)");
			renderText( -2.0, 3.2, BITMAP_FONT_TYPE_HELVETICA_12, s );
			
			sprintf(s,"I, M       : Zoom");
			renderText( -2.0, 3.0, BITMAP_FONT_TYPE_HELVETICA_12, s );

			sprintf(s,"H          : Toggle Help Display");
			renderText( -2.0, 2.8, BITMAP_FONT_TYPE_HELVETICA_12, s );
		
		
		}

		for(i=0; i<numTeams; i++)
		{
			if(hmap->teamFlag[i]==9)
			{
				if(i==0)
					glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
				else
					glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
				sprintf(s,"TEAM  %i WINS!!!!!!!",i+1);
				renderText( -1, 3.0, BITMAP_FONT_TYPE_TIMES_ROMAN_24, s );

				if(END_GAME)
					Reset();
				END_GAME=true;
			}
		}
		



		//////////////////END WRITE TEXT//////////////////////////////
		//Psuedo map
		glBegin(GL_POLYGON);
			glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
			glVertex2f(5.0f, 5.0f);
			glVertex2f(4.0f, 5.0f);
			glVertex2f(4.0f, 4.0f);
			glVertex2f(5.0f, 4.0f);
		glEnd();

		//draw robot locations
		for(i = 0 ; i<players.size(); i++){
			if(players[i].team==1)
				glColor3f(1.0f, 0.0f, 0.0f);
			else
				glColor3f(0.0f, 0.0f, 1.0f);
			glBegin(GL_POINTS);
				//glVertex2f(players[i].getPosX()/hmap->getLength()+9.0f, players[i].getPosZ()/hmap->getWidth()+9.0f);
				glVertex2f(players[i].getPosX()/hmap->getLength()+4.0f, players[i].getPosZ()/hmap->getWidth()+4.0f);
			glEnd();
			//cout << " X = " << players[i].getPosX() << " Z = " << players[i].getPosZ() << endl;

		}
		
		//draw flag locations
		for(i = 0 ; i<hmap->_flag.size(); i++){
			if(hmap->_flag[i].team==1)
				glColor3f(1.0f, 1.0f, 0.0f);
			else if(hmap->_flag[i].team==2)
				glColor3f(0.0f, 1.0f, 1.0f);
			else
				glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_POINTS);
				//glVertex2f(players[i].getPosX()/hmap->getLength()+9.0f, players[i].getPosZ()/hmap->getWidth()+9.0f);
				glVertex2f(hmap->_flag[i].pos[0]/hmap->getLength()+4.0f, hmap->_flag[i].pos[2]/hmap->getWidth()+4.0f);
			glEnd();
			//cout << " X = " << players[i].getPosX() << " Z = " << players[i].getPosZ() << endl;
		}

		//draw your location

		glBegin(GL_POINTS);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(lookFrom[0]/hmap->getLength()+4.0f, lookFrom[2]/hmap->getWidth()+4.0f);
		glEnd();

		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}*/
}

void GUI::weaponsOff()
{
	//hand items
	rightHandOut=false;
	leftHandOut=false;
	batOut=false;
	knifeOut=false;
	flashlightOut=false;
	hammerOut=false;
	gunOut=false;
	uziOut=false;
	dualUzi=false;
	rifleOut=false;
	sniperOut=false;
	bazookaOut=false;
}


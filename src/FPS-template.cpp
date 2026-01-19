
//for the Novodex Library (before windows.h)
#define NOMINMAX 

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

//NOVODEX PHYSICS
#include <NxPhysics.h>
#include "Actors.h"
#include "DrawObjects.h"

//My Classes
#include "mesh.h"

/*
#include "myMesh.h"
#include "myMath.h"
#include "heightMap.h"
#include "AI.h"
#include "robot.h"
#include "robotMesh.h"
#include "wavebox.h"
#include "bitmap_fonts.h"
*/

//#include "./robot/robot.h"
//#include <GL/glut.h>

//#include <GL/glext.h> // Your local header file
#include "glext.h"

// GL_ARB_point_parameters
PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB  = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;


    GLuint      m_textureID;
   
using namespace std;


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

	


 
void unit_square();
float getRandomMinMax( float fMin, float fMax );
Vec3f getRandomVector( void );


/*NOVODEX IMPLEMENTATION*/

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,-9.8,0);

// Time globals
NxReal gTime;
NxReal gLastTime;

// Actor globals
NxActor* ball = NULL;
NxActor* box = NULL;
NxActor* groundPlane = NULL;

// Focus actor
NxActor* gSelectedActor = NULL;

/*END NOVODEX SETUP*/


// The libgfx library provides a minimalist GUI framework encapsulated in the
// MxGUI class.  To use this framework, you simply construct a subclass
// derived from MxGUI, overriding selected virtual functions.
class GUI : public MxGUI
{
public:
    // Here we list the variables that our program uses.

	bool dragging;
	float h_angle, v_angle, temp;

	Mesh calf;
    
//	CWaveBox w;

//	robotMesh robot;


    //Camera Variables
    NxVec3 lookFrom;
    NxVec3 lookAt;
    NxVec3 up;
	NxVec3 cam;

	//for mouse functions
	TVec2<int> oldMouse;
	
	

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
	
	/*Novodex headers*/
	void GUI::InitNx();
	void GUI::ReleaseNx();
	NxReal GUI::UpdateTime();
	void GUI::RunPhysics();
	NxVec3 GUI::ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength);


};

GUI::GUI()
{

	dragging = false;

	h_angle = 0.0f;
	v_angle = 0.0f;
	
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
	calf.loadObj("./calf.obj");

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

}

/*NOVODEX SET UP*/

void GUI::InitNx()
{
    // Create the physics SDK
    gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
    if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_MIN_SEPARATION_FOR_PENALTY, -0.01);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity                      = gDefaultGravity;
    sceneDesc.broadPhase               = NX_BROADPHASE_COHERENT;
    sceneDesc.collisionDetection    = true;
    gScene = gPhysicsSDK->createScene(sceneDesc);

    // Create the default material
    NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
    defaultMaterial->setRestitution(0.5);
    defaultMaterial->setStaticFriction(0.5);
    defaultMaterial->setDynamicFriction(0.5);

	// Create the objects in the scene
//	groundPlane = CreateGroundPlane();
	box = CreateBox( NxVec3(0, 0 ,0), NxVec3(0, 0, 0), 0 );
	UpdateTime();
}

NxReal GUI::UpdateTime()
{
    NxReal deltaTime;
    LARGE_INTEGER count, freq;
    QueryPerformanceCounter(&count);  // Get current count
    QueryPerformanceFrequency(&freq); // Get processor freq
    gTime = (NxReal)count.LowPart/(NxReal)freq.LowPart;
    deltaTime = gTime - gLastTime;
    gLastTime = gTime;
    return deltaTime;
}

void GUI::RunPhysics()
{
	// Update the time step
	NxReal deltaTime = UpdateTime();

	// Run collision and dynamics for delta time since the last frame
	gScene->simulate(deltaTime);	
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

void GUI::ReleaseNx()
{
	if (gScene) gPhysicsSDK->releaseScene(*gScene);
	if (gPhysicsSDK)  gPhysicsSDK->release();
}


/*END NOVODEX*/


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



//	GLfloat light4_position[] = { 50.0f, 500.0, hmap->getWidth()-50.0f, 1.0 };
//	GLfloat light5_position[] = { hmap->getLength()-50.0f, 500.0, hmap->getWidth()-50.0f, 1.0 };
//	GLfloat light6_position[] = { hmap->getLength()-50.0f, 500.0, 50.0, 1.0 };


    ///////////////////
   // Setup Lighting
   ///////////////////
//	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	
   // glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
   // glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
   

   	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	

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


//	if(!robot.built)
//		robot.DL();

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
   
    //DrawBox(box);
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glViewport(0, 0, canvas->w(), canvas->h());
	aspect=1.5;
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, aspect, 0.1, 100.0);

	
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
	draw_all();

}


// This method is called when animation is activated, and the canvas needs to
// be updated for the next frame.  The variable gui.default_fps determines how
// many times per second this method will be called.
//
// The GUI framework will call draw_contents() after this procedure finishes.
// You should NEVER call draw_contents() yourself.
void GUI::update_animation()
{

    //key press and hold
    if(Fl::event_key(FL_Shift_L))//run
	{
		walk_speed/=1.5;
		if(walk_speed<=0.3)
			walk_speed=0.3;
	}
	else
		walk_speed=4.0;

	NxVec3 temp(0.0, 0.0, 0.0);
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
    if( which==1 )
    {
		if(Fl::event_x()-oldMouse[0]!=0)
			h_angle+=(float)(Fl::event_x()-oldMouse[0]) / 5.0f;
		if(Fl::event_y()-oldMouse[1]!=0)
			v_angle-=(float)(Fl::event_y()-oldMouse[1]) / 5.0f;
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


	/*w.Load("./sounds/knife_hit1.wav");
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
	//w.Load("./sounds/gushing_wind.wav");*/

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

		//Initialize Novodex Scene
	gui.InitNx();

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
//	gui.add_toggle_menu("Options/Fog", int('F'), gui.fog, 0); //new added
//	gui.add_toggle_menu("Options/Lock Height", int('L'), gui.lockHeight, 0); //new added
//	gui.add_toggle_menu("Options/Display HUD", int('H'), gui.hud, 0); //new added
//	gui.add_toggle_menu("Options/Display Help", int('?'), gui.helpScreen, 0); //new added
//	gui.add_toggle_menu("Window/Rocket Cam", int('R'), gui.rocket_window, 0); //new added
//	gui.add_toggle_menu("Options/Quake Paritcles", int('Q'), gui.quake_particles, 0); //new added
//	gui.add_toggle_menu("Terrain/Grow Terrain", int('P'), gui.grow, 0); //new added


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

	 //camera
	lookAt[0]=lookFrom[0]-1.0*cos(h_angle*M_PI/180.0);
	lookAt[1]=lookFrom[1]+1.0*( sin(v_angle*M_PI/180.0) );//+cos(v_angle*M_PI/180.0) );
	lookAt[2]=lookFrom[2]-1.0*sin(h_angle*M_PI/180.0);
	
	gluLookAt(lookFrom[0], lookFrom[1], lookFrom[2], lookAt[0], lookAt[1], lookAt[2], 0.0, 1.0, 0.0);

	glScalef(10.0, 10.0, 10.0);

	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	
	glPopMatrix();

}

void GUI::draw_min(){

}

void GUI::weaponsOff()
{

}


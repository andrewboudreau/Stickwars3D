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
#include <time.h>
#include <GL/glut.h>
#include <time.h>

// We'll also want to make use of FLTK's file selector widget
#include <FL/Fl_File_Chooser.H>
#include "myMesh.h"
#include "heightMap.h"


using namespace std;

    //Lighting variables
	GLfloat terrain_diffuse[] = { 0.2f, 0.4f, 0.4f, 1.0f };
   	
   	GLfloat mat_diffuse[] = { 0.2f, 0.4f, 0.4f, 1.0f };
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };

    GLfloat lm_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };

	GLfloat light0_ambient[] = { 0.2f, 0.4f, 0.4f, 1.0f };
	GLfloat light0_diffuse[] = { 0.2f, 0.4f, 0.4f, 1.0f };
	//GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light0_position[] = { 0.0, 10.0, 0.0, 1.0 };
	

 
 
// The libgfx library provides a minimalist GUI framework encapsulated in the
// MxGUI class.  To use this framework, you simply construct a subclass
// derived from MxGUI, overriding selected virtual functions.
class GUI : public MxGUI
{
public:
    // Here we list the variables that our program uses.
    
	float h_angle, v_angle, opt_theta, center[2];

    bool dragging;
    
	//meshes
	//scene

	heightMap hmap;
    
    //Camera Variables
    TVec3<float> lookFrom;
    TVec3<float> lookAt;
    TVec3<float> up;
	TVec3<float> cam;

	//for mouse functions
	TVec2<int> oldMouse;


	bool background;	//turn scene off if too slow
	bool grow;			//make teh map grow
	bool scopeOn;
    float zoom;
	float aspect;
	int wheel;
	
	float walk_speed;
    

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
};

GUI::GUI()
{
	int x=0;// for for loops
	int y=0;
	srand ( time(NULL) );	//seed random
    // It's a good idea to always initialize member variables to
    // reasonable values in the class constructor.
    opt_theta = 10.0f;
    v_angle = h_angle = 0.0f;
    dragging = false;
    center[0] = center[1] = 50.0f;

	background=true;	//have it on initially

	grow=false;		//dont grow map at first

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
	scopeOn = false;

	walk_speed=2.0;		//lower=faster


}

GUI::~GUI()
{
}



// This procedure is called everytime the OpenGL context of the window needs
// to be initialized.  For the most part, this occurs when the window is first
// created and whenever it is resized.
void GUI::setup_for_drawing()
{

    ///////////////////
   // Setup Lighting
   ///////////////////
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

   	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
;
	


   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
   //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
   
   
	glShadeModel(GL_SMOOTH);
	
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	
    // Only enable texturing if we successfully loaded a texture image.

    // Select an appropriate texture mode.  Experiment with the
    // options other than DECAL.
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Standard boilerplate stuff.  You probably won't need to
    // change this.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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
    gluPerspective(zoom, aspect, 0.1, 3000.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(lookFrom[0], lookFrom[1], lookFrom[2], lookAt[0], lookAt[1], lookAt[2], 0.0, 1.0, 0.0);
	
    animate(true);
	glEnable(GL_NORMALIZE);

	//initialize the heightmap
	hmap.initHeightMap(100, 100);
	hmap.scale=1;
    
}

static void unit_square()
{
    glBegin(GL_POLYGON);
      //glTexCoord2f(0, 0);
      glVertex3f(-0.5, 0, -0.5);

      //glTexCoord2f(0, 1);
      glVertex3f(-0.5, 0, 0.5);

      //glTexCoord2f(1, 1);
      glVertex3f(0.5, 0, 0.5);

      //glTexCoord2f(1, 0);
      glVertex3f(0.5, 0, -0.5);
    glEnd();
}

// This procedure is called whenever the contents of the drawing canvas need
// to be redrawn.  This may be for a variety of reasons, including when the
// canvas has been covered by another window that has now been removed or in
// response to an animation update.
void GUI::draw_contents()
{
   
    int x;	//for for loops
    
    aspect=1.5;
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, aspect, 0.1, 5000.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    glScalef(10.0, 10.0, 10.0);


 
    //camera
	lookAt[0]=lookFrom[0]-1.0*cos(h_angle*M_PI/180.0);
	lookAt[1]=lookFrom[1]+1.0*( sin(v_angle*M_PI/180.0) );//+cos(v_angle*M_PI/180.0) );
	lookAt[2]=lookFrom[2]-1.0*sin(h_angle*M_PI/180.0);
	
	gluLookAt(lookFrom[0], lookFrom[1], lookFrom[2], lookAt[0], lookAt[1], lookAt[2], 0.0, 1.0, 0.0);
	
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	
	//enable lighting
    glEnable(GL_LIGHTING);

    //shading   
    glShadeModel(GL_SMOOTH);	
	
	//draw the map
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, terrain_diffuse);

	glScalef(hmap.scale, hmap.scale, hmap.scale);

	hmap.draw();
	glScalef(1.0/hmap.scale, 1.0/hmap.scale, 1.0/hmap.scale);

	if(lookFrom[0]<0)
		lookFrom[0]=0;
	if(lookFrom[2]<0)
		lookFrom[2]=0;
	if(lookFrom[0]>hmap.getWidth()*hmap.scale)
		lookFrom[0]=hmap.getWidth()*hmap.scale;
	if(lookFrom[2]>hmap.getLength()*hmap.scale)
		lookFrom[2]=hmap.getLength()*hmap.scale;
	lookFrom[1]=hmap.getHeight(lookFrom[0], lookFrom[2])+10.0;


    glPopMatrix();
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
	TVec3<float> dir;//temp vars for calculations
	TVec3<float> angle;	

	if(grow)
		hmap.makeHill(rand()%20, rand()%60+20, rand()%60+20);

	

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
			lookAt[1]=lookFrom[1];;
            temp = lookFrom-lookAt;
			lookAt[1]=temp2;
            lookAt -= temp/walk_speed;
            lookFrom -=temp/walk_speed;
    }
    if( Fl::event_key('s') )//go backward
	{
			temp2=lookAt[1];
			lookAt[1]=lookFrom[1];
            temp = lookFrom-lookAt;
			lookAt[1]=temp2;
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
   switch(key)
   {
	   case 'p':
			grow=!grow;
			break;
       case 'q'://where am i
		    cout<<"X="<<lookFrom[0]<<endl;
			cout<<"Y="<<lookFrom[1]<<endl;
			cout<<"Z="<<lookFrom[2]<<endl;
			break;
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
    gui.toplevel->label("!Siebel is DOOMed!");
	//gui.resize_canvas(1024, 768);
	//gui.lock_size();

    // This will actually present the application window on the screen, and
    // begin the event loop.  Note that this function *never* returns.  We
    // only 'return' its value to keep Visual C++ from complaining about
    // main() not returning any value.
    return gui.run();
}

static void load_texture(ByteRaster *img)
{
    if( img )
    {
        glEnable(GL_TEXTURE_2D);

        // Select an appropriate texture mode.  Experiment with the
        // options other than DECAL.
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // Standard boilerplate stuff.  You probably won't need to
        // change this.
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
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

static
bool center_on_click(float *ctr, int *where)
{
    double world[3];

    unproject_pixel(where, world);
    ctr[0] = (float)world[0];
    ctr[1] = (float)world[1];

    return true;
}
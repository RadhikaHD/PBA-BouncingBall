/*  This code accesses the following files:
 
 Vector.cpp
 Vector.h
 Utility.cpp
 Utility.h
 Makefile

Source: http://people.cs.clemson.edu/~dhouse/courses/817/handouts/bounce.zip

 */
#include "Vector.h"
#include <cstdlib>
#include <cstdio>
//#include "GL/gl.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std; 

#define WINDOW_WIDTH	740	
#define WINDOW_HEIGHT	740
#define EPS 0.08
#define STARTX 0.2	
#define STARTY 0.8
#define STARTZ 0.5
#define RADIUS 0.08	
#define FLOORHEIGHT 0
#define BOXLEFT 0
#define BOXRIGHT 1
#define BOXHEIGHT 1
#define BOXFRONT 1
#define BOXBACK 0
#define MAXSTEPS 100000

static double WinWidth = WINDOW_WIDTH;
static double WinHeight = WINDOW_HEIGHT;
static double Mass = 10.0;
static double TimeStep = 0.07;
static double Time = 0;
static double CoeffofRestitution = 0.8;
static double DispTime = 1;

static Vector3d Ball(STARTX, STARTY, STARTZ);
static Vector3d Velocity(0, 0, 0);
static Vector3d G(0,-9.86, 0);
static Vector3d V0(0.1,0.1,0.1);
static Vector3d OldBall[MAXSTEPS];

static int Start = true;
static int Stopped = true;
static int Step = true;
static int NSteps = 0;
static int NTimeSteps = -1;
static int TimeStepsPerDisplay = Max(1, int(DispTime / TimeStep + 0.5));
static int TimerDelay = int(0.5 * TimeStep * 1000);
static int Collision[MAXSTEPS];

void TimerCallback(int value);

void setup_the_viewvolume()
{
	
Vector3d eye, view, up;
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(37.0, 1.0, 0.1, 20.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

eye.x = 0.5; 
eye.y = 1.3;
eye.z = 3.0;
view.x = 0.5;
view.y = 0.2;
view.z = 0.0;
up.x = 0.0;
up.y = 1.0; 
up.z = 0.0;

gluLookAt(eye.x, eye.y, eye.z, view.x, view.y, view.z, up.x, up.y, up.z);

}

void do_material()
{
	
float mat_ambient[] = {0.0,0.0,0.0,1.0}; 
float mat_diffuse[] = {1.0,0.549,0.0,1.0}; 
float mat_specular[] = {0.3,0.3,0.3,1.0};
float mat_shininess[] = {2.0}; 

glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

}

void do_lights()
{
	
float light0_ambient[] = {0.0, 0.0, 0.0, 0.0};
float light0_diffuse[] = {2.0, 2.0, 2.0, 0.0};
float light0_specular[] = {2.25, 2.25, 2.25, 0.0};
float light0_position[] = {1.5, 2.0, 2.0, 1.0};
float light0_direction[] = { -1.5, -2.0, -2.0, -2.0, 1.0};

glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light0_ambient);
glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);

glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);

}


void doReshape(int w, int h)
{

int vpw, vph;
float aspect = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);

  if(float(w) / float(h) > aspect)
  {
    vph = h;
    vpw = int(aspect * h + 0.5);
  }
  else
  {
    vpw = w;
    vph = int(w / aspect + 0.5);
  }
  
glViewport(0, 0, vpw, vph);
WinWidth = w;
WinHeight = h;
setup_the_viewvolume();

}

void DrawBox()
{

do_lights();
do_material();

int i;

Vector3d front[4] = { Vector3d(0.0,0.0,1.0), Vector3d(0.0,1.0,1.0), Vector3d(1.0,1.0,1.0), Vector3d(1.0,0.0,1.0)};
Vector3d back[4] = {Vector3d(1.0,0.0,0.0),Vector3d(1.0,1.0,0.0),Vector3d(0.0,1.0,0.0),Vector3d(0.0,0.0,0.0)};
Vector3d left[4] = {Vector3d(0.0,0.0,0.0),Vector3d(0.0,1.0,0.0),Vector3d(0.0,1.0,1.0),Vector3d(0.0,0.0,1.0)};
Vector3d right[4] = {Vector3d(1.0,0.0,1.0),Vector3d(1.0,1.0,1.0),Vector3d(1.0,1.0,0.0),Vector3d(1.0,0.0,0.0)};
Vector3d top[4] = {Vector3d(0.0,1.0,1.0),Vector3d(0.0,1.0,0.0),Vector3d(1.0,1.0,0.0),Vector3d(1.0,1.0,1.0)};
Vector3d bottom[4] = {Vector3d(0.0,0.0,0.0),Vector3d(0.0,0.0,1.0),Vector3d(1.0,0.0,1.0),Vector3d(1.0,0.0,0.0)};

glBegin(GL_QUADS);
	/*
	Do not draw the front surface for now
	
	glNormal3f(0.0, 0.0, -1.0);
	for(i=0;i<4;i++) glVertex3f(front[i].x, front[i].y, front[i].z);
	*/
	glNormal3f(0.0, 0.0, 1.0);
	for(i=0;i<4;i++) glVertex3f(back[i].x, back[i].y, back[i].z);
	glNormal3f(1.0, 0.0, 0.0);
	for(i=0;i<4;i++) glVertex3f(left[i].x, left[i].y, left[i].z);
	glNormal3f(1.0, 0.0, 0.0);
	for(i=0;i<4;i++) glVertex3f(right[i].x, right[i].y, right[i].z);
	glNormal3f(0.0, 1.0, 0.0);
	for(i=0;i<4;i++) glVertex3f(top[i].x, top[i].y, top[i].z);
	glNormal3f(0.0, 1.0, 0.0);
	for(i=0;i<4;i++) glVertex3f(bottom[i].x, bottom[i].y, bottom[i].z);
glEnd();

glFlush();

}

void DrawSphere(float x, float y, float z)
{
	
float drawx = x;
float drawy = y;
float drawz = z;
float drawrad = RADIUS;

GLfloat sphere_diffuse[] = { 0.411, 0.411, 0.411, 1.0 };
glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_diffuse);

glPushMatrix();
glTranslatef (drawx,drawy,drawz); 
glutSolidSphere(drawrad,40,40);
glPopMatrix ();

glFlush();

}



void DrawBall(int collision)
{
   
DrawBox();
DrawSphere(Ball.x, Ball.y, Ball.z);
glutSwapBuffers();

  if(NSteps < MAXSTEPS){
    OldBall[NSteps] = Ball;
    Collision[NSteps] = collision;
    NSteps++;
  }
  else{
    cerr << "Ball position table overflow, restarting!!" << endl;
    Time = 0;
    NSteps = 0;
  }
  
}


Vector3d Accel()
{

Vector3d acceleration;
double mass;
mass = Mass;
acceleration = G;
return acceleration;
  
}


void Simulate()
{

Vector3d acceleration;
Vector3d newvelocity, newball;
int resting;

	if(Stopped)
	return;

resting =  Abs(TimeStep * Velocity.y) < EPS && Abs(Ball.y - (RADIUS + FLOORHEIGHT)) < EPS; 

acceleration = Accel();

  if(resting && acceleration.y < EPS)
  {
    Velocity.y = 0;
    Ball.y = RADIUS + FLOORHEIGHT;
    acceleration.y = 0;
  }
  else
    resting = false;

newvelocity = Velocity + TimeStep * acceleration;
newball = Ball + TimeStep * Velocity;

  //detect collision with floor

  if(!resting && newball.y - RADIUS <= FLOORHEIGHT && Ball.y - RADIUS > FLOORHEIGHT)
  {
    
    double f = (FLOORHEIGHT - (Ball.y - RADIUS)) / (newball.y - Ball.y);
    newvelocity = Velocity + f * TimeStep * acceleration;
    newball = Ball + f * TimeStep * Velocity;
    Velocity = newvelocity;
    Velocity.y *= -CoeffofRestitution;
    
    Ball = newball;

    acceleration = Accel();
    newvelocity = Velocity + (1 - f) * TimeStep * acceleration;
    newball = newball + (1 - f) * TimeStep * Velocity; 
  }
  
  // left face collision
  if (!resting && newball.x - RADIUS < BOXLEFT && Ball.x - RADIUS > BOXLEFT)
  {
    double f = (BOXLEFT - (Ball.x - RADIUS)) / (newball.y - Ball.y);
    newvelocity = Velocity + f * TimeStep * acceleration;
    newball = Ball + f * TimeStep * Velocity;
    Velocity = newvelocity;
    Velocity.x *= -CoeffofRestitution;
    
    Ball = newball;

    acceleration = Accel();
    newvelocity = Velocity + (1 - f) * TimeStep * acceleration;
    newball = newball + (1 - f) * TimeStep * Velocity; 
    
  }
  
  // right face collision
  if (!resting && newball.x - RADIUS <= BOXRIGHT && Ball.x + RADIUS > BOXRIGHT)
  {
    
    double f = (BOXRIGHT - (Ball.x + RADIUS)) / (newball.x - Ball.x);
    newvelocity = Velocity + f * TimeStep * acceleration;
    newball = Ball + f * TimeStep * Velocity;
    Velocity = newvelocity;
    Velocity.x *= -CoeffofRestitution;
    
    Ball = newball;

    acceleration = Accel();
    newvelocity = Velocity + (1 - f) * TimeStep * acceleration;
    newball = newball + (1 - f) * TimeStep * Velocity; 
   
  }
  
  // top face collision
  if (!resting && (newball.y + RADIUS) >= BOXHEIGHT && Ball.y + RADIUS < BOXHEIGHT)
  {
    
    double f = (Ball.y - RADIUS) / (Ball.y - newball.y);
    newvelocity = Velocity + f * TimeStep * acceleration;
    newball = Ball + f * TimeStep * Velocity;
    Velocity = newvelocity;
    Velocity.y *= -CoeffofRestitution;
    
    Ball = newball;

    acceleration = Accel();
    newvelocity = Velocity + (1 - f) * TimeStep * acceleration;
    newball = newball + (1 - f) * TimeStep * Velocity;
    
  }
  
   // front face collision
  if (!resting && newball.z + RADIUS >= BOXFRONT && Ball.z + RADIUS < BOXFRONT)
  {
    
    double f = (BOXFRONT - (Ball.z + RADIUS)) / (newball.z - Ball.z);
    newvelocity = Velocity + f * TimeStep * acceleration;
    newball = Ball + f * TimeStep * Velocity;
    Velocity = newvelocity;
    Velocity.z *= -CoeffofRestitution;
    
    Ball = newball;

    acceleration = Accel();
    newvelocity = Velocity + (1 - f) * TimeStep * acceleration;
    newball = newball + (1 - f) * TimeStep * Velocity;
    
  }
  
   // back face collision
  if (!resting && newball.z - RADIUS < BOXBACK && Ball.z - RADIUS > BOXBACK)
  {
    
    double f = (BOXLEFT - (Ball.z - RADIUS)) / (newball.z - Ball.z);
    newvelocity = Velocity + f * TimeStep * acceleration;
    newball = Ball + f * TimeStep * Velocity;
    Velocity = newvelocity;
    Velocity.z *= -CoeffofRestitution;
    
    Ball = newball;

    acceleration = Accel();
    newvelocity = Velocity + (1 - f) * TimeStep * acceleration;
    newball = newball + (1 - f) * TimeStep * Velocity;
    
  }

Time += TimeStep;
NTimeSteps++;
Velocity = newvelocity;
Ball = newball;



  glutIdleFunc(NULL);
  
  
  Stopped = true;
  
  
}


void TimerCallback(int)
{
  Simulate();
}


void drawDisplay()
{

glEnable(GL_DEPTH_TEST);
glClearColor(0.0,0.0,0.0, 0.0);
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
DrawBall(0);

}


void handleKey(unsigned char key, int x, int y)
{
  
  switch(key)
  {
    case ' ':		// start simulation
  
	  if(key == ' ')
	  {  
		if(Start)
		{
		  Start = false;
		  Stopped = false;
		  Ball.set(STARTX, STARTY, STARTZ);
		  
		  Velocity = V0;
		  DrawBall(0);
		  glutIdleFunc(Simulate);   
		}
		else if(Stopped)
		{
		  Stopped = false;
		  glutIdleFunc(Simulate);    
		}
		else
		{
		  Stopped = true;
		  glutIdleFunc(NULL);    
		}
	  }
     break;
    
    
     
    case 27:		
      exit(0);
      break;
      
    default:		
      return;
      break;
  }
  
glutPostRedisplay();

}


int main(int argc, char* argv[])
{
	
glutInit(&argc, argv);
glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA | GLUT_DEPTH );
glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
glutInitWindowPosition(100,50);
glutCreateWindow("Bouncing Ball In a cube Demo");
glClearColor(0.35,0.35,0.35, 0.0);
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
glutReshapeFunc(doReshape);
glutDisplayFunc(drawDisplay);
glutKeyboardFunc(handleKey);
glutMainLoop();
return 0;

}

//  ========================================================================
//  COSC363 Computer Graphics (2023), University of Canterbury
//
//  Spaceship.cpp
//  See Lab03.pdf for details.
//  ========================================================================

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <GL/freeglut.h>
#include "loadTGA.h"
using namespace std;

//---- Globals ----
GLuint txId[2];   //Texture ids
float angle = -10.0;		//Rotation angle for viewing
float cam_hgt = 40;			//Camera height
const int SS_NBODY = 18;	//Base curve used in the construction of spaceship's body
const float vx_init[SS_NBODY] = { 5,  15, 19,   21, 23.5, 25,   27, 29, 30, 30, 29, 27, 25,  23, 21, 18,  15, 5};
const float vy_init[SS_NBODY] = { 20, 20, 19.5, 19, 18,   17.5, 16, 14, 12, 10,  9, 7.5, 6.5, 6, 5.5, 5.2, 5, 5};
GLUquadricObj*	q;

//---- A function to load textures in TGA format ----
void loadTextures()				
{
	glGenTextures(2, txId); 	// Create 2 texture ids

	glBindTexture(GL_TEXTURE_2D, txId[0]);  //First texture
    loadTGA("Rock.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, txId[1]);  //Second texture
	loadTGA("Stars.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

//---- Function to compute the normal vector using three vertices ----
void normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	float nx, ny, nz;
	nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
	ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
	nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
	glNormal3f(nx, ny, nz);
}

//---- The following functions draw different parts of the scene ----
//---- Floor plane -----
void drawFloor()
{
	glColor3f(0., 1, 1.);		//Floor colour
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);			//A single quad
	   glVertex3f(-500, 0, 500);
	   glVertex3f(500, 0, 500);
	   glVertex3f(500, 0, -500);
	   glVertex3f(-500, 0, -500);
	glEnd();
}

//---- Spaceship Dome ----
void draw_ss_top()
{
	glColor3f(0.4, 0.4, 0.4);			
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);	
	glNormal3f(0, 1, 0);
	glVertex3f(-5, 30, 5);
	glVertex3f(5, 30, 5);
	glVertex3f(5, 30, -5);
	glVertex3f(-5, 30, -5);

	normal(-5, 30, 5, -10, 20, 10, 10, 20, 10);
	glVertex3f(-5, 30, 5);
	glVertex3f(-10, 20, 10);
	glVertex3f(10, 20, 10);
	glVertex3f(5, 30, 5);

	normal(5, 30, 5, 10, 20, 10, 10, 20, -10);
	glVertex3f(5, 30, 5);
	glVertex3f(10, 20, 10);
	glVertex3f(10, 20, -10);
	glVertex3f(5, 30, -5);

	normal(5, 30, -5, 10, 20, -10, -10, 20, -10);
	glVertex3f(5, 30, -5);
	glVertex3f(10, 20, -10);
	glVertex3f(-10, 20, -10);
	glVertex3f(-5, 30, -5);

	normal(-5, 30, -5, -10, 20, -10, -10, 20, 10);
	glVertex3f(-5, 30, -5);
	glVertex3f(-10, 20, -10);
	glVertex3f(-10, 20, 10);
	glVertex3f(-5, 30, 5);

	glEnd();
}

//---- Alien! ----
void draw_ss_alien()
{
	glColor3f(0, 0, 1);
	glPushMatrix();
	  glTranslatef(0, 26, 0);
	  glScalef(4, 2., 2.);
	  glutSolidSphere(1, 36, 18);
	glPopMatrix();
	glPushMatrix();
	   glTranslatef(0, 24, 0);
	   glutSolidSphere(1.5, 36, 18);
	glPopMatrix();
	glPushMatrix();
	   glTranslatef(0, 20, 0);
	   glScalef(2, 4, 2.);
	   glutSolidSphere(1, 36, 18);
	glPopMatrix();
	glColor3f(1, 1, 1);
	glPushMatrix();
	  glTranslatef(-2, 26, 1);
	  glutSolidSphere(1, 36, 18);
	glPopMatrix();
	glPushMatrix();
	  glTranslatef(2, 26, 1);
	  glutSolidSphere(1, 36, 18);
	glPopMatrix();
}

//---- Spaceship Main Body ----
void draw_ss_body()
{
	float vx[SS_NBODY], vy[SS_NBODY], vz[SS_NBODY];   //vertex positions
	float wx[SS_NBODY], wy[SS_NBODY], wz[SS_NBODY];
	float toRadians = M_PI / 180.0;
	float angStep = 10.0 * toRadians;  //Rotate base curve in 10 deg steps

	//----Define spotlight position, direction here----
	//float light1_pos[4] = { 21.21, 10, 21.21, 1 };
	//float light1_dir[3] = { 0.5, -0.7071, 0.5 };
	//glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);

	glColor3f(1, 0, 0);
	for (int i = 0; i < SS_NBODY; i++)		//Initialize data everytime the frame is refreshed
	{
		vx[i] = vx_init[i];
		vy[i] = vy_init[i];
		vz[i] = 0;
	}

	for (int j = 0; j < 36; j++)
	{
		for (int i = 0; i < SS_NBODY; i++)
		{
			wx[i] = cos(angStep)*vx[i] + sin(angStep)*vz[i];
			wy[i] = vy[i];
			wz[i] = -sin(angStep)*vx[i] + cos(angStep)*vz[i];
		}

		glBegin(GL_QUAD_STRIP);
		for (int i = 1; i < SS_NBODY; i++)
		{
			normal(vx[i - 1], vy[i - 1], vz[i - 1], vx[i], vy[i], vz[i], wx[i], wy[i], wz[i]);
			if (i == 1)
			{
				glVertex3f(vx[0], vy[0], vz[0]);
				glVertex3f(wx[0], wy[0], wz[0]);
			}
			glVertex3f(vx[i], vy[i], vz[i]);
			glVertex3f(wx[i], wy[i], wz[i]);
		}
		glEnd();

		for (int i = 0; i < SS_NBODY; i++)
		{
			vx[i] = wx[i];
			vy[i] = wy[i];
			vz[i] = wz[i];
		}
	}

	//Two yellow spheres
	glColor3f(1, 1, 0);
	glPushMatrix();
	  glRotatef(45, 0, 1, 0);
	  glTranslatef(0, 10, 29);
	  glutSolidSphere(2, 36, 10);
	glPopMatrix();
	glPushMatrix();
	  glRotatef(-45, 0, 1, 0);
	  glTranslatef(0, 10, 29);
	  glutSolidSphere(2, 36, 10);
	glPopMatrix();
 }

//---- Spaceship Legs ----
void draw_ss_legs()
{
	glColor3f(0.6, 0.6, 0.6);
	for (int i = 0; i < 3; i++)
	{
		glPushMatrix();
		   glRotatef(120 * i, 0, 1, 0);
		   glTranslatef(0, 0, -25);
		   glRotatef(-26.56, 1, 0, 0);    //atan(5/10)
		   gluCylinder(q, 1, 1, 12, 18, 4);
		glPopMatrix();

		glPushMatrix();
		   glRotatef(120 * i, 0, 1, 0);
		   glTranslatef(0, 0, -25);
		   glRotatef(-90, 1, 0, 0); 
		   gluCylinder(q, 3, 0, 1.4, 18, 2);
		glPopMatrix();
	}
}

//---- Function to draw the whole spaceship ----
void drawSpaceship()
{
	draw_ss_body();
	draw_ss_legs();
	draw_ss_alien();
	draw_ss_top();
}

//---- A circular spaceport ----
void drawSpaceport()
{
	glColor3f(1, 0.7, 0.7);
	glPushMatrix();
	   glTranslatef(0, 2, 0);
	   glRotatef(-90, 1, 0, 0);
	   gluDisk(q, 0, 50, 400, 100);
	glPopMatrix();
	glColor3f(0.556, 0.25, 0.226);
	glPushMatrix();
	   glRotatef(-90, 1, 0, 0);
	   gluCylinder(q, 50, 50, 2, 72, 2);
	glPopMatrix();
}

//---- Initialize function ----
void initialise()
{ 
	float white[] = { 1, 1, 1, 1 };
	loadTextures();	

	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	//glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40);
	//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10);

	glEnable(GL_COLOR_MATERIAL);
	glClearColor(1., 1., 1., 1.);     //Background colour 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 10, 1000);  //The camera view volume


}


//---- Special key event callback ----
void special(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) angle--;
	else if (key == GLUT_KEY_RIGHT) angle++;
	else if (key == GLUT_KEY_UP) cam_hgt++;
	else if (key == GLUT_KEY_DOWN) cam_hgt--;

	if (cam_hgt > 200) cam_hgt = 200;
	else if (cam_hgt < 10) cam_hgt = 10;
	glutPostRedisplay();
}

//---- Display callback ----
void display() 
{
	float light0_pos[4] = { 10., 100., 100., 1.0 };  //light's position
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glMatrixMode(GL_MODELVIEW);								
	glLoadIdentity();
	gluLookAt(0, cam_hgt, 100, 0, 0, 0, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glRotatef(angle, 0.0, 1.0, 0.0);		//rotate the whole scene

	glPushMatrix();
	   glTranslatef(0, 2, 0);    //The spaceport has a height of 2 units
	   drawSpaceship();
	glPopMatrix();

	drawSpaceport();
	drawFloor();

	glFlush();									
}

//---- Glut initialization ---
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE| GLUT_DEPTH);
   glutInitWindowSize (800, 800); 
   glutInitWindowPosition (10, 10);
   glutCreateWindow ("Spaceship");
   initialise();
   glutDisplayFunc(display); 
   glutSpecialFunc(special);
   glutMainLoop();
   return 0;
}

/*==================================================================================
* COSC 363  Computer Graphics (2023)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab06.pdf   for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "TextureBMP.h"
#include <GL/freeglut.h>
using namespace std;

const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;

// Texture Mapping Coords
int z1 = -60;
int z2 = -90;
int x1 = -15;
int x2 = 5;

TextureBMP texture;

vector<SceneObject*> sceneObjects;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 lightPos(10, 40, -3);					//Light's position
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found

	if (ray.index == 4)
	{
		//Stripe pattern
		int stripeWidth = 5;
		int iz = (ray.hit.z) / stripeWidth;
		int k = iz % 2; //2 colors
		if (k == 0) color = glm::vec3(0, 1, 0);
		else color = glm::vec3(1, 1, 0.5);
		obj->setColor(color);

		//Texture Mapping
		float texcoords = (ray.hit.x - x1) / (x2 - x1);
		float texcoordt = (ray.hit.z - z1) / (z2 - z1);
		if (texcoords > 0 && texcoords < 1 &&
			texcoordt > 0 && texcoordt < 1)
		{
			color = texture.getColorAt(texcoords, texcoordt);
			obj->setColor(color);
		}

	}


	color = obj->lighting(lightPos, -ray.dir, ray.hit);						//Object's colour
	glm::vec3 lightVec = lightPos - ray.hit;

	Ray shadowRay(ray.hit, lightVec);
	shadowRay.closestPt(sceneObjects);
	float lightDist = glm::length(lightVec);

	if (shadowRay.index > -1 && shadowRay.dist < lightDist) { // Pixel is in shadow
		color = 0.2f * obj->getColor(); // 0.2 = ambient scale factor
	}

	if (obj->isReflective() && step < MAX_STEPS)
	{
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor);
	}


	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;

			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray

			Ray ray = Ray(eye, dir);

			glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

    glEnd();
    glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

	texture = TextureBMP("Butterfly.bmp");

    glClearColor(0, 0, 0, 1);

	// BIG DARK BLUE SPHERE
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, 0.0, -90.0), 15.0); // Creates Pointer to a new sphere with centre at (-5, 0, -90) and radius 15
	sphere1->setColor(glm::vec3(0, 0, 1));   //Set colour to blue
	sphere1->setReflectivity(true, 0.8);
	sceneObjects.push_back(sphere1);		 //Add sphere to scene objects

	// SMALL CYAN SPHERE
	Sphere* sphere2 = new Sphere(glm::vec3(10, 10, -60), 3.0);
	sphere2->setColor(glm::vec3(0, 1, 1));
	sphere2->setShininess(5); // Not very shiny
	sceneObjects.push_back(sphere2);

	// SMALL RED SPHERE
	Sphere* sphere3 = new Sphere(glm::vec3(5.0, 5.0, -70.0), 4.0);
	sphere3->setColor(glm::vec3(1, 0, 0));
	sphere3->setSpecularity(false); // Not reflective
	sceneObjects.push_back(sphere3);

	// MEDIUM GREEN SPHERE
	Sphere* sphere4 = new Sphere(glm::vec3(5, -10, -60), 5.0);
	sphere4->setColor(glm::vec3(0, 1, 0));
	sceneObjects.push_back(sphere4);

	// PLANE
	Plane* plane = new Plane(glm::vec3(-20., -15, -40), //Point A
		glm::vec3(20., -15, -40), //Point B
		glm::vec3(20., -15, -200), //Point C
		glm::vec3(-20., -15, -200)); //Point D
	plane->setColor(glm::vec3(0.8, 0.8, 0));
	plane->setSpecularity(false);
	sceneObjects.push_back(plane);
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}

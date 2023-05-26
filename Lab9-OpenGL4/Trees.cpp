//  ============================================================================
//  COSC422: Advanced Computer Graphics (2022);  University of Canterbury.
//
//  FILE NAME: Trees.cpp
//
//	See Lab09.pdf for details
//  =============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "loadTGA.h"
using namespace std;

GLuint vaoID[2], texID[2];
GLuint theProgram;
GLuint objLoc;

void loadTextures()
{
    glGenTextures(2, texID);
   // Load road texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadTGA("Road.tga");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Load tree texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID[1]);
    loadTGA("Tree.tga");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void initialise()
{
	float toRad = 3.14159265 / 180.0;   //Conversion from degs to radians

	loadTextures();
	GLuint program = createShaderProg("Trees.vert", "Trees.frag");

	//Assign values to sampler objects in the fragment shader
	GLuint texLoc = glGetUniformLocation(program, "texRoad");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(program, "texTree");
	glUniform1i(texLoc, 1);

	//Index to distinguish between road and tree
	objLoc = glGetUniformLocation(program, "objIndex");

	glm::vec4 light = glm::vec4(0.0, 50.0, 100.0, 1.0);
	glm::mat4 proj = glm::perspective(55.0f*toRad, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 10.0, 70.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix

	glm::vec4 lightEye = view*light;     //Light position in eye coordinates
	GLuint lgtLoc = glGetUniformLocation(program, "lightPos");
	glUniform4fv(lgtLoc, 1, &lightEye[0]);

	glm::mat4 projView = proj*view;  //Product matrix
	glm::mat4 invMatrix = glm::inverse(view);  //Inverse of model-view matrix for normal transformation
	GLuint matrixLoc1 = glGetUniformLocation(program, "mvMatrix");
	GLuint matrixLoc2 = glGetUniformLocation(program, "mvpMatrix");
	GLuint matrixLoc3 = glGetUniformLocation(program, "norMatrix");
	glUniformMatrix4fv(matrixLoc1, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(matrixLoc2, 1, GL_FALSE, &projView[0][0]);
	glUniformMatrix4fv(matrixLoc3, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here

	//Data for "road"
	float vert_road[4*3] = { -10.0, 0.0, 70.0, 10.0, 0.0, 70.0, 10.0, 0.0, -70.0, -10.0, 0.0, -70.0};
	float norm_road[4*3] = {0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0};
	float tex_road[4*2] = {0.0, 0.0, 1.0, 0.0, 1.0, 5.0, 0.0, 5.0};

	//Data for "trees"
	float vert_tree[96] = { 0 }, norm_tree[96] = { 0 };
	float tex_tree[64];
	float zpos[4] = {45, 15, -15, -45 };  //positions of trees
	for (int i = 0; i < 8; i++)   //8 trees,  4 on each side of the road
	{
		int side = i / 4;   //0 left side,  1 right side
		for (int j = 0; j < 4; j++)  //4 verts
		{
			int voff = 12 * i + 3 * j;   //vertex offset for position, normal
			if(j==0 || j==3) vert_tree[voff] = -16 + side * 20;
			else vert_tree[voff] = -4 + side * 20;
			vert_tree[voff + 1] = (j/2)*12;
			vert_tree[voff + 2] = zpos[i-side*4];
			norm_tree[voff + 2] = 1;

			int toff = 8 * i + 2 * j;  //texture coords offset
			if( j < 2) tex_tree[toff] = j;
			else tex_tree[toff] = 3 - j;
			tex_tree[toff+1] = j / 2;
		}
	}

	GLuint vboID[3];  //3 vertex attributes: position, normal, texcoord

	glGenVertexArrays(2, vaoID); //2 objects: road, tree

    glBindVertexArray(vaoID[0]);  //road
    glGenVertexArrays(3, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_road), vert_road, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  //position

    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norm_road), norm_road, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);  //normal

    glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(tex_road), tex_road, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);  //texture coords

    glBindVertexArray(vaoID[1]);  //tree
    glGenVertexArrays(3, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_tree), vert_tree, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  //position

	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(norm_tree), norm_tree, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);  //normal

    glBindBuffer(GL_ARRAY_BUFFER, vboID[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_tree), tex_tree, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);  //texture coords

    glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUniform1i(objLoc, 0);
	glBindVertexArray(vaoID[0]);
	glDrawArrays(GL_QUADS, 0, 4);   //road

	glUniform1i(objLoc, 1);
	glBindVertexArray(vaoID[1]);
	glDrawArrays(GL_QUADS, 0, 32);   //8 trees

	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Trees");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit())
	{
		cerr << "Unable to initialize GLEW  ...exiting" << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}


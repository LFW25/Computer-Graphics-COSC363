//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  FILE NAME: QuadPatches.cpp
//  See Lab10.pdf for details.
//
//  Required files   QuadPatches.vert, QuadPatches.frag
//                   QuadPatches.cont, QuadPatches.eval
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

GLuint vaoID;
GLuint mvpMatrixLoc;
float angle = -40.0;					//Rotation angle
const float CDR = 3.14159265/180.0;     //Conversion from degrees to rad
glm::mat4 projView;

//Loads a shader file and returns the reference to a shader object
//======This is a standard function in shader applications =======
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

//Initialises the shader program, creates and loads buffer data
void initialise()
{
	glm::mat4 proj, view; //Projection and view matrices

//-----------Load shaders----------------------
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "QuadPatches.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "QuadPatches.frag");
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "QuadPatches.cont");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "QuadPatches.eval");

//---------Attach and link shaders--------------
	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");

//--------Compute matrices----------------------
	proj = glm::perspective(40.0f*CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
	view = glm::lookAt(glm::vec3(0.0, 15.0, 30.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix
	projView = proj * view;  //Proj-View matrix

//---------Load buffer data-----------------------
	float patchVerts[] =     //9 patch vertices
	{
	   -5, 0, 5,    //P0
		0, 3, 5,    //P1
		5, 0, 5,    //P2

	   -5, 6, 0,
		0, 10, 0,
		5, 6, 0,

	   -5, 3, -5,
		0, 4, -5,
		5, 3, -5    //P8
	};

	GLuint vboID;
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &vboID);    //Only one VBO

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(patchVerts), patchVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindVertexArray(0);
	glPatchParameteri(GL_PATCH_VERTICES, 9);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}


void special(int key, int x, int y)
{
	if(key == GLUT_KEY_LEFT) angle--;
	else if (key == GLUT_KEY_RIGHT) angle++;
	glutPostRedisplay();
}

//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{
	glm:: mat4 mvpMatrix = glm::rotate(projView, angle*CDR, glm::vec3(0.0, 1.0, 0.0));  //rotation matrix pre-mupltiplied by projetion-view matrix

	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawArrays(GL_PATCHES, 0, 9);
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Tessellation Example");
	glutInitContextVersion (4, 1);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutSpecialFunc(special); 
	glutMainLoop();
	return 0;
}


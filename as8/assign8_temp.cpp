/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 8-MAPPING
Spring 2006

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include <GL/glut.h>
#include <windows.h>

#include "glprocs.h"
# include "assign8_temp.h"
#include "read_tga.h"

#define PI 3.14159265359

#define PrintOpenGLError()::PrintOGLError(__FILE__, __LINE__)

using namespace std;


//object related information
int verts[3], faces[3], norms[3];    // Number of vertices, faces and normals in the system
point *vertList[3], *normList[3]; // Vertex and Normal Lists
faceStruct *faceList[3];	    // Face List


//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,p;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;
int program=-1;

float originX, originY, originZ;
int selection = 0;
//Parameters for Copper (From: "Computer Graphics Using OpenGL" BY F.S. Hill, Jr.) 
//GLfloat ambient_cont [] = {0.19125,0.0735,0.0225};
GLfloat diffuse_cont [] = {0.7038,0.27048,0.0828};
//GLfloat specular_cont [] = {0.256777,0.137622,0.086014};

GLfloat ambient_cont[] = { 0.1,0.1,0.1 };
//GLfloat diffuse_cont[] = { 0.5,0.5,0.5 };
GLfloat specular_cont[] = { 0.8,0.8,0.8 };
GLfloat exponent = 12.8;


//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;



float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;


void findOrigin(int slot) {
	float maxX = vertList[slot][faceList[slot][0].v1].x;
	float minX = vertList[slot][faceList[slot][0].v1].x;
	float maxY = vertList[slot][faceList[slot][0].v1].y;
	float minY = vertList[slot][faceList[slot][0].v1].y;
	float maxZ = vertList[slot][faceList[slot][0].v1].z;
	float minZ = vertList[slot][faceList[slot][0].v1].z;
	for (int i = 0; i < faces[slot]; i++)
	{

		point v;
		v = vertList[slot][faceList[slot][i].v1];
		if (v.x < minX) {
			minX = v.x;
		}
		if (v.x > maxX) {
			maxX = v.x;
		}
		if (v.x < minX) {
			minX = v.x;
		}
		if (v.y > maxY) {
			maxY = v.y;
		}
		if (v.y < minY) {
			minY = v.y;
		}
		if (v.x > maxX) {
			maxY = v.y;
		}

		v = vertList[slot][faceList[slot][i].v2];
		if (v.x < minX) {
			minX = v.x;
		}
		if (v.x > maxX) {
			maxX = v.x;
		}
		if (v.x < minX) {
			minX = v.x;
		}
		if (v.y > maxY) {
			maxY = v.y;
		}
		if (v.y < minY) {
			minY = v.y;
		}
		if (v.x > maxX) {
			maxY = v.y;
		}
		v = vertList[slot][faceList[slot][i].v3];
		if (v.x < minX) {
			minX = v.x;
		}
		if (v.x > maxX) {
			maxX = v.x;
		}
		if (v.x < minX) {
			minX = v.x;
		}
		if (v.y > maxY) {
			maxY = v.y;
		}
		if (v.y < minY) {
			minY = v.y;
		}
		if (v.x > maxX) {
			maxY = v.y;
		}
	}

	originX = (maxX + minX) / 2;
	originY = (maxY + minY) / 2;
	originZ = (maxZ + minZ) / 2;
}

void getSphericalTex(point p) {
	point v = { p.x - originX, p.y - originY, p.z - originZ };
	float vmag = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v = { v.x / vmag, v.y / vmag, v.z / vmag };
	float costheta = v.y;
	float cosphi = v.z;
	//printf("x: %f y: %f z: %f\n", v.x, v.y, v.z);
	//printf("costheta: %f cosphi: %f\n", costheta, cosphi);
	glTexCoord2f((costheta + 1)/2, (cosphi + 1)/2);
}


void DisplayFunc(void) 
{
	int slot;
	if (selection == 0 || selection == 9) {
		slot = 0;
	}
	else if (selection == 1 || selection == 3 || selection == 5 || selection == 7 || selection == 10) {
		slot = 1;
	}
	else {
		slot = 2;
	}
    GLuint id ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  0,0,0,
			  0,1,0);

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);

		setParameters(program);

	// Load image from tga file
	TGA *TGAImage	= new TGA("./planartexturemap/abstract2.tga");
	//TGA *TGAImage	= new TGA("./cubicenvironmentmap/cm_right.tga");

	// Use to dimensions of the image as the texture dimensions
	uint width	= TGAImage->GetWidth();
	uint height	= TGAImage->GetHeigth();
	
	// The parameters for actual textures are changed

	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable( GL_TEXTURE_2D );

	glBindTexture (GL_TEXTURE_2D, id); 

    delete TGAImage;

	findOrigin(slot);
	printf("originX: %f originY: %f originZ: %f\n", originX, originY, originZ);
	for (int i = 0; i < faces[slot]; i++)
	{
		
		glBegin(GL_TRIANGLES);
			point v1, v2, v3, n1, n2, n3;
			v1 = vertList[slot][faceList[slot][i].v1];
			v2 = vertList[slot][faceList[slot][i].v2];
			v3 = vertList[slot][faceList[slot][i].v3];
			n1 = vertList[slot][faceList[slot][i].v1];
			n2 = vertList[slot][faceList[slot][i].v2];
			n3 = vertList[slot][faceList[slot][i].v3];
			glNormal3f(n1.x, n1.y, n1.z);
			//glTexCoord2f(v1.x, v1.y);
			getSphericalTex(v1);

			glVertex3f(v1.x, v1.y, v1.z);
			glNormal3f(n2.x, n2.y, n2.z);
		
			//glTexCoord2f(v2.x, v2.y);
			getSphericalTex(v2);
		
			glVertex3f(v2.x, v2.y, v2.z);
			glNormal3f(n3.x, n3.y, n3.z);
			
			//glTexCoord2f(v3.x, v3.y);
			getSphericalTex(v3);
			glVertex3f(v3.x, v3.y, v3.z);
		glEnd();

	}	

	//glutSolidTeapot(1);
	setParameters(program);
	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
		if (CameraPhi > (PI - 0.01))
			CameraPhi = PI - 0.01;
		if (CameraPhi < 0.01)
			CameraPhi = 0.01;
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}





//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		selection = (selection + 1) % 11;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		if (illimunationMode == 0)
		{
			illimunationMode = 1;
		}
		else
		{
			illimunationMode = 0;
		}
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
		}
		else
		{
			shadingMode =0;
		}
		break;
	case 'd':
	case 'D':
		if (lightSource == 0)
		{
			lightSource =1;
		}
		else
		{
			lightSource =0;
		}
		break;
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
		}
		break;

    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 6");



	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

	
	setShaders();
	meshReader("sphere.obj", 1, 0); //temp
	meshReader("sphere.obj", 1, 1);
	meshReader("teapot.obj", 1, 2);

	glutMainLoop();

	return 0;
}

/*************************************************************
Shader related methods,
Setting the shader files
Setting the shader variables
*************************************************************/

void error_exit(int status, char *text)
{

	// Print error message

	fprintf(stderr,"Internal Error %i: ", status);
	fprintf(stderr,text);
	printf("\nTerminating as Result of Internal Error.\nPress Enter to exit.\n");

	// Keep the terminal open

	int anyKey = getchar();

	// Exit program

	exit(status);
}

int PrintOGLError(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void setShaders() 
{

	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("sampleshader.vert");
	fs = shaderFileRead("sampleshader.frag");

	const char * vv = vs;
	const char * ff = fs;

	GLint       vertCompiled, fragCompiled;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);

	glGetObjectParameterivARB(fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
	glGetObjectParameterivARB(vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    if (!vertCompiled || !fragCompiled)
	{
        cout<<"not compiled"<<endl;
	}
	
	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	program =p;
	//attach the shader objects to the program object
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p);


	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);

	    
	setParameters(p);

}

//Gets the location of the uniform variable given with "name" in the memory
//and tests whether the process was successfull.
//Returns the location of the queried uniform variable
int getUniformVariable(GLuint program,char *name)
{
	int location = glGetUniformLocationARB(program, name);
	
	if (location == -1)
	{
 		error_exit(1007, "No such uniform variable");
	}
	PrintOpenGLError();
	return location;
}

void update_Light_Position()
{
	
	// Create light components
	GLfloat light_position[] = { CameraRadius*cos(CameraTheta)*sin(CameraPhi),			  
			  CameraRadius*cos(CameraPhi) , 
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	

}

//Sets the light positions, etc. parameters for the shaders
void setParameters(GLuint program)
{
	int light_loc;
	int ambient_loc,diffuse_loc,specular_loc;
	int exponent_loc;

	//sample variable used to demonstrate how attributes are used in vertex shaders.
	//can be defined as gloabal and can change per vertex
	float tangent = 0.0;
	float tangent_loc;

	update_Light_Position();

	//Access uniform variables in shaders
	ambient_loc = getUniformVariable(program, "AmbientContribution");	
	glUniform3fvARB(ambient_loc,1, ambient_cont);

	diffuse_loc = getUniformVariable(program, "DiffuseContribution");
	glUniform3fvARB(diffuse_loc,1, diffuse_cont);

	specular_loc = getUniformVariable(program, "SpecularContribution");
	glUniform3fvARB(specular_loc,1,specular_cont);

	exponent_loc = getUniformVariable(program, "exponent");
	glUniform1fARB(exponent_loc,exponent);

	//Access attributes in vertex shader
	tangent_loc = glGetAttribLocationARB(program,"tang");
	glVertexAttrib1fARB(tangent_loc,tangent);

	GLint tex = glGetUniformLocationARB(program, "tex");
	glUniform1iARB(tex, 0);

}


/****************************************************************
Utility methods:
shader file reader
mesh reader for objectt
****************************************************************/
//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}

void meshReader (char *filename,int sign, int slot)
{
  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL) { 
    printf("Cannot open %s\n!", filename);
    exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp))
   {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      if (letter == 'v')
	  {
		  verts[slot]++;
	  }
      else
	  {
		  faces[slot]++;
	  }
   }

  fclose(fp);

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList[slot] = (faceStruct *)malloc(sizeof(faceStruct)*faces[slot]);
  vertList[slot] = (point *)malloc(sizeof(point)*verts[slot]);
  normList[slot] = (point *)malloc(sizeof(point)*verts[slot]);

  fp = fopen(filename, "r");

  // Read the veritces
  for(i = 0;i < verts[slot];i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[slot][i].x = x;
      vertList[slot][i].y = y;
      vertList[slot][i].z = z;
    }

  // Read the faces
  for(i = 0;i < faces[slot];i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[slot][i].v1 = ix - 1;
      faceList[slot][i].v2 = iy - 1;
      faceList[slot][i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts[slot]);
  for (i = 0;i < verts[slot];i++)
    {
      normList[slot][i].x = normList[slot][i].y = normList[slot][i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces[slot];i++)
    {
      v1.x = vertList[slot][faceList[slot][i].v2].x - vertList[slot][faceList[slot][i].v1].x;
      v1.y = vertList[slot][faceList[slot][i].v2].y - vertList[slot][faceList[slot][i].v1].y;
      v1.z = vertList[slot][faceList[slot][i].v2].z - vertList[slot][faceList[slot][i].v1].z;
      v2.x = vertList[slot][faceList[slot][i].v3].x - vertList[slot][faceList[slot][i].v2].x;
      v2.y = vertList[slot][faceList[slot][i].v3].y - vertList[slot][faceList[slot][i].v2].y;
      v2.z = vertList[slot][faceList[slot][i].v3].z - vertList[slot][faceList[slot][i].v2].z;

      crossP.x = v1.y*v2.z - v1.z*v2.y;
      crossP.y = v1.z*v2.x - v1.x*v2.z;
      crossP.z = v1.x*v2.y - v1.y*v2.x;

      len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

      crossP.x = -crossP.x/len;
      crossP.y = -crossP.y/len;
      crossP.z = -crossP.z/len;

      normList[slot][faceList[slot][i].v1].x = normList[slot][faceList[slot][i].v1].x + crossP.x;
      normList[slot][faceList[slot][i].v1].y = normList[slot][faceList[slot][i].v1].y + crossP.y;
      normList[slot][faceList[slot][i].v1].z = normList[slot][faceList[slot][i].v1].z + crossP.z;
      normList[slot][faceList[slot][i].v2].x = normList[slot][faceList[slot][i].v2].x + crossP.x;
      normList[slot][faceList[slot][i].v2].y = normList[slot][faceList[slot][i].v2].y + crossP.y;
      normList[slot][faceList[slot][i].v2].z = normList[slot][faceList[slot][i].v2].z + crossP.z;
      normList[slot][faceList[slot][i].v3].x = normList[slot][faceList[slot][i].v3].x + crossP.x;
      normList[slot][faceList[slot][i].v3].y = normList[slot][faceList[slot][i].v3].y + crossP.y;
      normList[slot][faceList[slot][i].v3].z = normList[slot][faceList[slot][i].v3].z + crossP.z;
      normCount[faceList[slot][i].v1]++;
      normCount[faceList[slot][i].v2]++;
      normCount[faceList[slot][i].v3]++;
    }
  for (i = 0;i < verts[slot];i++)
    {
      normList[slot][i].x = (float)sign*normList[slot][i].x / (float)normCount[i];
      normList[slot][i].y = (float)sign*normList[slot][i].y / (float)normCount[i];
      normList[slot][i].z = (float)sign*normList[slot][i].z / (float)normCount[i];
    }

}




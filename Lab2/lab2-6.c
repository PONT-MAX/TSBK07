// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"
#include <math.h>




Model* m;
Model* n;

// Perspective
#define near 1.0
#define far 100.0
#define right 1.0
#define left -1.0
#define top 1.0
#define bottom -1.0
GLfloat projectionMatrix[] = {
    2.0f*near/(right-left), 0.0f,                   (right+left)/(right-left),      0.0f,
    0.0f,                   2.0f*near/(top-bottom), (top+bottom)/(top-bottom),      0.0f,
    0.0f,                   0.0f,                   -(far + near)/(far - near),     -2*far*near/(far - near),
    0.0f,                   0.0f,                   -1.0f,                          0.0f };


// The rotation matrices will be changed for animation
GLfloat rotationMatrix[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f };
GLfloat rotationMatrix2[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f };
GLfloat translationMatrix[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f };


void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, &OnTimer, value);
}


// vertex array object
unsigned int vertexArrayObjID;

unsigned int bunnyVertexArrayObjID;
unsigned int bunnyVertexBufferObjID;
unsigned int bunnyIndexBufferObjID;
unsigned int bunnyNormalBufferObjID;
unsigned int bunnyTexCoordBufferObjID;
mat4 rot;
mat4 trans;
mat4 total;

mat4 camera;

    GLuint program;
// Reference to texture
GLuint tex;

void init(void)
{
	// vertex buffer object, used for uploading the geometry


	// Reference to shader program
    m = LoadModel("bunnyplus.obj");
    n = LoadModelPlus("teddy.obj");

	dumpInfo();
    glutTimerFunc(1, &OnTimer, 0);

	// GL inits
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glClearColor(0.1,0.5,0.5,0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab2-6.vert", "lab2-6.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
    
    glGenVertexArrays(1, &bunnyVertexArrayObjID);
    glGenBuffers(1, &bunnyVertexBufferObjID);
    glGenBuffers(1, &bunnyIndexBufferObjID);
    glGenBuffers(1, &bunnyNormalBufferObjID);
    glGenBuffers(1, &bunnyTexCoordBufferObjID);
    
    glBindVertexArray(bunnyVertexArrayObjID);
    
    // VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bunnyVertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
    
    // VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);
    
    if (m->texCoordArray != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, bunnyTexCoordBufferObjID);
        glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
        glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
    }
    
    glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
    LoadTGATextureSimple("maskros512.tga", &tex);
	// End of upload of geometry
    glActiveTexture(GL_TEXTURE0);
	
	printError("init arrays");
}

GLfloat t = 0.0;

void display(void)
{

    t = t+0.01;
    trans = T(0, 0, 0);
    rot = Ry(180);
    total = Mult(rot, trans);

    
    camera = lookAt(sin(t)*2.5,0.5,cos(t)*2.5f,
                    0.0f,0.0f,0.0f,
                    0.0f,1.0f,0.0f);

    printError("init shader");
	printError("pre display");
    // Matrixes

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    rotationMatrix2[0] = cos(t);
    rotationMatrix2[1] = -sin(t);
    rotationMatrix2[4] = sin(t);
    rotationMatrix2[5] = cos(t);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix2"), 1, GL_TRUE, rotationMatrix2);
    rotationMatrix[5] = cos(t/5.0);
    rotationMatrix[6] = -sin(t/5.0);
    rotationMatrix[9] = sin(t/5.0);
    rotationMatrix[10] = cos(t/5.0);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "Camera"), 1, GL_TRUE, camera.m);

    
    glBindVertexArray(bunnyVertexArrayObjID);    // Select VAO
    glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);
    
   
    trans = T(0, 1, 0);
    total = Mult(rot, trans);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    
    DrawModel(n,program,"in_Position","in_Normal","inTexCoord");
	
	printError("display");
	
	glutSwapBuffers();                  // Swap
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();
}

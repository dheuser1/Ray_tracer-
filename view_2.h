/*
 * The View class is our way of encapsulating all the data and OpenGL rendering required for our projects.
 * Thus, this class is the one that will change most often from one program to another
 *
*/

#ifndef VIEW_H_2
#define VIEW_H_2
//Remember that glew will not include all the opengl functions, so you must include both glew and gl
//IF YOU DO THIS, REMEMBER TO INCLUDE GLEW BEFORE GL, OTHERWISE YOU WILL GET COMPILING ERRORS!
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
using namespace std;
#include "utils/Object.h"
#include "utils/Light.h"
#include "MyCamera.h"
#include "Maze.h"


/* 
 * we are using glm, a mathematics library for OpenGL
 * glm has two advantages:
 * 1. It is based on the GLSL specification. That is, all the data structures, functions and algorithms it offers
 *    follow the same convention as GLSL shaders. Therefore the transition to GLSL will be very smooth.
 * 2. glm is completely header-based. Thus there are no libraries or dlls to link to. This makes it very portable.
 **/

//the include below is for the basic glm stuff. You will find this header until C:\QTFiles\glm, so include that path in the project file
#include <glm/glm.hpp>

class view_2
{
        //the different kinds of vertex buffer objects (VBOs)
    enum Buffer_IDs {ArrayBuffer,IndexBuffer,NumBuffers};
        //the different kinds of attributes for a vertex. This corresponds to the variables in VertexAttribs class above.
   // enum Attrib_IDs {vPosition=0,vColor=1};

    //a structure that encapsulates information about our GLSL shaders.
    typedef struct {
        GLenum       type; //is it a vertex shader, a fragment shader, a geometry shader, a tesselation shader or none of the above?
        string  filename; //the file that stores this shader
        GLuint       shader; //the ID for this shader after it has been compiled
    } ShaderInfo;

	typedef struct
    {
        GLint positionLocation;
        GLint ambientLocation;
        GLint diffuseLocation;
        GLint specularLocation;
    } LightLocation;

public:
    view_2();
	~view_2();

    //the delegation functions called from the windowing functions in main.cpp
    void resize(int w,int h);
    void initialize();
    void draw();
	void advanceCamera();
	void retractCamera();
	void rotateLeftCamera();
	void rotateRightCamera();
	void rotateCWCamera();
	void rotateCCWCamera();
	void rotateUpCamera();
	void rotateDownCamera();
	void accelerateCamera();
	void decelerateCamera();
	void use_mouse_data(int mouse_x, int mouse_y, int code);
	void VectorAssign(int lesserx, int greaterx, int lessery, int greatery);
	void lines_to_cells();
	void to_file();
	void update_x(int x) {cam_x=x;}
	void update_y(int y) {cam_y=y;}
	void update_spot();

	
    //helper functions to probe the supported (latest) version of OpenGL and GLSL.
    void getOpenGLVersion(int *major,int *minor);
    void getGLSLVersion(int *major,int *minor);

protected:
	//helper function to load in the model
	void initObjects();
    //helper function to compile and link our GLSL shaders
    GLuint createShaders(ShaderInfo *shaders);
    //helper function to print error messages for shader compiling
    void printShaderInfoLog(GLuint shader);

private:
    //the width and height of the window
    int WINDOW_WIDTH,WINDOW_HEIGHT;
	graphics::Light light[1];

	//the GLSL program id
	GLuint program;
    //all our Vertex Access Object IDs
    GLuint vao;
    //all our Vertex Buffer Object IDs
    GLuint vbo[NumBuffers];
   //IDs for locating various variables in our shaders
    GLint projectionLocation,modelViewLocation,colorLocation;

    //the actual projection and modelview matrices
    glm::mat4 proj,modelView;

	MyCamera camera;

	//position of the hovering camera
	glm::vec3 cam_pos;
	float theta;

	//triangle mesh object
	graphics::Object *meshObject;
	graphics::Object *meshObject_solved;
	graphics::Object *meshObject_spot;
	Maze* my_maze;
	string **vert_mtrx;
	int min_win;
	int aspect;
	int start_rec_x;
	int start_rec_y;
	int end_rec_x;
	int end_rec_y;
	int row;
	int col;
	vector<VertexAttribs> coordinates;
	vector<VertexAttribs> coordinates_solved;
	vector<VertexAttribs> coordinates_spot;
    vector<unsigned int> lines;
	vector<unsigned int> lines_solved;
	vector<unsigned int> lines_spot;
	VertexAttribs v;
	TriangleMesh tmesh;
	TriangleMesh tmesh_solved;
	TriangleMesh tmesh_spot;
	vector<double> square;	
	int dec(string base_2);
	vector< vector<int> > ans;
	int cam_x;
	int cam_y;
};

#endif // VIEW_H_2
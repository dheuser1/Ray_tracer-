#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "View.h"
#include <GL/glew.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "Ray.h"
#include "hitRecord.h"
using namespace std;


//glm headers to access various matrix producing functions, like ortho below in resize
#include <glm/gtc/matrix_transform.hpp>
//the glm header required to convert glm objects into normal float/int pointers expected by OpenGL
//see value_ptr function below for an example
#include <glm/gtc/type_ptr.hpp>

#include "SceneXMLReader.h"

View::View()
{
	//each step of 1 is 3
	trackballTransform = glm::mat4(1.0);
	time = 0.0;
	my_maze = new Maze();
	row=my_maze->get_row();
	col=my_maze->get_col();
	vert_mtrx=my_maze->get_vert_mtrx();
	ans=my_maze->get_ans();
	cam_state=1;
	cam_x=-3;
	cam_y=25;
	my_maze->flag_pos(25-cam_y,cam_x+26);
	flag_x=my_maze->get_flag_x();
	flag_y=my_maze->get_flag_y();
	direction=0;
	steps=0;
	solved_steps=my_maze->get_solved_steps();
	make_pic=true;
	//was used to make maze xml file
	//make_xml();
}

View::~View()
{

}

void View::make_xml()
{
	ofstream outstream;
	string name = "maze_rides.xml";
	string temp;
	outstream.open(name);
	if(outstream.fail())
	{
		cout<<"opening failed"<<endl;
		exit(1);
	}

	outstream<<"<scene>"<<endl;
	outstream<<"	<instance name=\"sphere\" path=\"models/sphere\" />"<<endl;
	outstream<<"	<instance name=\"box\" path=\"models/box\" />"<<endl;
	outstream<<"	<instance name=\"cylinder\" path=\"models/cylinder\" />"<<endl;
	outstream<<"	<instance name=\"cone\" path=\"models/cone\" />"<<endl;
	outstream<<"	<transform>"<<endl;
	outstream<<"		<set>"<<endl;
	outstream<<"			<scale> .3 .3 .3 </scale>"<<endl;
	outstream<<"			<rotate> -90 0 0 1 </rotate>"<<endl;
	outstream<<"			<translate> -80 75 0 </translate>"<<endl;
	outstream<<"		</set>"<<endl;
	outstream<<"		<group>"<<endl;

	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			//vertical walls
			if(vert_mtrx[i][j].at(0)=='1')
			{
				outstream<<"			<transform>"<<endl;
				outstream<<"				<set>"<<endl;
				outstream<<"					<scale> 10 2 6 </scale>"<<endl;
				outstream<<"					<translate> "<<10*i<<" "<<10*j<<" 0 </translate>"<<endl;
				outstream<<"				</set>"<<endl;
				outstream<<"				<object instanceof=\"box\">"<<endl;
				outstream<<"					<material>"<<endl;
				outstream<<"						<ambient>.2 0 0</ambient>"<<endl;
				outstream<<"						<diffuse>.7 0 0</diffuse>"<<endl;
				outstream<<"						<specular>.8 0 0</specular>"<<endl;
				outstream<<"						<shininess>10</shininess>"<<endl;
				outstream<<"					</material>"<<endl;
				outstream<<"				</object>"<<endl;
				outstream<<"			</transform>"<<endl;

			}
			//horizontal walls
			if(vert_mtrx[i][j].at(1)=='1')
			{
				outstream<<"			<transform>"<<endl;
				outstream<<"				<set>"<<endl;
				outstream<<"					<scale> 2 10 6 </scale>"<<endl;
				outstream<<"					<translate> "<<10*i-5<<" "<<10*j+5<<" 0 </translate>"<<endl;
				outstream<<"				</set>"<<endl;
				outstream<<"				<object instanceof=\"box\">"<<endl;
				outstream<<"					<material>"<<endl;
				outstream<<"						<ambient>.2 0 0</ambient>"<<endl;
				outstream<<"						<diffuse>.7 0 0</diffuse>"<<endl;
				outstream<<"						<specular>.8 0 0</specular>"<<endl;
				outstream<<"						<shininess>10</shininess>"<<endl;
				outstream<<"					</material>"<<endl;
				outstream<<"				</object>"<<endl;
				outstream<<"			</transform>"<<endl;
			}

			//get edge vertical walls
			if(i<row && j==col-1 && vert_mtrx[i][j].at(2)=='1')
			{
				outstream<<"			<transform>"<<endl;
				outstream<<"				<set>"<<endl;
				outstream<<"					<scale> 10 2 6 </scale>"<<endl;
				outstream<<"					<translate> "<<10*i<<" "<<10*j+10<<" 0 </translate>"<<endl;
				outstream<<"				</set>"<<endl;
				outstream<<"				<object instanceof=\"box\">"<<endl;
				outstream<<"					<material>"<<endl;
				outstream<<"						<ambient>.2 0 0</ambient>"<<endl;
				outstream<<"						<diffuse>.7 0 0</diffuse>"<<endl;
				outstream<<"						<specular>.8 0 0</specular>"<<endl;
				outstream<<"						<shininess>10</shininess>"<<endl;
				outstream<<"					</material>"<<endl;
				outstream<<"				</object>"<<endl;
				outstream<<"			</transform>"<<endl;
			}

			//get edge horizontal walls
			if(i==row-1 && j<col && vert_mtrx[i][j].at(3)=='1')
			{
				outstream<<"			<transform>"<<endl;
				outstream<<"				<set>"<<endl;
				outstream<<"					<scale> 2 10 6 </scale>"<<endl;
				outstream<<"					<translate> "<<10*i+5<<" "<<10*j+5<<" 0 </translate>"<<endl;
				outstream<<"				</set>"<<endl;
				outstream<<"				<object instanceof=\"box\">"<<endl;
				outstream<<"					<material>"<<endl;
				outstream<<"						<ambient>.2 0 0</ambient>"<<endl;
				outstream<<"						<diffuse>.7 0 0</diffuse>"<<endl;
				outstream<<"						<specular>.8 0 0</specular>"<<endl;
				outstream<<"						<shininess>10</shininess>"<<endl;
				outstream<<"					</material>"<<endl;
				outstream<<"				</object>"<<endl;
				outstream<<"			</transform>"<<endl;
			}
		}   
	}

	outstream<<"		</group>"<<endl;
	outstream<<"	</transform>"<<endl;
	outstream<<"</scene>"<<endl;
	outstream.close();
}

void View::resize(int w, int h)
{
	//record the new dimensions
	WINDOW_WIDTH = w;
	WINDOW_HEIGHT = h;

	/*
	* This program uses orthographic projection. The corresponding matrix for this projection is provided by the glm function below.
	*The last two parameters are for the near and far planes.
	*
	*Very important: the last two parameters specify the position of the near and far planes with respect
	*to the eye, in the direction of gaze. Thus positive values are in front of the camera, and negative
	*values are in the back!
	**/

	while (!proj.empty())
		proj.pop();

	//proj.push(glm::ortho(-200.0f,200.0f,-200.0f*WINDOW_HEIGHT/WINDOW_WIDTH,200.0f*WINDOW_HEIGHT/WINDOW_WIDTH,0.1f,10000.0f));
	proj.push(glm::perspective(120.0f*3.14159f/180,(float)WINDOW_WIDTH/WINDOW_HEIGHT,0.1f,10000.0f));
}

void View::openFile(string filename)
{
	SceneXMLReader reader;
	cout << "Loading...";
	reader.importScenegraph(filename,sgraph);

	cout << "Done" << endl;
}

void View::initialize()
{
	//populate our shader information. The two files below are present in this project.
	ShaderInfo shaders[] =
	{
		{ GL_VERTEX_SHADER, "phong-multiple.vert" },
		{ GL_FRAGMENT_SHADER, "phong-multiple.frag" },
		{ GL_NONE, "" }
		/* {GL_VERTEX_SHADER,"triangles.vert"},
		{GL_FRAGMENT_SHADER,"triangles.frag"},
		{GL_NONE,""} //used to detect the end of this array*/
	};

	//call helper function, get the program shader ID if everything went ok.
	program = createShaders(shaders);

	//use the above program. After this statement, any rendering will use this above program
	//passing 0 to the function below disables using any shaders
	glUseProgram(program);

	projectionLocation = glGetUniformLocation(program,"projection"); 

	sgraph.initShaderProgram(program);


}

void View::draw()
{
	if(cam_state!=4)
	{
		make_pic=true;

		double PI = 3.14159;
		/*
		*The modelview matrix for the View class is going to store the world-to-view transformation
		*This effectively is the transformation that changes when the camera parameters chang
		*This matrix is provided by glm::lookAt
		*/
		glUseProgram(program);
		while (!modelview.empty())
			modelview.pop();
		GLuint a;
		//camera is located at eye and is faced towards center.
		//eye, center,up
		modelview.push(glm::mat4(1.0));
		if(cam_state==1)
		{
			modelview.top() = modelview.top() * glm::lookAt(glm::vec3(-80,-30,50),glm::vec3(0,0,0),glm::vec3(0,0,1)) * trackballTransform;

		}

		else if (cam_state == 3)
		{
			glm::mat4 position= glm::mat4(1.0);
			glm::vec4 p = glm::vec4(0, 10, 0, 1);
			glm::vec4 e = glm::vec4(0, 10, -50, 1);
			glm::vec4 u = glm::vec4(0, 1, 0, 0);
			sgraph.getcameranode()->getNodeAni(position);
			p = position * p;
			e = position * e;
			u = position * u;

			modelview.top() = modelview.top() * glm::lookAt(glm::vec3(p.r, p.g, p.b), glm::vec3(e.r, e.g,e.b), glm::vec3(u.r, u.g, u.b));

			p = e;
		}

		glUniformMatrix4fv(projectionLocation,1,GL_FALSE,glm::value_ptr(proj.top()));
		/*
		*Instead of directly supplying the modelview matrix to the shader here, we pass it to the objects
		*This is because the object's transform will be multiplied to it before it is sent to the shader
		*for vertices of that object only.
		*
		*Since every object is in control of its own color, we also pass it the ID of the color
		*in the activated shader program.
		*
		*This is so that the objects can supply some of their attributes without having any direct control
		*of the shader itself.
		*/
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		a = glGetError();
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		sgraph.draw(modelview, program);
		a = glGetError();
		glFinish();
		a = glGetError();
		glUseProgram(0);
		modelview.pop();
	}
	else if(make_pic==true)
	{
		while (!modelview.empty())
		{
			modelview.pop();
		}
		modelview.push(glm::mat4(1.0));
		modelview.top() = modelview.top() * glm::lookAt(glm::vec3(-80,-30,50),glm::vec3(0,0,0),glm::vec3(0,0,1)) * trackballTransform;
		sgraph.ray_trace(800,800,modelview);
		if(!modelview.empty())
		{
			modelview.pop();
		}
		make_pic=false;
	}
}

void View::mousepress(int x, int y)
{
	prev_mouse = glm::vec2(x,y);
}

void View::mousemove(int x, int y)
{
	int dx,dy;

	dx = x - prev_mouse.x;
	dy = (y) - prev_mouse.y;

	if ((dx==0) && (dy==0))
		return;

	//(-dy,dx) gives the axis of rotation

	//the angle of rotation is calculated in radians by assuming that the radius of the trackball is 300
	float angle = sqrt((float)dx*dx+dy*dy)/100;

	prev_mouse = glm::vec2(x,y);

	trackballTransform = glm::rotate(glm::mat4(1.0),angle,glm::vec3(-dy,dx,0)) * trackballTransform;
}
//make member vars for cam position draw will use them and cam state to see what fo feed lookat
void View::set_camera(int cam)
{
	//trackballTransform = glm::mat4(1.0);
	cam_state=cam;
}



void View::pos_update(int i)
{
	if(i==1)
	{
		if(direction==0 && vert_mtrx[25-cam_y][cam_x+26].at(3)=='0' && cam_y>-25)
		{
			cam_y-=i;
			steps++;
		}
		else if(direction==1 && vert_mtrx[25-cam_y][cam_x+26].at(0)=='0' && cam_x>-26)
		{
			cam_x-=i;
			steps++;
		}
		else if(direction==2 && vert_mtrx[25-cam_y][cam_x+26].at(1)=='0' && cam_y<25)
		{
			cam_y+=i;
			steps++;
		}
		else if(direction==3 && vert_mtrx[25-cam_y][cam_x+26].at(2)=='0' && cam_x<24)
		{
			cam_x+=i;
			steps++;
		}
	}

	else
	{
		if(direction==0 && vert_mtrx[25-cam_y][cam_x+26].at(1)=='0' && cam_y<25)
		{
			cam_y-=i;
			steps++;
		}
		else if(direction==1 && vert_mtrx[25-cam_y][cam_x+26].at(2)=='0' && cam_x<24)
		{
			cam_x-=i;
			steps++;
		}
		else if(direction==2 && vert_mtrx[25-cam_y][cam_x+26].at(3)=='0' && cam_y>-25)
		{
			cam_y+=i;
			steps++;
		}
		else if(direction==3 && vert_mtrx[25-cam_y][cam_x+26].at(0)=='0' && cam_x>-26)
		{
			cam_x+=i;
			steps++;
		}
	}
	if(cam_y==flag_y && cam_x == flag_x)
	{
		my_maze->flag_pos(25-cam_y,cam_x+26);
		flag_x=my_maze->get_flag_x();
		flag_y=my_maze->get_flag_y();
	}
}
// 0 is facing down 1 is left 2 is up 3 is right
void View::dir_update(int i)
{
	if(i==1)
	{
		direction++;
		if(direction==4)
		{
			direction=0;
		}
	}
	else
	{
		direction--;
		if(direction==-1)
		{
			direction=3;
		}
	}
}

/*
*This is a helper function that will take shaders info as a parameter, compiles them and links them
*into a shader program.
*
*This function is standard and should not change from one program to the next.
*/

GLuint View::createShaders(ShaderInfo *shaders)
{
	ifstream file;
	GLuint shaderProgram;
	GLint linked;

	ShaderInfo *entries = shaders;

	shaderProgram = glCreateProgram();


	while (entries->type !=GL_NONE)
	{
		file.open(entries->filename.c_str());
		GLint compiled;


		if (!file.is_open())
			return false;

		string source,line;


		getline(file,line);
		while (!file.eof())
		{
			source = source + "\n" + line;
			getline(file,line);
		}
		file.close();


		const char *codev = source.c_str();


		entries->shader = glCreateShader(entries->type);
		glShaderSource(entries->shader,1,&codev,NULL);
		glCompileShader(entries->shader);
		glGetShaderiv(entries->shader,GL_COMPILE_STATUS,&compiled);

		if (!compiled)
		{
			printShaderInfoLog(entries->shader);
			for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
			{
				glDeleteShader(processed->shader);
				processed->shader = 0;
			}
			return 0;
		}
		glAttachShader( shaderProgram, entries->shader );
		entries++;
	}

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram,GL_LINK_STATUS,&linked);

	if (!linked)
	{
		printShaderInfoLog(entries->shader);
		for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
		{
			glDeleteShader(processed->shader);
			processed->shader = 0;
		}
		return 0;
	}

	return shaderProgram;
}

void View::printShaderInfoLog(GLuint shader)
{
	int infologLen = 0;
	int charsWritten = 0;
	GLubyte *infoLog;

	glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infologLen);
	//	printOpenGLError();
	if (infologLen>0)
	{
		infoLog = (GLubyte *)malloc(infologLen);
		if (infoLog != NULL)
		{
			glGetShaderInfoLog(shader,infologLen,&charsWritten,(char *)infoLog);
			printf("InfoLog: %s\n\n",infoLog);
			free(infoLog);
		}

	}
	//	printOpenGLError();
}

void View::getOpenGLVersion(int *major,int *minor)
{
	const char *verstr = (const char *)glGetString(GL_VERSION);
	if ((verstr == NULL) || (sscanf_s(verstr,"%d.%d",major,minor)!=2))
	{
		*major = *minor = 0;
	}
}

void View::getGLSLVersion(int *major,int *minor)
{
	int gl_major,gl_minor;

	getOpenGLVersion(&gl_major,&gl_minor);
	*major = *minor = 0;

	if (gl_major==1)
	{
		/* GL v1.x can only provide GLSL v1.00 as an extension */
		const char *extstr = (const char *)glGetString(GL_EXTENSIONS);
		if ((extstr!=NULL) && (strstr(extstr,"GL_ARB_shading_language_100")!=NULL))
		{
			*major = 1;
			*minor = 0;
		}
	}
	else if (gl_major>=2)
	{
		/* GL v2.0 and greater must parse the version string */
		const char *verstr = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
		if ((verstr==NULL) || (sscanf_s(verstr,"%d.%d",major,minor) !=2))
		{
			*major = 0;
			*minor = 0;
		}
	}
}

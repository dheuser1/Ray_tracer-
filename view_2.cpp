#include "view_2.h"
#include <GL/glew.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cmath>
#include<iostream>
#include<vector>
#include<sstream>
#include<fstream>
#include<cstdlib>
#include"Maze.h"

using namespace std;
#include "utils/TriangleMesh.h"
//glm headers to access various matrix producing functions, like ortho below in resize
#include <glm/gtc/matrix_transform.hpp>
//the glm header required to convert glm objects into normal float/int pointers expected by OpenGL
//see value_ptr function below for an example
#include <glm/gtc/type_ptr.hpp>

view_2::view_2()
{
	meshObject = NULL;
	meshObject_solved = NULL;
	meshObject_spot = NULL;
	cam_pos[1] = 150;
	theta = 0;
	my_maze = new Maze();
	min_win=600;
	row=my_maze->get_row();
	col=my_maze->get_col();
	vert_mtrx=my_maze->get_vert_mtrx();
	aspect = (row<col)?col:row;
	ans=my_maze->get_ans();
	cam_x=-3;
	cam_y=25;
}

view_2::~view_2()
{
	//to_file();
	if(my_maze!=NULL)
	{
		delete my_maze;
	}

	if (meshObject!=NULL)
	{
		delete meshObject;
	}
	if (meshObject_solved!=NULL)
	{
		delete meshObject_solved;
	}
	if (meshObject_spot!=NULL)
	{
		delete meshObject_spot;
	}
}

void view_2::VectorAssign(int lesserx, int greaterx, int lessery, int greatery)
{
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			if((i<lessery || i>greatery) || (j<lesserx ||j>greaterx))
			{
				//vertical walls
				if(vert_mtrx[i][j].at(0)=='1')
				{
					lines.push_back(i*(col+1)+j);
					lines.push_back((i+1)*(col+1)+j);
				}
				//horizontal walls
				if(vert_mtrx[i][j].at(1)=='1')
				{
					lines.push_back(i*(col+1)+j);
					lines.push_back(i*(col+1)+j+1);
				}
				//get edge vertical walls
				if(i<row && j==col-1 && vert_mtrx[i][j].at(2)=='1')
				{
					lines.push_back(i*(col+1)+j+1);
					lines.push_back((i+1)*(col+1)+j+1);
				}
				//get edge horizontal walls
				if(i==row-1 && j<col && vert_mtrx[i][j].at(3)=='1')
				{
					lines.push_back((i+1)*(col+1)+j);
					lines.push_back((i+1)*(col+1)+j+1);
				}
			}
			//this is the outline of the rectangle
			//it is a bit buggy if this is disabled it works but will not draw the outline
			//start disabling here
			/*

			int range = greaterx-lesserx;
			range = range>0?range:1;
			int rand_1 = rand()%range + lesserx;
			int rand_2=rand()%range + lesserx;
			//get top
			if(i==greatery && j>=lesserx && j<=greaterx)
			{
			if(j!=rand_1)
			{
			lines.push_back((i+1)*(col+1)+j);
			lines.push_back((i+1)*(col+1)+j+1);
			}
			}
			//get bottom
			if(i==lessery && j>=lesserx && j<=greaterx)
			{
			if(j!=rand_2)
			{
			lines.push_back(i*(col+1)+j);
			lines.push_back(i*(col+1)+j+1);
			}
			}
			//get left
			if(j==lesserx && i>=lesserx && i<=greaterx)
			{
			lines.push_back(i*(col+1)+j);
			lines.push_back((i+1)*(col+1)+j);
			}
			if(j==greaterx && i>=lesserx && i<=greaterx)
			{
			lines.push_back(i*(col+1)+j+1);
			lines.push_back((i+1)*(col+1)+j+1);
			}
			*/
			//end  here to disable
		}   
	}
}

//
void view_2::update_spot()
{
	coordinates_spot.clear();
	lines_spot.clear();
	v.position = glm::vec4((cam_x+26+.25),(26+cam_y+.25),0,1);
	coordinates_spot.push_back(v);
	v.position = glm::vec4((cam_x+26+.75),(26+cam_y+.75),0,1);
	coordinates_spot.push_back(v);
	v.position = glm::vec4((cam_x+26+.25),(26+cam_y+.75),0,1);
	coordinates_spot.push_back(v);
	v.position = glm::vec4((cam_x+26+.75),(26+cam_y+.25),0,1);
	coordinates_spot.push_back(v);

	lines_spot.push_back(coordinates_spot.size()-2);
	lines_spot.push_back(coordinates_spot.size()-1);
	lines_spot.push_back(coordinates_spot.size()-4);
	lines_spot.push_back(coordinates_spot.size()-3);

	tmesh_spot.initVertexData(coordinates_spot);
	tmesh_spot.initTriangles(lines_spot);

	meshObject_spot = new graphics::Object(tmesh_spot);

	graphics::Material mat_spot;

	mat_spot.setAmbient(0,1,1);
	mat_spot.setDiffuse(0,1,1);
	mat_spot.setSpecular(0,1,1);

	meshObject_spot->setMaterial(mat_spot);
	//meshObject_spot->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1/500),min_win/(aspect+1/500),20)));
}

void view_2::initObjects()
{
	//push in vertices
	for(int i=row+1; i>0; i--)
	{
		for(int j=0; j<col+1; j++)
		{
			//can scale it here with fact*j, fact*i
			v.position = glm::vec4(j,i,0,1);
			coordinates.push_back(v);
		}
	}
	
	//go through each step to solve the maze
	for(int i=0; i<ans.size()-1; i++)
	{
		//add in verts at the center of the cell
		v.position = glm::vec4(ans[i][1]+.5,row-ans[i][0]+.5,0,1);
		coordinates_solved.push_back(v);
		v.position = glm::vec4(ans[i][3]+.5,row-ans[i][2]+.5,0,1);
		coordinates_solved.push_back(v);
		//make a line from the 2 new verts
		lines_solved.push_back(coordinates_solved.size()-2);
		lines_solved.push_back(coordinates_solved.size()-1);
	}

	

	VectorAssign(row,0, col, 0);

	//done with the cell data in this class, avoid dangling pointer
	update_spot();

	tmesh.initVertexData(coordinates);
	tmesh.initTriangles(lines);

	tmesh_solved.initVertexData(coordinates_solved);
	tmesh_solved.initTriangles(lines_solved);

	meshObject = new graphics::Object(tmesh);

	graphics::Material mat;

	mat.setAmbient(1,1,1);
	mat.setDiffuse(1,1,1);
	mat.setSpecular(1,1,1);

	meshObject->setMaterial(mat);

	//make another TriangleMesh that can be another color
	meshObject_solved = new graphics::Object(tmesh_solved);

	graphics::Material mat_solved;

	mat_solved.setAmbient(1,0,1);
	mat_solved.setDiffuse(1,0,1);
	mat_solved.setSpecular(1,0,1);

	meshObject_solved->setMaterial(mat_solved);

	//place the camera accordingly
	glm::vec4 minBounds = meshObject->getMinimumWorldBounds();
	glm::vec4 maxBounds = meshObject->getMaximumWorldBounds();
	float dist = glm::distance(maxBounds,minBounds);

	//set the size of the object. By default each mesh is in a cube of side 1 centered at the origin 

	//meshObject->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(600/(aspect+1),600/(aspect+1),20)));
	meshObject->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1/500),min_win/(aspect+1/500),20)));
	meshObject_solved->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1/500),min_win/(aspect+1/500),20)));
	camera.init(glm::vec3(dist,dist,2*dist),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	//lines_to_cells();
}

void view_2::lines_to_cells()
{
	int vert_index;

	//0 the data
	//save the bottom and far right, it can never be changed 
	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			vert_mtrx[i][j][0]='0';
			vert_mtrx[i][j][1]='0';
			if(i!=row-1)
			{
				vert_mtrx[i][j][3]='0';
			}
			if(j!=col-1)
			{
				vert_mtrx[i][j][2]='0';
			}
		}
	}

	for(int i=0; i<lines.size(); i++)
	{
		for(int j=0; j<row; j++)
		{
			for(int k=0; k<col; k++)
			{
				vert_index=col*j+k;

				if(lines[i]==vert_index+j || lines[i]==vert_index+j+1 || lines[i]==vert_index+col+j+1 || lines[i]==vert_index+col+j+1)
				{
					if(lines[i+1]==vert_index+j || lines[i+1]==vert_index+j+1 || lines[i+1]==vert_index+col+j+1 || lines[i+1]==vert_index+col+j+1)
					{
						//if this is hit it means that the cell has both verts in it
						//find which side it is
						//this is top side
						if((lines[i]==vert_index+j &&  lines[i+1]==vert_index+j+1) || (lines[i+1]==vert_index+j &&  lines[i]==vert_index+j+1))
						{
							vert_mtrx[j][k][1]='1';
							//top of one is bottom of another as long as it is not very top
							if(j>0)
							{
								vert_mtrx[j-1][k][3]='1';
							}
						}
						//far right
						if(k==col-1 && ((lines[i]==vert_index+col+j+1 && lines[i+1]==vert_index+col+j+2) || 
							(lines[i+1]==vert_index+col+j+1 && lines[i]==vert_index+col+j+2)))
						{
							vert_mtrx[j][k][2]='1';
							cout<<"far right at "<<j<<" "<<k<<endl;
						}
						//far down
						if(j==row-1 && ((lines[i]==vert_index+j+1 && lines[i+1]==vert_index+col+j+2) || 
							(lines[i+1]==vert_index+j+1 && lines[i]==vert_index+col+j+2)))
						{
							vert_mtrx[j][k][3]='1';
						}
						//left side
						if((lines[i]==vert_index+j &&  lines[i+1]==vert_index+col+j+1) ||
							(lines[i+1]==vert_index+j &&  lines[i]==vert_index+col+j+1))
						{
							vert_mtrx[j][k][0]='1';
							//left of one is right of another as long as it is not vert left
							if(k>0)
							{
								vert_mtrx[j][k-1][2]='1';
							}
						}
					}
				}
			}
		}
		i++;	
	}

	//need to make new mesh so get rid of old mesh
	if(meshObject!=NULL)
	{
		delete meshObject;
	}
	tmesh.initVertexData(coordinates);
	tmesh.initTriangles(lines);
	meshObject = new graphics::Object(tmesh);

	graphics::Material mat;

	mat.setAmbient(1,1,1);
	mat.setDiffuse(1,1,1);
	mat.setSpecular(1,1,1);

	meshObject->setMaterial(mat);

	meshObject->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1/500),min_win/(aspect+1/500),20)));
	draw();
}

void view_2::use_mouse_data(int mouse_x, int mouse_y, int code)
{
	//this is used to make the scaling work
	int adjust=min_win/(aspect+1);

	//pressed get first point
	if(code==0)
	{
		start_rec_x=mouse_x/adjust;
		start_rec_y=mouse_y/adjust;

		//get starting x and y
		square.push_back(mouse_x);
		square.push_back(mouse_y);

		//add starting vertex 
		v.position = glm::vec4((square[0]/adjust),(square[1]/adjust),0,1);
		coordinates.push_back(v);
	}
	//released 
	if(code==1)
	{
		//square 0 is small x
		//square 1 is big y
		//square 2 is big x
		//square 3 is small y
		end_rec_x=mouse_x/adjust;
		end_rec_y=mouse_y/adjust;

		//drawing outside of maze
		if(start_rec_x<0 || start_rec_x>col || end_rec_x<0 || end_rec_x>col||
			start_rec_y<1 || start_rec_y>row || end_rec_y<1 || end_rec_y>row)
		{
			square.pop_back();
			square.pop_back();
			if(meshObject!=NULL)
			{
				delete meshObject;
			}
			tmesh.initVertexData(coordinates);
			tmesh.initTriangles(lines);
			meshObject = new graphics::Object(tmesh);
			//reset mesh

			graphics::Material mat;

			mat.setAmbient(1,1,1);
			mat.setDiffuse(1,1,1);
			mat.setSpecular(1,1,1);

			meshObject->setMaterial(mat);

			meshObject->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1),min_win/(aspect+1),20)));
			draw();
			return;
		}

		//get ending x and y
		square.push_back(mouse_x);
		square.push_back(mouse_y);

		//get rid of the vertexs of type double
		coordinates.pop_back();
		//adding one to some of them is to make it expand to cover all cells it is over
		//add the int vertexs 

		int x1=0, x2=0, y1=0, y2=0;

		if(y1<0)
		{
			y1=0;
		}
		if(x1<0)
		{
			x1=0;
		}
		if(x2>col)
		{
			x2=col;
		}

		if(y2>row)
		{
			y2=row;
		}

		int greaterx=0, lesserx=0, greatery=0, lessery=0;

		if(square[0]>square[2])
		{
			lesserx=square[2]/adjust;
			greaterx=square[0]/adjust;
		}
		else
		{
			lesserx=square[0]/adjust;
			greaterx=square[2]/adjust;
		}

		if(square[1]>square[3])
		{
			lessery=row+1-square[1]/adjust;
			greatery=row+1-square[3]/adjust;
		}
		else
		{
			lessery=row+1-square[3]/adjust;
			greatery=row+1-square[1]/adjust;
		}

		if(lessery<0)
		{
			lessery=0;
		}
		if(lesserx<0)
		{
			lesserx=0;
		}
		if(greaterx>col)
		{
			greaterx=col;
		}
		if(greatery>row)
		{
			greatery=row;
		}
		lines.clear();

		VectorAssign(lesserx,greaterx,lessery,greatery);

		//need to make new mesh so get rid of old mesh
		if(meshObject!=NULL)
		{
			delete meshObject;
		}
		tmesh.initVertexData(coordinates);
		tmesh.initTriangles(lines);
		meshObject = new graphics::Object(tmesh);
		//reset mesh

		graphics::Material mat;

		mat.setAmbient(1,1,1);
		mat.setDiffuse(1,1,1);
		mat.setSpecular(1,1,1);

		meshObject->setMaterial(mat);

		meshObject->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1),min_win/(aspect+1),20)));
		draw();
		//this time do not delete keep rectange drawn 

		//but pop all 4 squares
		square.pop_back();
		square.pop_back();
		square.pop_back();
		square.pop_back();
		lines_to_cells();
	}
	//moved
	//works like released but keeps the origin and is in double form it will not snap to cells until the mouse is released
	if(code==2)
	{
		square.push_back(mouse_x);
		square.push_back(mouse_y);

		v.position = glm::vec4((square[2]/adjust),(square[1]/adjust),0,1);
		coordinates.push_back(v);
		v.position = glm::vec4((square[2]/adjust),(square[3]/adjust),0,1);
		coordinates.push_back(v);
		v.position = glm::vec4((square[0]/adjust),(square[3]/adjust),0,1);
		coordinates.push_back(v);

		lines.push_back(coordinates.size()-4);
		lines.push_back(coordinates.size()-3);

		lines.push_back(coordinates.size()-3);
		lines.push_back(coordinates.size()-2);

		lines.push_back(coordinates.size()-2);
		lines.push_back(coordinates.size()-1);

		lines.push_back(coordinates.size()-1);
		lines.push_back(coordinates.size()-4);

		//need to make new mesh so get rid of old mesh
		if(meshObject!=NULL)
		{
			delete meshObject;
		}
		tmesh.initVertexData(coordinates);
		tmesh.initTriangles(lines);
		meshObject = new graphics::Object(tmesh);

		graphics::Material mat;

		mat.setAmbient(1,1,1);
		mat.setDiffuse(1,1,1);
		mat.setSpecular(1,1,1);

		meshObject->setMaterial(mat);

		meshObject->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1),min_win/(aspect+1),20)));
		draw();

		//pop the 2 squares
		square.pop_back();
		square.pop_back();

		//pop the 3 coordinates
		coordinates.pop_back();
		coordinates.pop_back();
		coordinates.pop_back();

		//pop the 8 lines
		lines.pop_back();
		lines.pop_back();
		lines.pop_back();
		lines.pop_back();
		lines.pop_back();
		lines.pop_back();
		lines.pop_back();
		lines.pop_back();
	}
}

void view_2::resize(int w, int h)
{
	WINDOW_WIDTH = w;
	WINDOW_HEIGHT = h;
	aspect = (row<col)?col:row;

	if(w<h)
	{
		min_win=w;
	}
	else
	{
		min_win=h;
	}
	// *This program uses orthographic projection. The corresponding matrix for this projection is provided by the glm function below.
	//    proj = glm::perspective(120.0f,(float)WINDOW_WIDTH/WINDOW_HEIGHT,0.1f,10000.0f);
	proj = glm::ortho((float)0,(float)1*WINDOW_HEIGHT*3,(float)0,(float)1*WINDOW_HEIGHT*3,0.1f,10000.0f);

	//change transform for new window size
	if(meshObject!=NULL)
	{
		meshObject->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1)/50,min_win/(aspect+1)/50,20)));
		draw();
	}
	if(meshObject_solved!=NULL)
	{
		meshObject_solved->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1)/50,min_win/(aspect+1)/50,20)));
		draw();
	}
	if(meshObject_spot!=NULL)
	{
		meshObject_spot->setTransform(glm::scale(glm::mat4(1.0),glm::vec3(min_win/(aspect+1)/50,min_win/(aspect+1)/50,20)));
		draw();
	}

}

void view_2::to_file()
{
	ofstream outstream;
	string name = "maze-"+to_string(row)+"x"+to_string(col)+"output.txt";
	string temp;
	outstream.open(name);
	if(outstream.fail())
	{
		cout<<"opening failed"<<endl;
		exit(1);
	}
	outstream<<row<<endl;
	outstream<<col<<endl;
	outstream<<my_maze->get_start_end()<<endl;

	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			outstream<<to_string(dec(vert_mtrx[i][j]))<<" ";	
		}
		outstream<<endl;
	}
	outstream.close();
}

int view_2::dec(string base_2)
{
	int dec=0;
	if(base_2.at(0)=='1')
	{
		dec+=8;
	}
	if(base_2.at(1)=='1')
	{
		dec+=4;
	}
	if(base_2.at(2)=='1')
	{
		dec+=2;
	}
	if(base_2.at(3)=='1')
	{
		dec+=1;
	}
	return dec;
}

void view_2::initialize()
{
	//populate our shader information. The two files below are present in this project.
	ShaderInfo shaders[] =
	{
		{GL_VERTEX_SHADER,"default.vert"},
		{GL_FRAGMENT_SHADER,"default.frag"},
		{GL_NONE,""}
	};
	//call helper function, get the program shader ID if everything went ok.
	program = createShaders(shaders);

	if (program<=0)
		exit(0);

	initObjects();

	//use the above program. After this statement, any rendering will use this above program
	//passing 0 to the function below disables using any shaders
	glUseProgram(program);

	//assuming the program above is compiled and linked correctly, get IDs for all the input variables
	//that the shader expects our program to provide.
	//think of these IDs as references to those shader variables

	//the second parameter of glGetUniformLocation is a string that is the name of an actual variable in the shader program
	//this variable may exist in any of the shaders that are linked in this program.
	projectionLocation = glGetUniformLocation(program,"projection");
	modelViewLocation = glGetUniformLocation(program,"modelview");
	colorLocation = glGetUniformLocation(program,"vColor");

	glUseProgram(0);
}

//*Function called from display in main. Should contain or call all the code to draw one screen from scratch
//*entirely.
void view_2::draw()
{
	//        theta += 0.1;
	glUseProgram(program);

	glEnable(GL_LINE_SMOOTH);// or GL_POLYGON_SMOOTH
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); ;
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); //GL_FASTEST,GL_DONT_CARE

	//*We would like to scale up the triangles. We use glm::scale to get the scale matrix.
	//*We pass this along as the modelview matrix to the shader, which multiplies each incoming vertex's
	//*position with this matrix

	modelView = glm::lookAt(glm::vec3(0,0,10),glm::vec3(0,0,0),glm::vec3(0,1,0)) * meshObject->getTransform();

	float cam_radius = 20;

	//        modelView = glm::lookAt(glm::vec3(cam_radius*cos(glm::radians(theta)),cam_radius,cam_radius*sin(glm::radians(theta))),glm::vec3(0,0,0),glm::vec3(0,1,0)) * meshObject->getTransform();
	//  modelView = camera.getWorldToCameraTransform() * meshObject->getTransform();

	// modelView = glm::lookAt(glm::vec3(0,cam_radius,0),glm::vec3(0,0,0),glm::vec3(1,0,0)) * meshObject->getTransform();

	//*Supply the shader with all the matrices it expects.
	//*We already have all the pointers for those matrices from "init" above
	//*The value_ptr creates a copy-by-value of the glm::mat4 into an array of floats as OpenGL expects

	glUniformMatrix4fv(projectionLocation,1,GL_FALSE,glm::value_ptr(proj));

	glUniformMatrix4fv(modelViewLocation,1,GL_FALSE,glm::value_ptr(modelView));

	//material properties
	glUniform3fv(colorLocation,1,glm::value_ptr(meshObject->getMaterial().getAmbient()));

	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); //draw in line mode. Every triangle is drawn as 3 lines
	meshObject->draw(GL_LINES);

	//draw the second TriangleMesh
	modelView = glm::lookAt(glm::vec3(0,0,10),glm::vec3(0,0,0),glm::vec3(0,1,0)) * meshObject_solved->getTransform();
	glUniform3fv(colorLocation,1,glm::value_ptr(meshObject_solved->getMaterial().getAmbient()));

	//meshObject_solved->draw(GL_LINES);

	//draw the third TriangleMesh
	modelView = glm::lookAt(glm::vec3(0,0,10),glm::vec3(0,0,0),glm::vec3(0,1,0)) * meshObject_spot->getTransform();
	glUniform3fv(colorLocation,1,glm::value_ptr(meshObject_spot->getMaterial().getAmbient()));

	meshObject_spot->draw(GL_LINES);


	//*OpenGL batch-processes all its OpenGL commands.
	//*The next command asks OpenGL to "empty" its batch of issued commands, i.e. draw

	//*This a non-blocking function. That is, it will signal OpenGL to draw, but won't wait for it to
	//*finish drawing.

	//*If you would like OpenGL to start drawing and wait until it is done, call glFinish() instead.

	glFlush();

	glUseProgram(0);
}

// *This is a helper function that will take shaders info as a parameter, compiles them and links them
// *into a shader program.
// *This function is standard and should not change from one program to the next.
GLuint view_2::createShaders(ShaderInfo *shaders)
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

void view_2::printShaderInfoLog(GLuint shader)
{
	int infologLen = 0;
	int charsWritten = 0;
	GLubyte *infoLog;

	glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infologLen);
	//        printOpenGLError();
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
	//        printOpenGLError();
}

void view_2::getOpenGLVersion(int *major,int *minor)
{
	const char *verstr = (const char *)glGetString(GL_VERSION);
	if ((verstr == NULL) || (sscanf_s(verstr,"%d.%d",major,minor)!=2))
	{
		*major = *minor = 0;
	}
}

void view_2::getGLSLVersion(int *major,int *minor)
{
	int gl_major,gl_minor;

	getOpenGLVersion(&gl_major,&gl_minor);
	*major = *minor = 0;

	if (gl_major==1)
	{
		// GL v1.x can only provide GLSL v1.00 as an extension
		const char *extstr = (const char *)glGetString(GL_EXTENSIONS);
		if ((extstr!=NULL) && (strstr(extstr,"GL_ARB_shading_language_100")!=NULL))
		{
			*major = 1;
			*minor = 0;
		}
	}
	else if (gl_major>=2)
	{
		//* GL v2.0 and greater must parse the version string /
		const char *verstr = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
		if ((verstr==NULL) || (sscanf_s(verstr,"%d.%d",major,minor) !=2))
		{
			*major = 0;
			*minor = 0;
		}
	}
}
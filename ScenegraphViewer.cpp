/*
 * This is the main file. It sets up the window using sfml and initializes OpenGL extensions using glew
 *
 * We use sfml, an open-source library. SFML has very basic support for guis. There is SFGUI but I have not used it yet. 
 * SFML is by no means a full-fledged powerful UI framework, but it is very simple to use, and is cross-platform. But one of the things going for it is speed.
 * Feel free to use and modify this file for every project.
 *
 * SFML was written for C++ and opengl was written for C
 * In order to initialize OpenGL extensions to use OpenGL 4, we use glew. This simplifies setting it up for every program.
 **/
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "View.h"
#include "view_2.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;


/* function prototypes */
void init(string& filename);
void resize(int w,int h);
void display(sf::RenderWindow *window);
void processEvent(sf::Event event,sf::RenderWindow& window);
void drawText(sf::RenderWindow& window,string text,int x,int y);

View v; //an object to our View class that encapsulates everything that we do.
//view_2 v_2;
sf::Font font;
sf::Clock sfclock;
int frames;
double frame_rate;
bool mousePressed;
int mouseX,mouseY;

//string filename = "humanoid.xml";
string filename = "test.xml";
//string filename = "jonride.xml";
//string filename = "all.xml";
//string filename = "all-cone.xml";

int main(int argc, char *argv[])
{
	cout<<"press 4 to output a ray-traced picture, press 1 to return to opengl"<<endl;
	cout<<"img_test.png is the name of the picture that is made"<<endl;
	frames = 0;
	frame_rate = 0;
    // Request a 32-bits depth buffer when creating the window
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;
	contextSettings.majorVersion = 4;
	contextSettings.minorVersion = 0;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Scenegraph Viewer", sf::Style::Default, contextSettings);
	resize(800,600);
  //  window.setVerticalSyncEnabled(true);

    // Make it the active window for OpenGL calls
 	window.setActive();   

    //glew currently has a bug that causes problems with 4.0. Setting this glew flag is the temporary solution for it
    glewExperimental = GL_TRUE;

    //initialize glew which initializes all the OpenGL extensions.
    if (glewInit()!=GLEW_OK)
    {
        cerr << "Unable to initialize GLEW...exiting" << endl;
        return EXIT_FAILURE;
    }

    //initialize stuff. This will likely change with every program.
    init(filename);

	
// Start game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
			processEvent(event,window);

        }

		if (window.isOpen())
			display(&window);
    }

    

    return EXIT_SUCCESS;
}

/*
 * This will be the function that processes any events
*/

void processEvent(sf::Event event,sf::RenderWindow& window)
{
	// Close window : exit
	switch(event.type)
	{
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::KeyPressed:
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			window.close();
			break;
		case sf::Keyboard::R:
			v.openFile(filename);
			break;
		case sf::Keyboard::Num1:
			v.set_camera(1);
			break;
		
		case sf::Keyboard::Num2:
			v.clear();
			break;
			/*
		case sf::Keyboard::Num3:
			v.set_camera(3);
			break;
		*/
			case sf::Keyboard::Num4:
			v.set_camera(4);
			break;
		/*
		case sf::Keyboard::Up:
			v.pos_update(1);
			//v_2.update_x(v.get_x());
			//v_2.update_y(v.get_y());
			//v_2.update_spot();
			break;
		case sf::Keyboard::Down:
			v.pos_update(-1);
			//v_2.update_x(v.get_x());
			//v_2.update_y(v.get_y());
			//v_2.update_spot();
			break;
		case sf::Keyboard::Left:
			v.dir_update(-1);
			break;
		case sf::Keyboard::Right:
			v.dir_update(1);
			break;
			*/
		}
		break;
	case sf::Event::Resized:
		resize(event.size.width, event.size.height);
		break;
		case sf::Event::MouseButtonPressed:
		mousePressed = true;
		mouseX = sf::Mouse::getPosition(window).x;
		mouseY = window.getSize().y - sf::Mouse::getPosition(window).y;
		v.mousepress(mouseX,mouseY);
		break;
	case sf::Event::MouseButtonReleased:
		mousePressed = false;
		break;
	case sf::Event::MouseMoved:
		if (mousePressed)
		{
			mouseX = sf::Mouse::getPosition(window).x;
			mouseY = window.getSize().y - sf::Mouse::getPosition(window).y;
			v.mousemove(mouseX,mouseY);//sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y);
			
		}
		break;
	
	}
}

/*
 * This function helps us to draw text over a part of the window
 */

void drawText(sf::RenderWindow *window,string text,int x,int y)
{
	// Create some text to draw on top of our OpenGL object
  
    sf::Text textobj(text, font);
    textobj.setColor(sf::Color(255, 255, 255, 255));
    textobj.setPosition((float)x-60*10,(float)y);
	textobj.setCharacterSize(18);

	int steps=v.get_steps();
	string taken="Steps taken ";
	taken+=to_string(steps);
	sf::Text textobj_1(taken, font);
    textobj_1.setColor(sf::Color(255, 255, 255, 255));
    textobj_1.setPosition((float)x-60*10,(float)y+20);
	textobj_1.setCharacterSize(18);

	int optimal=v.get_solved_steps();
	string os="Optimal steps ";
	os+=to_string(optimal);
	sf::Text textobj_2(os, font);
    textobj_2.setColor(sf::Color(255, 255, 255, 255));
    textobj_2.setPosition((float)x-60*10,(float)y+40);
	textobj_2.setCharacterSize(18);

	

	window->pushGLStates();
	window->resetGLStates();
	window->draw(textobj);
	window->draw(textobj_1);
	window->draw(textobj_2);
    window->popGLStates();
}


/*
 * This will be our display function. Whenever glut needs to redraw the window, it will call this function.
 * The name of this function (display) is of no consequence: you can name it whatever you want, so long as the constraints below are obeyed.
 * Thus all your rendering code should be in this function, or should be called from this function.
 *
*/

void display(sf::RenderWindow *window)
{
	if (frames==0)
		sfclock.restart();
	
	// Draw using SFML
	window->pushGLStates();
	window->resetGLStates();
	//insert SFML drawing code here (any part you are using that does not involve opengl code)
	window->popGLStates();

	//set up the background color of the window. This does NOT clear the window. Right now it is (0,0,0) which is black
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //this command actually clears the window.
	glEnable(GL_DEPTH_TEST);
	v.draw(); //simply delegate to our view class that has all the data and does all the rendering
	glViewport(590,440,800,600);
	//v_2.draw();
	glViewport(0,0,800,600);


	if (frames>500)
	{
		sf::Time t = sfclock.getElapsedTime();
		frame_rate = frames/t.asSeconds();
		frames = 0;
	}
	else
	{
		frames++;
	}
	stringstream str;

	str << "Frame rate " << frame_rate;
	// Draw some text on top of our OpenGL object
	drawText(window,str.str(),window->getSize().x,20);
    
	
	// Finally, display the rendered frame on screen
	window->display();
//	cout << "Rendering" << endl;
}

/*
 * This function will be called by glut whenever the window resizes. This happens initially when it creates the window, and later if the user manually resizes the window or maximizes it.
 * The name of this function (resize) is of no consequence: you can name it whatever you want, so long as the constraints below are obeyed.
 * This function must take two integers as parameters, the width and height of the resized window in that order
 * This function must return void.
 **/
void resize(int w,int h)
{
    //delegate to our view class.
    v.resize(w,h);
	//glViewport(0,0,200,200);
	//v_2.resize(200,200);

    //sets the viewport to cover the entire area of the resized window
    //glViewport(leftx,topy,width,height)
    glViewport(0,0,w,h);
}

void init(string& filename)
{
    int major,minor;
    v.getOpenGLVersion(&major,&minor);

    cout <<"Opengl version supported : "<<major<<"."<<minor<<endl;
    v.getGLSLVersion(&major,&minor);
    cout << "GLSL version supported : "<<major<<"."<<minor << endl;
    //delegate to our view class to do all the initializing
	v.initialize();
	//v_2.initialize();
	v.openFile(filename);

	if (!font.loadFromFile("resources/GARA.ttf"))
		return;

}

#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
#include<iostream>
#include"Ray.h"
#include"HitRecord.h"
#define GLM_FORCE_RADIANS
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <sstream>

Scenegraph::Scenegraph()
{
	root = NULL;
	nodes_found=false;
	cone_found=false;
	flag_found=false;
}

void Scenegraph::ray_trace(int w, int h, stack<glm::mat4>& modelView)
{
	//w is 1 for points, is 0 for vector
	//z of cam position should be >1
	cout << "start raytracing" << endl;
	sf::Uint8* pic = new sf::Uint8[w*h*4];
	//test color
	sf::Uint8 test_1=0;
	sf::Uint8 test_2=10;
	sf::Uint8 test_3=255;
	sf::Uint8 test_4=255;


	//pic has the right values
	/*
	for(int i=0; i<w*h*4; i++)
	{
	cout<<(int)pic[i]<<endl;
	}
	*/

	Ray my_ray;
	my_ray.setPoint(glm::vec4(0, 0, 0, 1 ));
	for(int i=h-1; i>=0; i--)
	{
		if(i%50==0)
		{
			cout<<"working on row "<<i<<endl;
		}
		for(int j=0; j<w; j++)
		{

			//cout<<"("<<j-width/2+.5<<","<<i-hight/2+.5<<")";
			//the z should be -.5h/tan(FOV/2)
			//if FOV=120 than tan(120/2)=root(3)=1.732
			my_ray.setVector(glm::vec4(j-w/2+.5, i-h/2+.5, -.5*h/1.732, 0));
			if (i==h/2 && j==w/2)
				//my_ray.setVector(glm::vec4(0, 0, -.5*h / 1.732, 0));
					cout << "";
			glm::vec3 color=raycast(my_ray, modelView, 0, 0, -1);
			//	if (w*i + j == 0)
			//		cout << endl;
			pic[(h-i-1)*w*4+j*4] = (sf::Uint8) (color.r*255);
			pic[(h-i-1)*w*4+j*4+1] =  (sf::Uint8) (color.g * 255);
			pic[(h-i-1)*w*4+j*4+2] =  (sf::Uint8) (color.b * 255);
			pic[(h-i-1)*w*4+j*4+3] = 255;
		}
	}

	//only makes white pic but it is the right size
	string name="img_test.png";
	sf::Image the_pic;
	the_pic.create((unsigned int)w, (unsigned int)h, pic);
	the_pic.saveToFile(name); 
	delete[] pic;
	pic=NULL;
	cout<<"made pic"<<endl;
}

void Scenegraph::makeScenegraph(Node *r)
{
	nodes_found = false;
	if (root!=NULL)
	{
		delete root;
		root = NULL;
	}
	this->root = r;

	if (this->root!=NULL)
		this->root->setScenegraph(this);
}



Scenegraph::~Scenegraph()
{
	if (root!=NULL)
	{
		delete root;
		root = NULL;
	}

	for (map<string,graphics::Texture *>::iterator it=textures.begin();it!=textures.end();it++)
	{
		delete it->second;
	}
}

void Scenegraph::initShaderProgram(GLint shaderProgram)
{
	// objectColorLocation = glGetUniformLocation(shaderProgram, "vColor");
	modelviewLocation = glGetUniformLocation(shaderProgram,"modelview");
	normalMatrixLocation = glGetUniformLocation(shaderProgram, "normalmatrix");
	// texturematrixLocation = glGetUniformLocation(shaderProgram, "texturematrix"); broken...
	numLightsLocation = glGetUniformLocation(shaderProgram, "numLights");

	mat_ambientLocation = glGetUniformLocation(shaderProgram, "material.ambient");
	mat_diffuseLocation = glGetUniformLocation(shaderProgram, "material.diffuse");
	mat_specularLocation = glGetUniformLocation(shaderProgram, "material.specular");
	mat_shininessLocation = glGetUniformLocation(shaderProgram, "material.shininess");
	textureLocation = glGetUniformLocation(shaderProgram, "image");

}


glm::vec3 Scenegraph::raycast(Ray r, stack<glm::mat4>& modelView, int bounce, int t_bounce, int caller)
{
	HitRecord hit; 
	if(caller==0)
	{
		bounce++;
	}
	if(caller==1)
	{
		t_bounce++;
	}

	if(bounce>5 || t_bounce>5)
	{
		return glm::vec3(0, 0, 0);
	}
	bool rayhit=root->intersect(modelView, hit, r); 
	if (rayhit)
	{ 
		//if (r.getVector().x<1 && r.getVector().x> -1 && r.getVector().y<1 && r.getVector().y>-1)
		//cout << "";
		vector<graphics::Light> lightvector;
		root->getlights(lightvector, modelView);
		glm::vec3 color = shade(r, lightvector, hit, modelView, bounce, t_bounce, caller);
		//glm::vec3 color = glm::vec3(1, 0, 0);
		return color;
	}
	else
	{
		return glm::vec3(0, 0, 0);
	}
}

//only want to see if anything is between point and light 
bool Scenegraph::raycast_bool(Ray r, stack<glm::mat4>& modelView)
{
	HitRecord hit; 
	bool rayhit=root->intersect(modelView, hit, r); 
	if (rayhit && hit.getT()<1.0 && hit.getT()>0.0)
	{ 
		return true;
	}
	else
	{
		return false;
	}
}
glm::vec3 Scenegraph::shade(Ray r, vector<graphics::Light> lightvector, HitRecord hit, stack<glm::mat4>& modelView, int bounce, int t_bounce, int caller)
{
	glm::vec3 lightVec, viewVec, reflectVec;
	glm::vec3 normalView;
	glm::vec3 ambient, diffuse, specular;
	float nDotL, rDotV;


	glm::vec4 fColor = glm::vec4(0, 0, 0, 1);
	glm::vec4 bColor = glm::vec4(0, 0, 0, 1);
	glm::vec4 tColor = glm::vec4(0, 0, 0, 1);
	normalView = glm::vec3(hit.getnormal());
	viewVec = glm::vec3(-hit.getipoint());
	viewVec = glm::normalize(viewVec);

	Ray shadow_ray;
	Ray reflect_ray;
	Ray transparent_ray;
	//transparent not quite right to test use <translate>-60 -40 50 </translate>
	/*
	if(hit.getmaterial().getTransparency()!=0)
	{
		glm::vec3 temp;
		if(t_bounce%2==1)
		{
			float index=1.0/hit.getmaterial().getRefractiveIndex();
			float ni=hit.getnormal().x*r.getVector().x+hit.getnormal().y*r.getVector().y+hit.getnormal().z*r.getVector().z;
			float cos_t=sqrt(((1.0f-index*index)*(1.0f-(ni*ni))));
			if(cos_t>0.0)
			{
				transparent_ray.setVector(glm::refract(glm::normalize(r.getVector()), glm::normalize(hit.getnormal()), index));
				//transparent_ray.setVector(index*r.getVector()-(cos_t+index*ni)*hit.getnormal());
				transparent_ray.setPoint(hit.getipoint()+transparent_ray.getVector()*.01f);

				temp=raycast(transparent_ray, modelView ,bounce, t_bounce, 1);

				tColor.x=temp.x;
				tColor.y=temp.y;
				tColor.z=temp.z;
			}
		}
		else
		{
			float index=hit.getmaterial().getRefractiveIndex();
			glm::vec4 invert_n=hit.getnormal()*(-1.0f);
			float ni=invert_n.x*r.getVector().x+invert_n.y*r.getVector().y+invert_n.z*r.getVector().z;
			float cos_t=sqrt(((1.0-index*index)*(1.0f-(ni*ni))));
			if(cos_t>0.0)
			{
				transparent_ray.setVector(glm::refract(glm::normalize(r.getVector()), glm::normalize(invert_n), index));
				//transparent_ray.setVector(index*r.getVector()-(cos_t+index*ni*invert_n));
				transparent_ray.setPoint(hit.getipoint()+transparent_ray.getVector()*.01f);

				temp=raycast(transparent_ray, modelView ,bounce, t_bounce, 1);

				tColor.x=temp.x;
				tColor.y=temp.y;
				tColor.z=temp.z;
			}
		}
	}
	*/
	//reflections
	if(hit.getmaterial().getReflection()!=0)
	{
		//bounce++;
		//reflect_ray.setPoint(hit.getipoint());
		reflect_ray.setVector(glm::reflect(r.getVector(),hit.getnormal()));
		//fudge reflection 
		reflect_ray.setPoint(hit.getipoint()+reflect_ray.getVector()*.0001f);
		glm::vec3 temp=raycast(reflect_ray, modelView, bounce, t_bounce, 0);
		bColor.x=temp.x;
		bColor.y=temp.y;
		bColor.z=temp.z;
	}



	for (int i = 0; i < lightvector.size(); i++)
	{

		if (lightvector[i].getPosition().w != 0)
			lightVec = glm::normalize(glm::vec3(lightvector[i].getPosition() - hit.getipoint()));//glm::vec3(lightvector[i].getPosition().x - hit.getipoint().x, lightvector[i].getPosition().y - hit.getipoint().y, lightvector[i].getPosition().z - hit.getipoint().z));
		else
			lightVec = glm::normalize(glm::vec3(-lightvector[i].getPosition()));//-lightvector[i].getPosition().x, -lightvector[i].getPosition().y, -lightvector[i].getPosition().z));

		//shadows
		//shadow_ray.setPoint(hit.getipoint());
		//fudge shoadow 
		shadow_ray.setVector(lightvector[i].getPosition()-hit.getipoint());
		shadow_ray.setPoint(hit.getipoint()+(lightvector[i].getPosition()-shadow_ray.getPoint())*.0001f);

		bool shadow_hit=raycast_bool(shadow_ray, modelView);

		if(shadow_hit==false)
		{
			nDotL = glm::dot(normalView, lightVec);
			glm::vec3 a = glm::vec3(-lightVec);//-lightVec.x, -lightVec.y, -lightVec.z);
			reflectVec = a - 2 * glm::dot(a, normalView)*normalView;
			reflectVec = glm::normalize(reflectVec);

			rDotV = max(glm::dot(reflectVec, viewVec), 0.0f);

			ambient = glm::vec3(hit.getmaterial().getAmbient()) * lightvector[i].getAmbient();
			diffuse = glm::vec3(hit.getmaterial().getDiffuse()) * lightvector[i].getDiffuse() * max(nDotL, 0.0f);
			if (nDotL > 0)
				specular = glm::vec3(hit.getmaterial().getSpecular()) * lightvector[i].getSpecular() * pow(rDotV, hit.getmaterial().getShininess());
			else
				specular = glm::vec3(0, 0, 0);
			fColor = fColor + glm::vec4(ambient + diffuse + specular, 1.0)*hit.getmaterial().getAbsorption()
				+ bColor*hit.getmaterial().getReflection()+tColor*hit.getmaterial().getTransparency() ;
			if(fColor.x>1)
			{
				fColor.x=1.0f;
			}
			if(fColor.y>1)
			{
				fColor.y=1.0f;
			}
			if(fColor.z>1)
			{
				fColor.z=1.0f;
			}

			if(hit.get_has_tex()==true)
			{
				fColor*=hit.get_texcolor();
			}

		}
	}


	return glm::vec3(fColor);
}
void Scenegraph::draw(stack<glm::mat4>& modelView, GLint shaderProgram)
{
	/*modelView.pop();
	modelView.push(glm::mat4(1.0));
	Ray r = Ray();
	HitRecord hit = HitRecord();
	r.setVector(glm::vec4(1, 0, 0, 1));
	bool inter = root->intersect(modelView, hit, r);*/

	if (root != NULL)
	{
		vector<graphics::Light> lightvector;
		root->getlights(lightvector, modelView);
		LightLocation lightLocation[5];

		for (int i = 0; i<lightvector.size(); i++)
		{
			stringstream name;

			name << "light[" << i << "].ambient";

			lightLocation[i].ambientLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

			name << "light[" << i << "].diffuse";

			lightLocation[i].diffuseLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

			name << "light[" << i << "].specular";

			lightLocation[i].specularLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

			name << "light[" << i << "].position";

			lightLocation[i].positionLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

		}
		glUniform1i(numLightsLocation, lightvector.size());

		//light properties
		for (int i = 0; i<lightvector.size(); i++)
		{
			glUniform3fv(lightLocation[i].ambientLocation, 1, glm::value_ptr(lightvector[i].getAmbient()));
			glUniform3fv(lightLocation[i].diffuseLocation, 1, glm::value_ptr(lightvector[i].getDiffuse()));
			glUniform3fv(lightLocation[i].specularLocation, 1, glm::value_ptr(lightvector[i].getSpecular()));
			glUniform4fv(lightLocation[i].positionLocation, 1, glm::value_ptr(lightvector[i].getPosition()));
		}
	}



	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureID);
	//glUniform1i(textureLocation, 0); //bind GL_TEXTURE0 to sampler2D (whatever is bound to GL_TEXTURE0)

	if (root!=NULL)
	{
		root->draw(modelView);
	}

	/*if (root!=NULL)
	{
	root->updateBB();
	root->drawBB(modelView);
	}*/
}
void Scenegraph::animate_cone(int x, int y)
{
	if(cone_found==false)
	{
		cone = root->getNode("spot");
		cone_found=true;
	}
	TransformNode* t_ride= dynamic_cast<TransformNode*>(cone);
	if(t_ride!=NULL)
	{

		t_ride->setAnimationTransform(glm::translate(glm::mat4(1.0), glm::vec3(x, y, 0)));

	}
}

void Scenegraph::animate_flag(int x, int y)
{
	if(flag_found==false)
	{
		flag = root->getNode("flag");
		flag_found=true;
	}
	TransformNode* t_ride= dynamic_cast<TransformNode*>(flag);
	if(t_ride!=NULL)
	{

		t_ride->setAnimationTransform(glm::translate(glm::mat4(1.0), glm::vec3(x, y, 0)));

	}
}

void Scenegraph::animate(float time)
{

	if (nodes_found == false)
	{
		//only want to find them once to make the framerate better
		nodes_found = true;
		ride = root->getNode("ride_11");
		seat_0 = root->getNode("move_seat_0");
		seat_1 = root->getNode("move_seat_1");
		seat_2 = root->getNode("move_seat_2");
		seat_3 = root->getNode("move_seat_3");
		wheel = root->getNode("wheeltransform");
		j_seat_0 = root->getNode("seattransformone");
		j_seat_1 = root->getNode("seattransformtwo");
		j_seat_2 = root->getNode("seattransformthree");
		j_seat_3 = root->getNode("seattransformfour");
	}

	TransformNode* t_ride = dynamic_cast<TransformNode*>(ride);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(3000.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_0);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(8000.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_1);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(10000.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_2);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(12000.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_3);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(18000.0f*time), glm::vec3(0, 1, 0)));
	}



	TransformNode * twheel = dynamic_cast<TransformNode*> (wheel);

	if (twheel != NULL)
	{
		twheel->setAnimationTransform(glm::translate(glm::mat4(1.0), glm::vec3(0, 100, 0))*glm::rotate(glm::mat4(1.0), glm::radians(1500.0f*time), glm::vec3(1, 0, 0))*glm::translate(glm::mat4(1.0), glm::vec3(0, -100, 0)));
	}

	TransformNode * tseat = dynamic_cast<TransformNode*> (j_seat_0);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(2200.0f*time), glm::vec3(0, 1, 0)));
	}

	tseat = dynamic_cast<TransformNode*> (j_seat_1);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(2200.0f*time), glm::vec3(0, 1, 0)));
	}

	tseat = dynamic_cast<TransformNode*> (j_seat_2);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(2200.0f*time), glm::vec3(0, 1, 0)));
	}

	tseat = dynamic_cast<TransformNode*> (j_seat_3);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(2200.0f*time), glm::vec3(0, 1, 0)));
	}
}


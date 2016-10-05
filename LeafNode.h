#ifndef _LEAF_H_
#define _LEAF_H_
#include "node.h"
#include "utils/Object.h"
#include "utils/Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include "utils/Texture.h"
#include <string>
#include <math.h>


class LeafNode : public Node
{
public:

	LeafNode(graphics::Object *instanceOf,Scenegraph *graph,string name="")
		:Node(graph,name)
	{
		this->texture = NULL;
		this->instanceOf = instanceOf;
		//default material
		material.setAmbient(1.0f,0.6f,0.6f);
		material.setDiffuse(1.0f,0.6f,0.6f);
		material.setSpecular(0.2f,0.1f,0.1f);
		material.setShininess(1);
	}

	~LeafNode(void)
	{
	}

	Node *clone()
	{
		LeafNode *newclone = new LeafNode(instanceOf,scenegraph,name);
		newclone->setMaterial(material);

		return newclone;
	}
	virtual void getNodeAni(glm::mat4& modelView)
	{
		if (parent != NULL)
			parent->getNodeAni(modelView);
		return;

	}
	virtual bool intersect(stack<glm::mat4>& modelView, HitRecord& hit, Ray r)
	{
		float red,green,blue;
		//if (r.getVector().x < 1 && r.getVector().x > -1 && r.getVector().y > -1 && r.getVector().y < 1)
			//cout << endl;
		r.setPoint(glm::inverse(modelView.top())*r.getPoint());
		r.setVector(glm::inverse(modelView.top())*r.getVector());
		
		if (instanceOf->getName().compare("box")==0)
		{
			float xtmin = -FLT_MAX;
			float xtmax = FLT_MAX;
			float ytmin = -FLT_MAX;
			float ytmax = FLT_MAX;
			float ztmin = -FLT_MAX;
			float ztmax = FLT_MAX;
			float tmin, tmax;
			if (r.getVector().x != 0)
			{
				xtmin = (-0.5 - r.getPoint().x) / r.getVector().x;
				xtmax = (0.5 - r.getPoint().x) / r.getVector().x;
				if (xtmin > xtmax)
					std::swap(xtmin, xtmax);
			}
			if (r.getVector().y != 0)
			{
				ytmin = (-0.5 - r.getPoint().y) / r.getVector().y;
				ytmax = (0.5 - r.getPoint().y) / r.getVector().y;
				if (ytmin > ytmax)
					std::swap(ytmin, ytmax);
			}
			if (r.getVector().z != 0)
			{
				ztmin = (-0.5 - r.getPoint().z) / r.getVector().z;
				ztmax = (0.5 - r.getPoint().z) / r.getVector().z;

				if (ztmin > ztmax)
					std::swap(ztmin, ztmax);
			}

			tmin = std::max(ztmin, std::max(xtmin, ytmin));
			tmax = std::min(ztmax, std::min(xtmax, ytmax));
			if (tmin < 0 && tmax < 0)
				return false;

			if (tmin <= tmax)
			{
				if (tmin < 0)
					tmin = tmax;

				if (tmin < hit.getT())
				{
					hit.setipoint(glm::vec4(r.getPoint().x + tmin*r.getVector().x, r.getPoint().y + tmin*r.getVector().y, r.getPoint().z + tmin*r.getVector().z, 1.0));
					hit.setT(tmin);
					hit.setmaterial(material);
					if (abs(hit.getipoint().x - 0.5f) < 0.000001f)
					{
						
						hit.setnormal(glm::vec4(glm::normalize(glm::vec3(glm::inverse(glm::transpose(modelView.top()))*glm::vec4(1, 0, 0, 0))), 0));
						if(texture!=NULL)
						{
							texture->lookup(1-(hit.getipoint().y+.5),1-(hit.getipoint().z+.5),red,green,blue);
							hit.has_tex=true;
						}
						hit.set_texcolor(glm::vec4(red,green,blue,1.0));
					}
					if (abs(hit.getipoint().x + 0.5f) < 0.000001f)
					{
						hit.setnormal(glm::vec4(glm::normalize(glm::vec3(glm::inverse(glm::transpose(modelView.top()))*glm::vec4(-1, 0, 0, 0))), 0));
						if(texture!=NULL)
						{
							texture->lookup(1-(hit.getipoint().y+.5),(hit.getipoint().z+.5),red,green,blue);
							hit.has_tex=true;
						}
						hit.set_texcolor(glm::vec4(red,green,blue,1.0));
					}
					if (abs(hit.getipoint().y - 0.5f) < 0.000001f)
					{
						hit.setnormal(glm::vec4(glm::normalize(glm::vec3(glm::inverse(glm::transpose(modelView.top()))*glm::vec4(0, 1, 0, 0))), 0));
						if(texture!=NULL)
						{
							texture->lookup((hit.getipoint().z+.5),(hit.getipoint().x+.5),red,green,blue);
							hit.has_tex=true;
						}
						hit.set_texcolor(glm::vec4(red,green,blue,1.0));
					}
					if (abs(hit.getipoint().y + 0.5f) < 0.000001f)
					{
						hit.setnormal(glm::vec4(glm::normalize(glm::vec3(glm::inverse(glm::transpose(modelView.top()))*glm::vec4(0, -1, 0, 0))), 0));
						if(texture!=NULL)
						{
							texture->lookup((hit.getipoint().z+.5),(1-(hit.getipoint().x+.5)),red,green,blue);
							hit.has_tex=true;
						}
						hit.set_texcolor(glm::vec4(red,green,blue,1.0));
					}
					if (abs(hit.getipoint().z - 0.5f) < 0.000001f)
					{
						hit.setnormal(glm::vec4(glm::normalize(glm::vec3(glm::inverse(glm::transpose(modelView.top()))*glm::vec4(0, 0, 1, 0))),0));
						if(texture!=NULL)
						{
							texture->lookup(1-(hit.getipoint().y+.5),((hit.getipoint().x+.5)),red,green,blue);
							hit.has_tex=true;
						}
						hit.set_texcolor(glm::vec4(red,green,blue,1.0));
					}
					if (abs(hit.getipoint().z + 0.5f) < 0.000001f)
					{
						hit.setnormal(glm::vec4(glm::normalize(glm::vec3(glm::inverse(glm::transpose(modelView.top()))*glm::vec4(0, 0, -1, 0))), 0));
						if(texture!=NULL)
						{
							texture->lookup((hit.getipoint().y+.5),((hit.getipoint().x+.5)),red,green,blue);
							hit.has_tex=true;
						}
						hit.set_texcolor(glm::vec4(red,green,blue,1.0));
					}
					hit.setipoint(modelView.top()*glm::vec4(r.getPoint().x + tmin*r.getVector().x, r.getPoint().y + tmin*r.getVector().y, r.getPoint().z + tmin*r.getVector().z, 1.0));
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		else if (instanceOf->getName().compare("sphere")==0)
		{
			
			float a = pow(r.getVector().x, 2) + pow(r.getVector().y, 2) + pow(r.getVector().z, 2);
			float b = 2 * (r.getVector().x*r.getPoint().x + r.getVector().y*r.getPoint().y + r.getVector().z*r.getPoint().z);
			float c = pow(r.getPoint().x, 2) + pow(r.getPoint().y, 2) + pow(r.getPoint().z, 2) - 1;

			if (pow(b, 2) - 4 * a*c < 0)
			{
				return false;
			}
			float tmin = (-b - sqrt(pow(b, 2) - 4 * a*c)) / (2 * a);
			if (tmin < 0)
				tmin = (-b + sqrt(pow(b, 2) - 4 * a*c)) / (2 * a);
			if (tmin < 0)
				return false;
			if (tmin < hit.getT())
			{
				hit.setT(tmin);
				hit.setmaterial(material);
				hit.setipoint(modelView.top()*glm::vec4(r.getPoint().x + tmin*r.getVector().x, r.getPoint().y + tmin*r.getVector().y, r.getPoint().z + tmin*r.getVector().z, 1.0));
				hit.setnormal(glm::vec4(glm::normalize(glm::vec3(glm::inverse(glm::transpose(modelView.top()))*glm::vec4(r.getPoint().x + tmin*r.getVector().x, r.getPoint().y + tmin*r.getVector().y, r.getPoint().z + tmin*r.getVector().z, 0))),0));
				//glm::vec4 temp=glm::vec4(r.getPoint().x + tmin*r.getVector().x, r.getPoint().y + tmin*r.getVector().y, r.getPoint().z + tmin*r.getVector().z, 1.0);
				glm::vec4 temp=glm::vec4(r.getPoint()+tmin*r.getVector());
				if(texture!=NULL)
				{
					float pi=3.14159;
					float temp_a=(asin(temp.y)+(pi/2))/pi;
					float temp_b=atan2(temp.z,temp.x)/(2.0*pi);
				
					texture->lookup(1-temp_b,temp_a,red,green,blue);
					hit.has_tex=true;
				}
				hit.set_texcolor(glm::vec4(red,green,blue,1.0));

				return true;
			}
			else
			{
				return false;
			}
		}

	}
	virtual void getlights(vector<graphics::Light>& lightvector, stack<glm::mat4> &modelView)
	{
		for (int i = 0; i < lights.size(); i++)
		{
			graphics::Light l;
			l.setAmbient(lights[i].getAmbient());
			l.setDiffuse(lights[i].getDiffuse());
			l.setPosition(lights[i].getPosition().x, lights[i].getPosition().y, lights[i].getPosition().z);
			l.setSpecular(lights[i].getSpecular());
			l.setSpotAngle(lights[i].getSpotAngle());
			l.setSpotDirection(glm::vec3(lights[i].getSpotDirection().x, lights[i].getSpotDirection().y, lights[i].getSpotDirection().z));
			glm::vec4 newpos = modelView.top()*l.getPosition();
			l.setPosition(newpos.x,newpos.y,newpos.z);
			lightvector.push_back(l);
			
		}
		
	}

	virtual void draw(stack<glm::mat4> &modelView)
    {
		GLuint a;
        if (instanceOf!=NULL)
		{
			 //get the color
           // glm::vec4 color = material.getAmbient();

            //set the color for all vertices to be drawn for this object
           // glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(material.getAmbient()));
			glUniform3fv(scenegraph->mat_ambientLocation, 1, glm::value_ptr(material.getAmbient()));
			glUniform3fv(scenegraph->mat_diffuseLocation, 1, glm::value_ptr(material.getDiffuse()));
			glUniform3fv(scenegraph->mat_specularLocation, 1, glm::value_ptr( material.getSpecular()));
			glUniform1f(scenegraph->mat_shininessLocation, material.getShininess());
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));
			glUniformMatrix4fv(scenegraph->normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(modelView.top() * instanceOf->getTransform()))));
			if (texture == NULL)
			{
				glBindTexture(GL_TEXTURE_2D, 1);
			}
			else
			{

				glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
			}
			
			//glUniformMatrix4fv(scenegraph->texturematrixLocation, 1, GL_FALSE, glm::value_ptr(instanceOf->getTextureTransform()));
			glUniform1i(scenegraph->textureLocation, 0); //bind GL_TEXTURE0 to sampler2D (whatever is bound to GL_TEXTURE0)


			instanceOf->draw(GL_TRIANGLES);        			
		}
    }

	virtual void drawBB(stack<glm::mat4>& modelView)
	{
		if (bbDraw)
		{
			glm::mat4 bbTransform;

			bbTransform = glm::translate(glm::mat4(1.0),0.5f*(minBounds+maxBounds)) * glm::scale(glm::mat4(1.0),maxBounds-minBounds);
			glm::vec4 color = glm::vec4(1,1,1,1);
			//set the color for all vertices to be drawn for this object
			glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(color));
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top() * bbTransform));
			scenegraph->getInstance("box")->draw(GL_TRIANGLES);        		
		}
	}
	
	virtual void updateBB()
	{
		minBounds = instanceOf->getMinimumWorldBounds().xyz();
		maxBounds = instanceOf->getMaximumWorldBounds().xyz();
	}

	glm::vec4 getColor()
	{
		return material.getAmbient();
	}

	/*
		*Set the material of each vertex in this object
		*/
	virtual void setMaterial(graphics::Material& mat)
	{
		material = mat;
	}

	/*
		* gets the material
		*/
	graphics::Material getMaterial()
	{
		return material;
	}

	virtual void setTexture(graphics::Texture *tex)
	{
		texture = tex;
	}
protected:
	graphics::Object *instanceOf;
	graphics::Material material;
	graphics::Texture *texture;
};
#endif

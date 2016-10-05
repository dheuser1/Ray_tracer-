#ifndef _TRANSFORMNODE_H_
#define _TRANSFORMNODE_H_


#include "Node.h"
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class TransformNode : public Node
{
public:

	TransformNode(Scenegraph *graph,string name="")
		:Node(graph,name)
	{
		transform = glm::mat4(1.0);
		animation_transform = glm::mat4(1.0);
		child = NULL;
	}

	~TransformNode(void)
	{
		if (child!=NULL)
			delete child;
	}

	virtual Node *clone()
	{
		Node *newchild;

		if (child!=NULL)
		{
			newchild = child->clone();
		}
		else
		{
			newchild = NULL;
		}

		TransformNode *newtransform = new TransformNode(scenegraph,name);
		newtransform->setTransform(transform);
		newtransform->setAnimationTransform(animation_transform);

		if (newchild!=NULL)
		{
			newtransform->setChild(newchild);
		}		
		return newtransform;
	}

	virtual Node *getNode(string name)
	{
		if (Node::getNode(name)!=NULL)
			return Node::getNode(name);

		if (child!=NULL)
		{
			return child->getNode(name);
		}

		return NULL;
	}

	void setChild(Node *child)
	{
		this->child = child;
		this->child->setParent(this);
	}
	virtual void getNodeAni(glm::mat4& modelView)
	{
		if (parent != NULL)
			parent->getNodeAni(modelView);
		
		modelView = modelView * animation_transform * transform;
		
		return;

	}
	virtual bool intersect(stack<glm::mat4>& modelView, HitRecord& hit, Ray r)
	{
		modelView.push(modelView.top());
		modelView.top() = modelView.top() * animation_transform * transform;

		if (child != NULL)
			if (child->intersect(modelView, hit, r))
			{
				modelView.pop();
				return true;
			}
			
		modelView.pop();
		return false;
		
			
				
		
	}
	virtual void getlights(vector<graphics::Light>& lightvector, stack<glm::mat4> &modelView)
	{
		modelView.push(modelView.top());
		modelView.top() = modelView.top() * animation_transform * transform;

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
			l.setPosition(newpos.x, newpos.y, newpos.z);
			lightvector.push_back(l);
		}

		if (child != NULL)
			child->getlights(lightvector, modelView);
		modelView.pop();
	}

	virtual void draw(stack<glm::mat4> &modelView)
    {
        modelView.push(modelView.top());
        modelView.top() = modelView.top() * animation_transform * transform;
        if (child!=NULL)
			child->draw(modelView);
        modelView.pop();
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
		modelView.push(modelView.top());
        modelView.top() = modelView.top() * animation_transform * transform;
        
		if (child!=NULL)
			child->drawBB(modelView);
        modelView.pop();
	}

	virtual void updateBB()
	{
		glm::vec4 points[8];
		glm::vec3 bounds[2];
		int i,j,k,l;
		float x,y,z;

		if (child==NULL)
			return;

		child->updateBB();

		bounds[0] = child->getMinBounds();
		bounds[1] = child->getMaxBounds();

		l=0;
		for (i=0;i<2;i++)
		{
			x = bounds[i][0];
			for (j=0;j<2;j++)
			{
				y = bounds[j][1];
				for (k=0;k<2;k++)
				{
					z = bounds[k][2];
					points[l] = glm::vec4(x,y,z,1);
					l++;
				}
			}
		}

		//transform
		for (i=0;i<8;i++)
		{
			points[i] = animation_transform * transform * points[i];
		}

		minBounds = maxBounds = glm::vec3(points[0].x,points[0].y,points[0].z);

		for (i=1;i<8;i++)
		{
			for (j=0;j<3;j++)
			{
				if (points[i][j]<minBounds[j])
					minBounds[j] = points[i][j];
				if (points[i][j]>maxBounds[j])
					maxBounds[j] = points[i][j];
			}
		}
	}

	void setTransform(glm::mat4 obj)
    {
        transform = obj;
    }

    void setAnimationTransform(glm::mat4 mat)
    {
        animation_transform = mat;
    }

    glm::mat4 getTransform()
    {
        return transform;
    }

    glm::mat4 getAnimationTransform()
    {
        return animation_transform;
    }

	void setScenegraph(Scenegraph *graph)
	{
		Node::setScenegraph(graph);
		if (child!=NULL)
		{
			child->setScenegraph(graph);
		}
	}

protected:
	glm::mat4 transform,animation_transform;
	Node *child;
};

#endif
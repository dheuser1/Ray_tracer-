#ifndef GROUPNODE_H
#define GROUPNODE_H

#include <GL/gl.h>
#include <vector>
using namespace std;
#include "Node.h"
#include <glm/gtc/matrix_transform.hpp>


class GroupNode: public Node
{
protected:
    vector<Node *> children;

public:
    GroupNode(Scenegraph *graph,string name="")
        :Node(graph,name)
    {

    }

    ~GroupNode()
    {
        for (unsigned int i=0;i<children.size();i++)
            delete children[i];
    }

	virtual Node *clone()
	{
		vector<Node *> newc;

		for (int i=0;i<children.size();i++)
		{
			newc.push_back(children[i]->clone());
		}

		GroupNode * newgroup = new GroupNode(scenegraph,name);

		for (int i=0;i<children.size();i++)
		{
			newgroup->addChild(newc[i]);
		}		
		return newgroup;

	}

	virtual Node* getNode(string name)
	{
		if (Node::getNode(name)!=NULL)
		{
			return Node::getNode(name);
		}

		int i=0;
		Node *answer = NULL;

		while ((i<children.size()) && (answer == NULL))
		{
			answer = children[i]->getNode(name);
			i++;
		}
		return answer;
	}
	virtual void getNodeAni(glm::mat4& modelView)
	{
		if (parent!=NULL)
			parent->getNodeAni(modelView);
		return;

	}
	virtual bool intersect(stack<glm::mat4>& modelView, HitRecord& hit, Ray r)
	{
		bool intersection = false;
		for (int i = 0; i<children.size(); i++)
		{
			if (children[i]->intersect(modelView, hit, r))
			{
				intersection = true;
			}
		}
		if (intersection)
		{
			return true;
		}
		else
			return false;
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
			l.setPosition(newpos.x, newpos.y, newpos.z);
			lightvector.push_back(l);
		}
		for (int i = 0; i<children.size(); i++)
		{
			children[i]->getlights(lightvector, modelView);
		}
	}
    virtual void draw(stack<glm::mat4> &modelView)
    {
        for (int i=0;i<children.size();i++)
        {
            children[i]->draw(modelView);
        }
    }

	void setScenegraph(Scenegraph * graph)
	{
		Node::setScenegraph(graph);
		for (int i=0;i<children.size();i++)
		{
			children[i]->setScenegraph(graph);
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
		for (int i=0;i<children.size();i++)
        {
            children[i]->drawBB(modelView);
        }
	}

	virtual void updateBB()
	{
		int i,j;

		if (children.size()==0)
			return;

		for (i=0;i<children.size();i++)
		{
			children[i]->updateBB();
		}

		minBounds = children[0]->getMinBounds();
		maxBounds = children[0]->getMaxBounds();

		for (i=1;i<children.size();i++)
		{
			for (j=0;j<3;j++)
			{
				if (children[i]->getMinBounds()[j]<minBounds[j])
					minBounds[j] = children[i]->getMinBounds()[j];

				if (children[i]->getMaxBounds()[j]>maxBounds[j])
					maxBounds[j] = children[i]->getMaxBounds()[j];
			}
		}
	}

    void addChild(Node *child)
    {
        children.push_back(child);
		child->setParent(this);
    }

    const vector<Node *> getChildren() const
    {
        return children;
    }
};

#endif // GROUP_H

#ifndef NODE_H
#define NODE_H

#define GLM_FORCE_RADIANS

#include <string>
#include <map>
#include <stack>
#include <iostream>
using namespace std;
#include "utils/Light.h"
#include <glm/glm.hpp>
#include <vector>
#include "HitRecord.h"
#include "Ray.h"

class Scenegraph;

class Node
{
protected:
    string name;
    Scenegraph *scenegraph;
	bool bbDraw;
	glm::vec3 minBounds,maxBounds;
	vector<graphics::Light> lights;
public:
    Node(Scenegraph *graph,string name="")
    {
		this->parent = NULL;
        scenegraph = graph;
        setName(name);
		bbDraw = false;
    }

    virtual ~Node()
    {
	}

	virtual Node *getNode(string name)
	{
		if (this->name == name)
			return this;

		return NULL;
	}


	virtual void getlights(vector<graphics::Light>& lightvector, stack<glm::mat4> &modelView)=0;
	virtual void getNodeAni(glm::mat4& modelView)=0;
	virtual bool intersect(stack<glm::mat4>& modelView, HitRecord& hit, Ray r) = 0;
    virtual void draw(stack<glm::mat4>& modelView)=0;
	virtual void drawBB(stack<glm::mat4>& modelView)=0;
	virtual void updateBB()=0;
	virtual Node *clone()=0;
	void setParent(Node *parent)
	{
		this->parent = parent;
	}

	virtual void setScenegraph(Scenegraph *graph)
	{
		this->scenegraph = graph;
	}

    void setName(const string& name)
    {
        this->name = name;
    }

	void setBBDraw(bool d)
	{
		bbDraw = d;
	}

	glm::vec3 getMinBounds()
	{
		return minBounds;
	}

	glm::vec3 getMaxBounds()
	{
		return maxBounds;
	}

	void addLight(const graphics::Light& l)
	{
		lights.push_back(l);
		cout << "Light added in node " << name << endl;
	}

protected:
	Node *parent;
};

#endif // NODE_H

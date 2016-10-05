#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#define GLM_FORCE_RADIANS

#include <map>
#include <stack>
#include <vector>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include "utils/Object.h"
#include "Node.h"
#include "utils/Texture.h"
#include "Ray.h"
#include "HitRecord.h"

class Scenegraph
{    
    friend class LeafNode;
	friend class GroupNode;
	friend class TransformNode;

	typedef struct
	{
		GLint positionLocation;
		GLint ambientLocation;
		GLint diffuseLocation;
		GLint specularLocation;
	} LightLocation;
public:
    Scenegraph();
    ~Scenegraph();
    void makeScenegraph(Node *root);
    void initShaderProgram(GLint shaderProgram);
    void draw(stack<glm::mat4>& modelView, GLint shaderProgram);
	glm::vec3 raycast(Ray r, stack<glm::mat4>& modelView, int bounce, int t_bounce, int caller);
	bool Scenegraph::raycast_bool(Ray r, stack<glm::mat4>& modelView);
	void Scenegraph::ray_trace(int w, int h, stack<glm::mat4>& modelView);
	glm::vec3 shade(Ray r, vector<graphics::Light> lightvector,HitRecord hit, stack<glm::mat4>& modelView, int bounce, int t_bounce, int caller);
	
	void addInstance(graphics::Object *in)
	{
		if (instances.count(in->getName())<1)
			instances[in->getName()] = in;
	}

	void addTexture(graphics::Texture *tex)
	{
		if (instances.count(tex->getName())<1)
			textures[tex->getName()] = tex;
		cout << "texture added" << endl;
	}

	void animate(float t);
	void animate_cone(int x, int y);
	void animate_flag(int x, int y);

	graphics::Object *getInstance(string name)
	{
		if (instances.count(name)<1)
			return NULL;

		return instances[name];
	}

	graphics::Texture *getTexture(string name)
	{
		if (textures.count(name)<1)
			return NULL;

		return textures[name];
	}
	Node* getcameranode()
	{
			return j_seat_0;
	}
	
private:
	bool nodes_found;
	bool cone_found;
	bool flag_found;
	Node *root;
	Node* ride;
	Node* seat_0;
	Node* seat_1;
	Node* seat_2;
	Node* seat_3;
	Node *wheel;
	Node* j_seat_0;
	Node* j_seat_1;
	Node* j_seat_2;
	Node* j_seat_3;
	Node* cone;
	Node* flag;
	map<string,graphics::Object *> instances;
	map<string,graphics::Texture *> textures;
	GLint objectColorLocation, modelviewLocation, normalMatrixLocation, numLightsLocation, texturematrixLocation;
	GLint mat_diffuseLocation, mat_ambientLocation, mat_specularLocation, mat_shininessLocation, textureLocation;
	GLuint textureID;
};

#endif // SCENEGRAPH_H

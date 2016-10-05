#ifndef HITRECORD_H
#define HITRECORD_H

#include <glm/glm.hpp>
#include "utils/Material.h"
class HitRecord
{
public:
	HitRecord()
	{
		t = FLT_MAX;
		ipoint = glm::vec4(0, 0, 0, 0);
		normal = glm::vec4(0, 0, 0, 0);
		texcolor=glm::vec4(1, 1, 1, 0);
		has_tex=false;
	}
	HitRecord(float newt, glm::vec4& newipoint, glm::vec4& newnormal, graphics::Material& newmaterial)
	{
		t = newt;
		ipoint = newipoint;
		normal = newnormal;
		material = newmaterial;

	}
	~HitRecord()
	{
	}
	float getT()
	{
		return t;
	}
	glm::vec4 getipoint()
	{
		return ipoint;
	}
	glm::vec4 getnormal()
	{
		return normal;
	}
	graphics::Material getmaterial()
	{
		return material;
	}
	void setT(float newt)
	{
		t = newt;
	}
	void setipoint(glm::vec4 newipoint)
	{
		ipoint = newipoint;
	}
	void setnormal(glm::vec4 newnormal)
	{
		normal = newnormal;
	}
	void setmaterial(graphics::Material newm)
	{
		material = newm;
	}
	void set_texcolor(glm::vec4 newcolor)
	{
		texcolor=newcolor;
	}
	glm::vec4 get_texcolor()
	{
		return texcolor;
	}
	void set_has_tex(bool b)
	{
		has_tex=b;
	}
	bool get_has_tex()
	{
		return has_tex;
	}
protected:
	float t;
	glm::vec4 ipoint;
	glm::vec4 normal;
	glm::vec4 texcolor;
	graphics::Material material;
public:
	bool has_tex;
};

#endif
#ifndef RAY_H
#define RAY_H
#include <glm/glm.hpp>


class Ray
{
public:
	Ray()
	{
		point = glm::vec4(0, 0, 0, 1);
		vector = glm::vec4(0, 0, 0, 0);
	}
	Ray(glm::vec4 newpoint, glm::vec4 newvector)
	{
		point = newpoint;
		vector = newvector;
	}
	void setPoint(glm::vec4 newpoint)
	{
		point = newpoint;
	}
	void setVector(glm::vec4 newvector)
	{
		vector = newvector;
	}

	glm::vec4 getPoint()
	{
		return point;
	}
	glm::vec4 getVector()
	{
		return vector;
	}
	void RayCast()
	{
		//finds intersection 
	}
	~Ray()
	{

	}
private:
	glm::vec4 point;
	glm::vec4 vector;
};

#endif
#ifndef _MYCAMERA_H_
#define _MYCAMERA_H_

#define GLM_FORCE_RADIANS //to force glm to use radians, support for degrees in some functions is deprecated
#include "glm/gtx/transform.hpp"

class MyCamera
{
public:

	MyCamera()
	{
		speed = 1.0f;
	}

	inline glm::mat4 getWorldToCameraTransform()
	{
		return worldToCamera ;
	}

	inline void init(glm::vec3 pos,glm::vec3 lookat,glm::vec3 up)
	{
		glm::vec3 tempX,tempY,tempZ;

		speed = 1;
		
		origin = glm::vec4(pos[0],pos[1],pos[2],1.0f);
		tempY = glm::vec3(up[0],up[1],up[2]);
		tempZ = glm::vec3(origin[0]-lookat[0],origin[1]-lookat[1],origin[2]-lookat[2]);
		tempZ = glm::normalize(tempZ);
		tempX = glm::cross(tempY,tempZ);
		tempY = glm::cross(tempZ,tempX);
		tempX = glm::normalize(tempX);
		tempY = glm::normalize(tempY);
		tempZ = glm::normalize(tempZ);

		X = glm::vec4(tempX,0.0f);
		Y = glm::vec4(tempY,0.0f);
		Z = glm::vec4(tempZ,0.0f);
		
		updateCameraTransform();		
	}

	inline void increaseSpeed()
	{
		speed++;
	}

	inline void decreaseSpeed()
	{
		speed--;
		if (speed<1)
			speed = 1;
	}

	~MyCamera(void)
	{
	}

	void advance()
	{
		origin = origin - Z * speed;
		updateCameraTransform();
	}

	void retract()
	{
		origin = origin + Z * speed;
		updateCameraTransform();
	}

	void rotateLeft()
	{
		glm::vec4 vec;

		glm::mat4 r = glm::rotate(glm::mat4(1.0),glm::radians(5*speed),glm::vec3(Y[0],Y[1],Y[2]));

		vec = glm::vec4(X[0],X[1],X[2],0.0f);
		vec = r * vec;
		X = vec;

		vec = glm::vec4(Z[0],Z[1],Z[2],0.0f);
		vec = r * vec;
		Z = vec;

		updateCameraTransform();
	}

	void rotateRight()
	{
		glm::vec4 vec;

		glm::mat4 r = glm::rotate(glm::mat4(1.0),glm::radians(-5*speed),glm::vec3(Y[0],Y[1],Y[2]));

		vec = glm::vec4(X[0],X[1],X[2],0.0f);
		vec = r * vec;
		X = vec;

		vec = glm::vec4(Z[0],Z[1],Z[2],0.0f);
		vec = r * vec;
		Z = vec;

		updateCameraTransform();
	}

	void rotateCW()
	{
		glm::vec4 vec;

		glm::mat4 r = glm::rotate(glm::mat4(1.0),glm::radians(-5*speed),glm::vec3(Z[0],Z[1],Z[2]));

		vec = glm::vec4(X[0],X[1],X[2],0.0f);
		vec = r * vec;
		X = vec;

		vec = glm::vec4(Y[0],Y[1],Y[2],0.0f);
		vec = r * vec;
		Y = vec;

		updateCameraTransform();
	}

	void rotateCCW()
	{		
		glm::vec4 vec;

		glm::mat4 r = glm::rotate(glm::mat4(1.0),glm::radians(5*speed),glm::vec3(Z[0],Z[1],Z[2]));

		vec = glm::vec4(X[0],X[1],X[2],0.0f);
		vec = r * vec;
		X = vec;

		vec = glm::vec4(Y[0],Y[1],Y[2],0.0f);
		vec = r * vec;
		Y = vec;

		updateCameraTransform();
	}

	void rotateUp()
	{
		glm::vec4 vec;

		glm::mat4 r = glm::rotate(glm::mat4(1.0),glm::radians(5*speed),glm::vec3(X[0],X[1],X[2]));

		vec = glm::vec4(Z[0],Z[1],Z[2],0.0f);
		vec = r * vec;
		Z = vec;

		vec = glm::vec4(Y[0],Y[1],Y[2],0.0f);
		vec = r * vec;
		Y = vec;

		updateCameraTransform();
	}

	void rotateDown()
	{
		glm::vec4 vec;

		glm::mat4 r = glm::rotate(glm::mat4(1.0),glm::radians(-5*speed),glm::vec3(X[0],X[1],X[2]));

		vec = glm::vec4(Z[0],Z[1],Z[2],0.0f);
		vec = r * vec;
		Z = vec;

		vec = glm::vec4(Y[0],Y[1],Y[2],0.0f);
		vec = r * vec;
		Y = vec;

		updateCameraTransform();
	}
	private:

		void updateCameraTransform()
		{
			cameraToWorld = glm::mat4(X,Y,Z,origin);
			worldToCamera = glm::inverse(cameraToWorld);
		}

private:
	glm::vec4 origin,X,Y,Z;
	glm::mat4 cameraToWorld,worldToCamera;
	float speed;
};

#endif
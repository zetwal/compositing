#ifndef RAY_CASTING_H
#define RAY_CASTING_H

#include <iostream>     
#include <algorithm>

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class rayCasting{
	glm::vec4 eye;

	float materialProperties[4];
	glm::vec3 pixelColor;

	float transferFn[256*4];

	glm::vec4 viewVector;
	glm::vec4 upVector;

	glm::mat4 worldToView;

	float *gridMesh;
	float *scalarData;

	float *renderedImage;

	int screenDims[2];
	int screenPos[2];

	glm::vec4 getRay(int x, int y);
	void intersect();

	glm::vec4 colorScalar(glm::vec3 gradient, glm::vec3 dir, float scalar);

	bool lighting;


public:
	rayCasting();

	void setEye(float _x, float _y, float _z){ eye.x=_x; eye.y=_y; eye.z=_z; eye.w=1;}
	void setViewVector(float _x, float _y, float _z){ viewVector.x=_x; viewVector.y=_y; viewVector.z=_z; viewVector.w=1;}
	void setUpVector(float _x, float _y, float _z){ upVector.x=_x; upVector.y=_y; upVector.z=_z; upVector.w=1;}

	void setMaterialProperties(float amb, float diff, float spec, float specPower){
		materialProperties[0] = amb; materialProperties[1] = diff; materialProperties[2] = spec; materialProperties[3] = specPower;}

	//void setWorldToViewMatrix(float mat[16]){ for (int i=0; i<16; i++) worldToView[i]=mat[i]; }
	//void setProjMatrix(float mat[16]){ for (int i=0; i<16; i++) projection[i]=mat[i]; }

	void setScreenDims(int x, int y){ screenDims[0] = x; screenDims[1] = y;}
	void setScreenPos(int x, int y){ screenPos[0] = x; screenPos[1] = y;}

	void castRay(int x, int y);




	~rayCasting();
};

#endif

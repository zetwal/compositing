#include "rayCasting.h"

rayCasting::rayCasting(){
	screenPos[0] = screenPos[1] = 0.0;

	lighting = false;
}

rayCasting::~rayCasting(){
}

glm::vec4 rayCasting::getRay(int x, int y){
	//
	// Transform from screen space to eye space
	glm::vec4 worldPos;

	// Normalized screen coordinates (-1 to 1)
	worldPos.x = ((float)x-screenPos[0])/screenDims[0] - 0.5;
	worldPos.y = ((float)y-screenPos[1])/screenDims[1] - 0.5;
	worldPos.z = 0;
	worldPos.w = 1.0;

	// From view to world
	worldPos = glm::inverse(worldToView) * worldPos;

	// Ray direction: pt = eye + t(worldPos - eye)
	return glm::normalize(worldPos - eye);
}



// A ray-(axis aligned)box intersection -  to find if we intersect the data
// 		Find the x,y,z coordinates of that intersection
//		Figure out where does that x,y,z map to in the gridMesh -  gives us the cell data 
//			how? we know the starting point and the size of each cell
// should return wherther or not there is an intersection
// + x,y,z into the cell of the intersection
// + cell index of the intersection
void intersect(){

}



glm::vec4 rayCasting::colorScalar(glm::vec3 gradient, glm::vec3 dir, float scalar){
	glm::vec4 srcColor;

	// query from tf function
	//srcColor = 

	if (lighting == true){
		float normal_dot_light = glm::dot(gradient,dir);
		normal_dot_light = std::max(0.0f, std::min((float)fabs(normal_dot_light),1.0f) );

		// Opacity correction
        float opacityCorrectiong = 0.7;
        srcColor.a = 1.0 - pow((1.0-srcColor.a),(double)opacityCorrectiong);

        //
        // Phong shading: I = (I  * ka) +   (I  * kd*abs(cos(angle))) + (Ia * ks*abs(cos(angle))^ns)
		srcColor = glm::vec4(glm::vec3(srcColor) * glm::vec3(materialProperties[0]) +  
				  			 glm::vec3(srcColor) * glm::vec3(materialProperties[0]) * glm::vec3(normal_dot_light) +
				  			 glm::vec3(materialProperties[2] * pow((double)normal_dot_light,materialProperties[3]) * srcColor.a),
				  			 srcColor.a);
	}

	return srcColor;
}


// x,y: pixel position on the screen
void rayCasting::castRay(int x, int y){
	glm::vec4 rayDir = getRay(x,y);
	
	//colorScalar
}
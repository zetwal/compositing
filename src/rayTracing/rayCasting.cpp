#include "rayCasting.h"

rayCasting::rayCasting(){
	screenPos[0] = screenPos[1] = 0.0;

	lighting = false;
}

rayCasting::~rayCasting(){
}

// x,y: pixel position on the screen
void rayCasting::castRay(int x, int y){
	glm::vec4 rayDir = getRay(x,y);
	
	//colorScalar
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



void
rayCasting::setTransferFn(glm::vec4 inputArray[tableEntries], int te, double attenuation, float over)
{
    minTFIndex = maxTFIndex = -1;

    if (attenuation < -1. || attenuation > 1.)
        std::cout << "Bad attenuation value !!!" << std::endl;
    

    if (te != tableEntries)
    	std::cout << "Wrong number of entries !!!" << std::endl;
    
  
    for (int i=0; i<te; i++){
        double bp = tan(1.570796327 * (0.5 - attenuation*0.49999));
        double alpha = pow(inputArray[i].r / 255.f, (float)bp);
        alpha = 1.0 - pow((1.0 - alpha), 1.0/over);

        transferFn1D[i].r = inputArray[i].r*alpha;
        transferFn1D[i].g = inputArray[i].g*alpha;
        transferFn1D[i].b = inputArray[i].b*alpha;
        transferFn1D[i].a = alpha;

        if (minTFIndex == -1)
            if (alpha > 0)
                minTFIndex = i;
            
        if (alpha > 0)
            maxTFIndex = i; 
    }


    if (minScalar != maxScalar)
        rangeScalar = maxScalar - minScalar;
    else
        rangeScalar = 1.0f;
}


int rayCasting::QueryTF(double scalarValue, glm::vec4 & color){
    if (scalarValue != scalarValue) // checking for NAN
        return 0;

    if (scalarValue <= minScalar){
        int index = 0;
        color = transferFn1D[index];

        return 0;
    }

    if (scalarValue >= maxScalar){
        int index = tableEntries-1;
        color = transferFn1D[index];

        return 0;
    }

    int indexLow, indexHigh;
    glm::vec4 colorLow, colorHigh;
    float indexPos, indexDiff;

    indexPos  = (scalarValue-minScalar)/(maxScalar-minScalar) * tableEntries;    // multiplier = 1.0/(max-min) * tableEntries
    indexLow  = (int)indexPos;
    indexHigh = (int)(indexPos+1.0);

    indexDiff = indexPos - indexLow;
    
    colorLow = transferFn1D[indexLow];
    colorHigh = transferFn1D[indexHigh];

    color = (1.0-indexDiff)*colorLow + indexDiff*colorHigh;

    return 1;
}


//   _ _ _ _ _ _ _
//  |      		 |
//  |			 |
//  |	a		 |
//  |- - - x     |
//  |      |     |
//  |      |b    |
//   - - - - - - -
//
//  a: dist from left
//  b: dist from bottom
//  c: distance from front (3D not shown)
float rayCasting::trilinearInterpolate(float vals[8], float distRight, float distTop, float distBack){
    float distLeft, distBottom, distFront;

    distRight = 1.0 - distLeft;
    distTop = 1.0 - distBottom;
    distBack = 1.0 - distFront;

    double val =    distRight	* distTop       * distBack * vals[0] + 
                    distLeft	* distTop       * distBack * vals[1] +
                    distRight   * distBottom    * distBack * vals[2] +
                    distLeft    * distBottom	* distBack * vals[3] +

                    distRight   * distTop       * distFront * vals[4] +
                    distLeft    * distTop       * distFront * vals[5] +
                    distRight   * distBottom    * distFront * vals[6] +
                    distLeft    * distBottom	* distFront * vals[7];
    return val;
}

void rayCasting::assignEight(float vals[8], int *index, float *scalarData){
    for (int i=0 ; i<8 ; i++)
        vals[i] = scalarData[index[i]];
}
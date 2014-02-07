#include "rayCasting.h"

rayCasting::rayCasting(){
}

rayCasting::~rayCasting(){
}


void transformWorldtoScreen(float inVec[4], float outVec[4]){
	outVec[0] =  worldToViewMat[0]*inVec[0] +  worldToViewMat[1]*inVec[1] +  worldToViewMat[2]*inVec[2] +  worldToViewMat[3]*inVec[3];
	outVec[1] =  worldToViewMat[4]*inVec[0] +  worldToViewMat[5]*inVec[1] +  worldToViewMat[6]*inVec[2] +  worldToViewMat[7]*inVec[3];
	outVec[2] =  worldToViewMat[8]*inVec[0] +  worldToViewMat[9]*inVec[1] + worldToViewMat[10]*inVec[2] + worldToViewMat[11]*inVec[3];
	outVec[3] = worldToViewMat[12]*inVec[0] + worldToViewMat[13]*inVec[1] + worldToViewMat[14]*inVec[2] + worldToViewMat[15]*inVec[3];

	outVec[0] = outVec[0]/outVec[3];
	outVec[1] = outVec[1]/outVec[3];
	outVec[2] = outVec[2]/outVec[3];
	outVec[3] = outVec[3]/outVec[3];
}


void rayCasting::rayCast(float *scalarData){

	// compute dimensions of the ending image
	float coordinates[8][4];
	coordinates[0][0]=meshMin[0];	coordinates[0][1]=meshMin[1];	coordinates[0][3]=meshMin[2];	coordinates[0][4]=1.0;
	coordinates[1][0]=meshMax[0];	coordinates[1][1]=meshMin[1];	coordinates[1][3]=meshMin[2];	coordinates[1][4]=1.0;
	coordinates[2][0]=meshMin[0];	coordinates[2][1]=meshMax[1];	coordinates[2][3]=meshMin[2];	coordinates[2][4]=1.0;
	coordinates[3][0]=meshMax[0];	coordinates[3][1]=meshMax[1];	coordinates[3][3]=meshMin[2];	coordinates[3][4]=1.0;

	coordinates[4][0]=meshMin[0];	coordinates[4][1]=meshMin[1];	coordinates[4][3]=meshMax[2];	coordinates[4][4]=1.0;
	coordinates[5][0]=meshMax[0];	coordinates[5][1]=meshMin[1];	coordinates[5][3]=meshMax[2];	coordinates[5][4]=1.0;
	coordinates[6][0]=meshMin[0];	coordinates[6][1]=meshMax[1];	coordinates[6][3]=meshMax[2];	coordinates[6][4]=1.0;
	coordinates[7][0]=meshMax[0];	coordinates[7][1]=meshMax[1];	coordinates[7][3]=meshMax[2];	coordinates[7][4]=1.0;

	float xMin, xMax, yMin, yMax;
	for (int i=0; i<8; i++){
		float transformedPoint[4];
		transformWorldtoScreen(coordinates[i],transformedPoint);
		if (i == 0){
			xMin = xMax = transformedPoint[0];
			yMin = yMax = transformedPoint[1];
			avgZ = transformedPoint[2];
		}else{
			if (xMin > transformedPoint[0])
				xMin = transformedPoint[0];

			if (xMax < transformedPoint[0])
				xMax = transformedPoint[0];

			if (yMin > transformedPoint[1])
				yMin = transformedPoint[1];

			if (yMax < transformedPoint[1])
				yMax = transformedPoint[1];

			avgZ += transformedPoint[2];
		}
	}
	avgZ = avgZ/8.0;


	for (int i=xMin; i<xMAx; i++)
		for (int  j=yMin; j<yMax; j++){
			
		}
}
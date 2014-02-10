// ************************************************************************* //
//                                 rayCasting.h                              //
// ************************************************************************* //

class rayCasting{

	// rendered image
	int imageSize[2];
	int startingPos[2];
	float *imgArray;
	float avgZ;

	// mesh dimensions
	double	meshMin[3];
    double	meshMax[3];			
    int		logicalBounds[3];	// dimenasions x,y,z
    float	*mesh;
    double  *scalarData;

    // kdtree
    double	currentPartitionExtents[6];  // minX, minY, minZ  -  maxX, maxY,maxZ

    // transformation matrix
	double worldToViewMat[16];
	double eyePos[4];

	// screen
	int screenSize[2];
	int sceenPosition[2];

public:
	void setMeshMin(float _meshMin[3]){ for(int i=0; i<3; i++) meshMin[i]=_meshMin[i]; }
	void setMeshMax(float _meshMax[3]){ for(int i=0; i<3; i++) meshMax[i]=_meshMax[i]; }
	void setLogicalBounds(float _logicalBounds[3]){ for(int i=0; i<3; i++) logicalBounds[i]=_logicalBounds[i]; }

	void setPartitionExtents(float _currentPartitionExtents[6]){ for(int i=0; i<6; i++) currentPartitionExtents[i]=_currentPartitionExtents[i]; }

	void setWorldToViewMat(float _worldToViewMat[16]){ for(int i=0; i<16; i++) worldToViewMat[i]=_worldToViewMat[i]; }
	void setEye(float _eye[3]){ for(int i=0; i<3; i++) eyePos[i]=_eye[i]; eyePos[3] = 1.0;}

	void setScreenSize(int _screenSize[2]){screenSize[0] = _screenSize[0]; screenSize[1]=_screenSize[1];}


	rayCasting();
	~rayCasting();

	void rayCast(float *scalarData);
};
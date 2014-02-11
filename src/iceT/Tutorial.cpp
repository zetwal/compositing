/* -*- c -*- *****************************************************************
** Copyright (C) 2007 Sandia Corporation
** Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
** license for use of this work by or on behalf of the U.S. Government.
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that this Notice and any statement
** of authorship are reproduced on all copies.
**
** This is a simple example of using the IceT library. It demonstrates the
** techniques described in the Tutorial chapter of the IceT User’s Guide.
*****************************************************************************/
#include <stdlib.h>

#include <IceT.h>
#include <IceTMPI.h>
#include <IceTDevImage.h>

#include <string>
#include <sstream>
#include <sys/stat.h>
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define TILE_WIDTH 1000
#define TILE_HEIGHT 1000

static void InitIceT();
static void DoFrame();
static void Draw(const IceTDouble * projection_matrix,
				const IceTDouble * modelview_matrix,
				const IceTFloat * background_color,
				const IceTInt * readback_viewport,
				IceTImage result);
static int winId;
static IceTContext icetContext;
IceTDouble projection_matrix[16];
IceTDouble modelview_matrix[16];
IceTFloat backgroundcolor[4];

IceTDrawCallbackType original_callback = &Draw;


float *buffer;

float clamp(float color){
	if(color > 255) color = 255;

	return color;
}

void createPpm(float array[], int dimx, int dimy, std::string filename){
    int i, j;
    std::cout << "createPpm2  dims: " << dimx << ", " << dimy << " -  " << filename.c_str() << std::endl;
    FILE *fp = fopen(filename.c_str(), "wb"); // b - binary mode 
    (void) fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
    for (j = 0; j < dimy; ++j){
        for (i = 0; i < dimx; ++i){
        	//std::cout << array[j*(dimx*4) + i*4 + 3] << "   ";
            static unsigned char color[3];
            float alpha = array[j*(dimx*4) + i*4 + 3];
            color[0] = clamp(array[j*(dimx*4) + i*4 + 0] * alpha * 255);  // red
            color[1] = clamp(array[j*(dimx*4) + i*4 + 1] * alpha * 255);  // green
            color[2] = clamp(array[j*(dimx*4) + i*4 + 2] * alpha * 255);  // blue 
            (void) fwrite(color, 1, 3, fp);
        }
    }
    (void) fclose(fp);
    std::cout << "End createPpm: " << std::endl;
}

float map(float value, float a1, float a2, float b1, float b2){
	return b1 + ((b2 - b1) / (a2 - a1)) * (value - a1);
}

std::string NumbToString(int Number)
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

std::string getCurrentPath(){

	char cCurrentPath[FILENAME_MAX];

        if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	    {
	      std::cout << "ERROR!" << std :: endl;
	      return "";
	    } else{

			cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
			printf ("The current working directory is %s", cCurrentPath);
			return cCurrentPath;
		}

}

int main(int argc, char **argv)
{
	int rank, numProc;
	IceTCommunicator icetComm;

	mkdir("debug", S_IRWXU|S_IRGRP|S_IXGRP);

	/* Setup MPI. */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	icetComm = icetCreateMPICommunicator(MPI_COMM_WORLD);
	icetContext = icetCreateContext(icetComm);
	icetDestroyMPICommunicator(icetComm);

	InitIceT();

	return 0;
}

static void InitMatrices()
{
	for(int i = 0; i < 16; i++){
		if(i%5 == 0) {
			modelview_matrix[i] = 1.0;
			projection_matrix[i] = 1.0;
		}else {
			modelview_matrix[i] = 0.0;
			projection_matrix[i] = 0.0;
		}
	}
	backgroundcolor[0] = 0.0f;
	backgroundcolor[1] = 0.0f;
	backgroundcolor[2] = 0.0f;
	backgroundcolor[3] = 0.0f;

}

static void InitData(int rank)
{
	for (int y = 0; y < TILE_HEIGHT; y++) {
	    for (int x = 0; x < TILE_WIDTH; x++) {

	    	int index = y * TILE_WIDTH + x * 4;

	        if( rank%3 == 0){
	        	buffer[index++] = 1.0;
	            buffer[index++] = 0.0;
	            buffer[index++] = 0;
	            buffer[index] = 1.0;
	        }

	        else if( rank%3 == 1){
	        	buffer[index++] = 0;
	            buffer[index++] = 1.0;
	            buffer[index++] = 0.0;
	            buffer[index] = 1.0;
	        }

	         else{
	        	buffer[index++] = 0.5;
	            buffer[index++] = 0;
	            buffer[index++] = 0.5;
	            buffer[index] = 1.0;
	        }
	    }
	}
}

static void InitIceT()
{
	
	IceTInt rank, num_proc;
	icetGetIntegerv(ICET_RANK, &rank);
	icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

	std::cout <<  rank << "		~ InitIceT()" << std::endl;

	// Inititialize 
	InitMatrices();

	float start, end;

	int div_width = TILE_WIDTH / num_proc;

	start = map(rank*div_width, 0, TILE_WIDTH, -1, 1);
	end = map((rank+1)*div_width + 2 * div_width / 3, 0, TILE_WIDTH, -1, 1);

	icetBoundingBoxf(start, end, start, end, -0.5, 0.5);

	// /////////////////////////
	icetCompositeMode( ICET_COMPOSITE_MODE_BLEND );
	icetSetColorFormat(ICET_IMAGE_COLOR_RGBA_FLOAT); // from 0- 1, or ICET_IMAGE_COLOR_RGBA_UBYTE from 0 to 255
	icetSetDepthFormat(ICET_IMAGE_DEPTH_NONE);

	icetEnable(ICET_ORDERED_COMPOSITE);


	IceTInt *procs = new IceTInt[num_proc];
	for(int i=0; i<num_proc; i++)
		procs[i] = num_proc - 1 - i;
	icetCompositeOrder(procs);

	// //Be aware that not all strategies support ordered compositing !!!!
	// //////////////////////////

	icetResetTiles();
	icetAddTile(0, 0, TILE_WIDTH, TILE_HEIGHT, 0);

	icetPhysicalRenderSize(TILE_WIDTH, TILE_HEIGHT);
	icetStrategy(ICET_STRATEGY_REDUCE);

	// Run the RayCaster
	// ExecuteRayCaster() blah blah

	buffer = new float[TILE_WIDTH * TILE_HEIGHT * 4]; // Populate buffer with data from the ray caster

	InitData(rank); // remove this after running ray caster


	// Now call the DoFrame() function
	DoFrame();
}

static void DoFrame()
{
	IceTInt rank, num_proc;
	/* We could get these directly from MPI, but it’s just as easy to get them
	* from IceT. */
	icetGetIntegerv(ICET_RANK, &rank);
	icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

	std::cout <<  rank << "		~ DoFrame()" << std::endl;

	icetDrawCallback(original_callback);

	IceTImage result =  icetDrawFrame( projection_matrix,
									   modelview_matrix,
									   backgroundcolor );

	if (icetImageGetColorFormat(result) == ICET_IMAGE_COLOR_RGBA_FLOAT) {
		int image_width, image_height, num_pixels;

		icetGetIntegerv(ICET_PHYSICAL_RENDER_WIDTH, &image_width);
		icetGetIntegerv(ICET_PHYSICAL_RENDER_HEIGHT, &image_height);

    	std::cout <<  rank << "		~Before writing final" <<std::endl;
        IceTFloat *data = new IceTFloat[image_width*image_height*4];

        // std::cout << "Before copying" <<std::endl;
        icetImageCopyColorf ( result, data, ICET_IMAGE_COLOR_RGBA_FLOAT);

        std::cout <<  rank << "		~Like writing right here" <<std::endl;

        createPpm(data, image_width, image_height, getCurrentPath() + "/debug/icet_" + NumbToString(rank) + ".ppm");
        std::cout <<  rank << "		~After writing final" <<std::endl;
    }

    icetDestroyContext(icetContext);
    std::cout <<  rank << "		~ after destroying" << std::endl;


	//glutDestroyWindow(winId);
	MPI_Finalize();
	exit(0);

}


static void Draw(	const IceTDouble * projection_matrix,
					const IceTDouble * modelview_matrix,
					const IceTFloat * background_color,
					const IceTInt * readback_viewport,
					IceTImage result){

	
	IceTInt rank, num_proc;
	icetGetIntegerv(ICET_RANK, &rank);
	icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

	std::cout <<  rank << "		~ Draw_two" << std::endl;
	int image_width, image_height, num_pixels;

	icetGetIntegerv(ICET_PHYSICAL_RENDER_WIDTH, &image_width);
	icetGetIntegerv(ICET_PHYSICAL_RENDER_HEIGHT, &image_height);

	icetClearImage(result);

	// write into the file
	int block_width = image_width / num_proc;

    if (icetImageGetColorFormat(result) == ICET_IMAGE_COLOR_RGBA_FLOAT) {

    	std::cout <<  rank << "		~ Before writing" <<std::endl;
        IceTFloat *data = icetImageGetColorf(result);
        for (int y = 0; y < image_height; y++) {
            for (int x = 0; x < image_width; x++) {

        		int index = y * image_width + x * 4;
            	data[0] = buffer[index++];
                data[1] = buffer[index++];
                data[2] = buffer[index++];
                data[3] = buffer[index];

                data += 4;
            }
        }

        data = icetImageGetColorf(result);
        // std::cout << "Before copying" <<std::endl;
        // icetImageCopyColorf ( result, data, ICET_IMAGE_COLOR_RGBA_FLOAT);

        createPpm(data, image_width, image_height, getCurrentPath() + "/debug/icet_sub_" + NumbToString(rank) + ".ppm");

        std::cout <<  rank << "		~ After writing" <<std::endl;
    } else {
        printf("ERROR: Encountered unknown color format.");
    }

	// The resulting image should be rendered (or copied) into the IceTImage, result, passed to
	// the callback. The image will be sized by the physical render width and height and its format will
	// conform to that set by icetSetColorFormat and icetSetDepthFormat. You can get the
	// buffers of the image with the icetImageGetColor and icetImageGetDepth functions.
	// Data written to these buffers will become part of the image.
	


}
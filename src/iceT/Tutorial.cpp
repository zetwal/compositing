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
/* IceT does not come with the facilities to create windows/OpenGL contexts.
* we will use glut for that. */
#include <IceT.h>
#include <IceTMPI.h>
#include <IceTDevState.h>
#include <IceTDevImage.h>
 #include <string>
 #include <sstream>

#define NUM_TILES_X 2
#define NUM_TILES_Y 2
#define TILE_WIDTH 300
#define TILE_HEIGHT 300
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

void createPpm(float array[], int dimx, int dimy, std::string filename){
    int i, j;
    std::cout << "createPpm2  dims: " << dimx << ", " << dimy << " -  " << filename.c_str() << std::endl;
    FILE *fp = fopen(filename.c_str(), "wb"); // b - binary mode 
    (void) fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
    for (j = 0; j < dimy; ++j){
        for (i = 0; i < dimx; ++i){
        	std::cout << array[j*(dimx*4) + i*4 + 3] << "   ";
            static unsigned char color[3];
            float alpha = array[j*(dimx*4) + i*4 + 3];
            color[0] = array[j*(dimx*4) + i*4 + 0] * alpha * 255;  // red
            color[1] = array[j*(dimx*4) + i*4 + 1] * alpha * 255;  // green
            color[2] = array[j*(dimx*4) + i*4 + 2] * alpha * 255;  // blue 
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

int main(int argc, char **argv)
{
	int rank, numProc;
	IceTCommunicator icetComm;
	/* Setup MPI. */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);


	std::cout << "before getting image_width" << std::endl;

	icetComm = icetCreateMPICommunicator(MPI_COMM_WORLD);
	icetContext = icetCreateContext(icetComm);
	icetDestroyMPICommunicator(icetComm);

	InitIceT();

	return 0;
}

static void InitIceT()
{
	
	IceTInt rank, num_proc;
	/* We could get these directly from MPI, but it’s just as easy to get them
	* from IceT. */
	icetGetIntegerv(ICET_RANK, &rank);
	icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

	std::cout <<  rank << "		~ InitIceT()" << std::endl;

	float x = rank;
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

	float start, end;
	
	if( rank == 0 ){
		start = map(50, 0, TILE_WIDTH, -1, 1);
		end = map(170, 0, TILE_WIDTH, -1, 1);
	}

	if( rank == 1 ){
		start = map(130, 0, TILE_WIDTH, -1, 1);
		end = map(250, 0, TILE_WIDTH, -1, 1);
	}

	icetBoundingBoxf(start, end, start, end, -0.5, 0.5);

	icetEnable(ICET_CORRECT_COLORED_BACKGROUND);

	/////////////////////////
	icetCompositeMode( ICET_COMPOSITE_MODE_BLEND );
	icetSetColorFormat(ICET_IMAGE_COLOR_RGBA_FLOAT); // from 0- 1, or ICET_IMAGE_COLOR_RGBA_UBYTE from 0 to 255
	icetSetDepthFormat(ICET_IMAGE_DEPTH_NONE);

	icetEnable(ICET_ORDERED_COMPOSITE);


	IceTInt *procs = new IceTInt(num_proc);
	for(int i=0; i<num_proc; i++)
		procs[i] = num_proc - 1 - i;
	icetCompositeOrder(procs);

	//Be aware that not all strategies support ordered compositing !!!!
	//////////////////////////

	icetResetTiles();
	icetAddTile(0, 0, TILE_WIDTH, TILE_HEIGHT, 0);

	icetPhysicalRenderSize(TILE_WIDTH, TILE_HEIGHT);

	/* Tell IceT what strategy to use. The REDUCE strategy is an all-around
	* good performer. */
	icetStrategy(ICET_STRATEGY_REDUCE);
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

        createPpm(data, image_width, image_height, "/home/pbmanasa/Desktop/debug/icet_" + NumbToString(rank) + ".ppm");
        std::cout <<  rank << "		~After writing final" <<std::endl;
    }

    std::cout <<  rank << "		~ destroying" << std::endl;

    icetDestroyContext(icetContext);
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
	/* We could get these directly from MPI, but it’s just as easy to get them
	* from IceT. */
	icetGetIntegerv(ICET_RANK, &rank);
	icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

	std::cout <<  rank << "		~ Draw_two" << std::endl;
	int image_width, image_height, num_pixels;

	icetGetIntegerv(ICET_PHYSICAL_RENDER_WIDTH, &image_width);
	icetGetIntegerv(ICET_PHYSICAL_RENDER_HEIGHT, &image_height);

	//IceTImage image = icetImageNull();

	// Initialize Image === page 82
	// IceTImage image = icetGetStateBufferImage(ICET_STRATEGY_BUFFER_0, image_width, image_height);

	// icetImageSetDimensions(image, image_width, image_height);
	// icetClearImage(image);

	/* Alternatively, you could get the width and height from the image passed */
	/* to the callback like this. */
	/* image_width = icetImageGetWidth(result); */
	/* image_height = icetImageGetHeight(result); */
	//num_pixels = icetImageGetNumPixels (image);
	//IceTUInt* pixel = icetImageGetColorui(image);

	// for (int i = 0; i < num_pixels; ++i) // *3 ???
	// {
	// 	pixel[i] = 50;
	// }

	int block_width = image_width / num_proc;

    if (icetImageGetColorFormat(result) == ICET_IMAGE_COLOR_RGBA_FLOAT) {

    	std::cout <<  rank << "		~ Before writing" <<std::endl;
        IceTFloat *data = icetImageGetColorf(result);
        for (int y = 0; y < image_height; y++) {
            for (int x = 0; x < image_width; x++) {
                // if ((image_height-y) < x) {
                //     data[0] = (float)x;
                //     data[1] = (float)y;
                //     data[2] = 0.0;
                //     data[3] = 1.0;
                // } 
             	//    if(x >= block_width * rank && x < block_width * (rank + 1) ){
	            //     data[0] = (float)x;
	            //     data[1] = (float)x;
	            //     data[2] = 0.0;
	            //     data[3] = 0.5;
            	// }

            	data[0] = data[1] = data[2] = data[3] = 0.0;

	            if( rank == 0 /*&& x >= 50 && x < 170 && y >= 50 && y < 170*/){
	            	data[0] = 1.0;
	                data[1] = 0;
	                data[2] = 0;
	                data[3] = 0.5;
	            }

	            if( rank == 1 /*&& x > 130 && x <= 250 && y > 130 && y < 250*/){
	            	data[0] = 0;
	                data[1] = 1.0;
	                data[2] = 0;
	                data[3] = 0.5;
	            }

                data += 4;
            }
        }

        data = icetImageGetColorf(result);
        // std::cout << "Before copying" <<std::endl;
        // icetImageCopyColorf ( result, data, ICET_IMAGE_COLOR_RGBA_FLOAT);

        createPpm(data, image_width, image_height, "/home/pbmanasa/Desktop/debug/icet_sub_" + NumbToString(rank) + ".ppm");

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
////////////////////////////////////////////////////////////////////////////
// Silicon Software ioLib example
//
//
//
// File:	ImageLoad.c
//
// Copyrights by Silicon Software 2009 - 2010
////////////////////////////////////////////////////////////////////////////

/*
*   This file contains an example for loading and saving images using the
*   Silicon Software io library. It generates a gray image, saves it
*   in TIFF file format, loads it, and shows it using the display lib.
*   In the next step, the same is done with a 24 bit RGB color image.
*/

#include <sisoIo.h>

#include <stdio.h>
#include <stdlib.h>

#include <SisoDisplay.h>

#ifdef _WIN32
#include <io.h>
#else /* _WIN32 */
#include <unistd.h>
#endif /* _WIN32 */

char * imageFileName			= "test.tif";
char * imageFileNameCol			= "testColor.tif";
const unsigned int width	    = 512;
const unsigned int height       = 512;


int main(int argc, char* argv[])

{
	SisoIoImageEngine * imageHandle		= NULL;
	unsigned char * imageData		    = NULL;
	const unsigned char * imageDataFromFile   = NULL;
	
	unsigned int  x			= 0;
	unsigned int  y			= 0;
	unsigned char grayValue = 1;
	unsigned char rValue	= 1;
	unsigned char gValue    = 1;
	unsigned char bValue    = 1;
	int ret					= 0;
	int displayId			= 0;
	int displayIdCol		= 0;
	int components			= 3;
	char ch					= 0;

	////////////////////////////////////
	// Create a testimage in 8 bit gray
	///////////////////////////////////

	// First alloc some memory
	printf("Create a grayscale image\n");
	imageData = (unsigned char * )malloc(width * height * sizeof(char));
	if (imageData == NULL) {
		printf("Could not allocate memory for image data\n");
		return -1;
	}

	// Now fill it....
	for (y = 0; y < height; y++) 
	{
		grayValue = y % 256;
		for (x = 0; x < width; x++) 
		{
			unsigned int bytePos = (y * height) + x;
			imageData[bytePos] = grayValue++;
		}
	}

	// ... and write it to a Tiff image.
	printf("Write Tiff\n");
	ret = IoWriteTiff(imageFileName, imageData, width, height, 8, 1);
	if (ret != 0) 
	{
		printf("Writing Tiff image failed with ret code: %i\n", ret);
	}
	printf("Writing Tiff ok\n");

	free(imageData);
	imageData = NULL;

	// Now open the Tiff again
	printf("Load Tiff image\n");
	ret  = IoImageOpen(imageFileName, &imageHandle);
	if(ret < 0)
	{
		printf("Loading Tiff failed!\n");
		return -1;
	}
	else
	{
		printf("Loading Tiff ok\n");
	}


	// print some information
	printf("Tiff has a width of %d pixel\n",      IoGetWidth(imageHandle));
	printf("Tiff has a height of %d pixel\n",     IoGetHeight(imageHandle));
	printf("Tiff has a %d components\n",          IoGetNrOfComponents(imageHandle));
	printf("Tiff has a %d bits per components\n", IoGetBitsPerComponent(imageHandle));
	printf("Tiff has a %d bits per pixel\n",      IoGetBitsPerPixel(imageHandle));

	// Get the image data
	imageDataFromFile = IoImageGetData(imageHandle);


	//Create a display to show the loaded image
	 displayId = CreateDisplay(8, width, height);
	 SetBufferWidth(displayId, width, height);

	// Show the Tiff image
	DrawBuffer(displayId, imageDataFromFile, 1, "");

	// free the opened image
	IoFreeImage(imageHandle);
	imageHandle       = NULL;
	imageDataFromFile = NULL;





	/////////////////////////////////////////////
	// Now we do the same with a BGR color image
	////////////////////////////////////////////

	// First alloc some memory
	printf("Create a BGR color image\n");
	imageData = (unsigned char * )malloc(width * height * sizeof(char) * components);
	if (imageData == NULL) 
	{
		printf("Could not allocate memory for image data\n");
		return -1;
	}

	// Now fill it....
	for (y = 0; y < height; y++)
	{
		rValue = y % 256;
		for (x = 0; x < width; x++)
		{
			unsigned int bytePos = ((y * height) + x) * 3;
			imageData[bytePos] = rValue++;
			imageData[bytePos+1] = gValue;
			imageData[bytePos+2] = bValue;
			if (rValue == 255) 
			{
				gValue++;
			}
			if (gValue == 255)
			{
				bValue++;
			}
		}
	}

	// ... and write it to a Tiff image.
	printf("Write Tiff\n");
	ret = IoWriteTiff(imageFileNameCol, imageData, width, height, 8, components);
	if (ret != 0) 
	{
		printf("Writing Tiff image failed with ret code: %i\n", ret);
	}
	printf("Writing Tiff ok\n");

	free(imageData);
	imageData = NULL;

	// Now open the Tiff again
	printf("Load Tiff image\n");
	ret = IoImageOpen(imageFileNameCol, &imageHandle);

	if(ret < 0)
	{
		printf("Loading Tiff failed!\n");
		return -1;
	}
	else
	{
		printf("Loading Tiff ok\n");
	}

	// print some information
	printf("Tiff has a width of %d pixel\n",       IoGetWidth(imageHandle));
	printf("Tiff has a height of %d pixel\n",      IoGetHeight(imageHandle));
	printf("Tiff has a %d components\n",           IoGetNrOfComponents(imageHandle));
	printf("Tiff has a %d bits per components\n",  IoGetBitsPerComponent(imageHandle));
	printf("Tiff has a %d bits per pixel\n",       IoGetBitsPerPixel(imageHandle));

	// Get the image data
	imageDataFromFile = IoImageGetData(imageHandle);

	// Create a display to show the loaded image
	displayIdCol = CreateDisplay(IoGetBitsPerComponent(imageHandle) * IoGetNrOfComponents(imageHandle),
					IoGetWidth(imageHandle) , IoGetHeight(imageHandle));

	SetBufferWidth(displayIdCol, IoGetWidth(imageHandle), IoGetHeight(imageHandle));

	// Show the Tiff image
	DrawBuffer(displayIdCol, imageDataFromFile, 1, "");

	// free the opened image
	IoFreeImage(imageHandle);
	imageHandle = NULL;
	imageDataFromFile = NULL;

	printf("Press any key\n");

	read(0, &ch, 1);

	// done.
	CloseDisplay(displayId);
	// done.
	CloseDisplay(displayIdCol);


	return 0;
}

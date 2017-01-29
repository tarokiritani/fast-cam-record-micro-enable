////////////////////////////////////////////////////////////////////////////
// ME4 Frame grabber example
//
//
//
// File:	KneeLutGrayFile.cpp
//
// Copyrights by Silicon Software 2002-2010
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "board_and_dll_chooser.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#define FILENAME	"anOriginalKneeLutFilenameGray.lut"

int ErrorMessage(Fg_Struct *fg)
{
	int			error	= Fg_getLastErrorNumber(fg);
	const char*	err_str = Fg_getLastErrorDescription(fg);
	fprintf(stderr,"Error: %d : %s\n",error,err_str);
	return error;
}

int ErrorMessageWait(Fg_Struct *fg)
{
	int			error	= ErrorMessage(fg);
	printf (" ... press ENTER to continue\n");
	getchar();
	return error;
}

const char *dll_list_me3[] = {
		"DualAreaGray",
		"DualLineGray",
		"MediumAreaGray",
		"MediumLineGray",
		""
};

const char *dll_list_me3xxl[] = {
		"DualAreaGray12XXL",
		"DualLineGray12XXL",
		"MediumAreaGray12XXL",
		"MediumLineGray12XXL",
		""
};


const char *dll_list_me4_dual[] = {
		"DualAreaGray16",
		"DualLineGray16",
		"MediumAreaGray16",
		"MediumLineGray16",
		""
};

const char *dll_list_me4_single[] = {
		"SingleAreaGray16",
		"SingleLineGray16",
		""
};


const char *dll_list_me4_VD_AD4_dual[] = {
	"DualAreaGray16",
	"DualLineGray16",
	"MediumAreaGray16",
	"MediumLineGray16",
	""
};



// returns the bit with according to the selected image format
int getNoOfBitsFromImageFormat(const int format)
{
	int Bits = 8;
	switch(format){
case FG_GRAY:
	Bits = 8;
	break;
case FG_GRAY16:
	Bits = 16;
	break;
case FG_COL24:
case FG_COL48:
	Bits = 24;
	break;
default:
	Bits = 8;
	break;
	};
	return Bits;
}


///////////////////////////////////////////////////////////////////////
// Main function
//

int main(int argc, char* argv[], char* envp[]){

	int nr_of_buffer	=	8;			// Number of memory buffer
	int nBoard			=	selectBoardDialog();			// Board Number
	int nCamPort		=	PORT_A;		// Port (PORT_A / PORT_B)
	int MaxPics			=	100;		// Number of images to grab
	int status			=   0;
	int i;
	Fg_Struct *fg;

	const char *dllName = NULL;

	int boardType = 0;

	boardType = Fg_getBoardType(nBoard);

	dllName = selectDll(boardType, dll_list_me3, dll_list_me3xxl, dll_list_me4_dual, dll_list_me4_single, dll_list_me4_VD_AD4_dual, NULL);

	// Initialization of the microEnable frame grabber
	if((fg = Fg_Init(dllName,nBoard)) == NULL) {
		status = ErrorMessageWait(fg);
		return status;
	}
	fprintf(stdout,"Init Grabber ok\n");

	// Setting the image size
	int width	= 512;
	int height	= 512;
	if (Fg_setParameter(fg,FG_WIDTH,&width,nCamPort) < 0) {
		status = ErrorMessageWait(fg);
		return status;
	}
	if (Fg_setParameter(fg,FG_HEIGHT,&height,nCamPort) < 0) {
		status = ErrorMessageWait(fg);
		return status;
	}

	int bitAlignment = FG_LEFT_ALIGNED;
	if (Fg_setParameter(fg,FG_BITALIGNMENT,&bitAlignment,nCamPort) < 0) {
		status = ErrorMessageWait(fg);
		return status;
	}

	fprintf(stdout,"Set Image Size on port %d (w: %d,h: %d) ok\n",nCamPort,width,height);


	// Initalizing and setting the gray value lookup table
	KneeLookupTable Lut,getLut;

	Lut.id = 0;
	Lut.nrOfElements = 1025;
	Lut.value = new double[1025];
	Lut.format = FG_GRAY;


	for(i=0; i<(int)Lut.nrOfElements;i++)
	{
		double x = 2.2*(double)i/1024.0;
		if(x<0.0) x=0.0;
		if(x>1.0) x=1.0;

		Lut.value[i] = x;
	}


	if(Fg_setParameter(fg, FG_KNEE_LUT,&Lut,nCamPort)<0){
		status = ErrorMessageWait(fg);
		return status;
	}


	// Reading the gray lookup table
	getLut.value = new double[1025];
	getLut.nrOfElements = 1025;
	getLut.id = 0;

	if(Fg_getParameter(fg, FG_KNEE_LUT,&getLut,nCamPort)<0){
		status = ErrorMessageWait(fg);
		return status;
	}
	fprintf(stdout,"Read Knee lookup table\n");
	fprintf(stdout,"Number of Elements %d \n",getLut.nrOfElements);

	for (i=0;i<(int)getLut.nrOfElements;i++) {
		fprintf(stdout,"<%d,%f>   ",i,getLut.value[i]);
		if(i%5==0){
			fprintf(stdout,"\n");
		}
	}
	free(Lut.value);
	free(getLut.value);

	// Save Knee lut to file
	if (Fg_setParameter(fg, FG_KNEE_LUT_FILE, (void*)FILENAME, nCamPort) < 0) {
		status = ErrorMessageWait(fg);
		return status;
	}
	int mode = FG_SAVE_LUT_TO_FILE;
	if(Fg_setParameter(fg, FG_KNEE_LUT_SAVE_LOAD_FILE,&mode,nCamPort)<0){
		status = ErrorMessageWait(fg);
		return status;
	}

	fprintf(stdout,"Lut saved in %s\n",FILENAME);

	// Load Knee lut from file
	if (Fg_setParameter(fg, FG_KNEE_LUT_FILE, (void*)FILENAME, nCamPort) < 0) {
		status = ErrorMessageWait(fg);
		return status;
	}
	mode = FG_LOAD_LUT_FROM_FILE;
	if(Fg_setParameter(fg, FG_KNEE_LUT_SAVE_LOAD_FILE,&mode,nCamPort)<0){
		status = ErrorMessageWait(fg);
		return status;
	}

	fprintf(stdout,"Lut Load from %s\n",FILENAME);

	// Memory allocation
	int format;
	Fg_getParameter(fg,FG_FORMAT,&format,nCamPort);
	size_t bytesPerPixel = 1;
	switch(format){
	case FG_GRAY:	bytesPerPixel = 1; break;
	case FG_GRAY16:	bytesPerPixel = 2; break;
	case FG_COL24:	bytesPerPixel = 3; break;
	case FG_COL32:	bytesPerPixel = 4; break;
	case FG_COL30:	bytesPerPixel = 5; break;
	case FG_COL48:	bytesPerPixel = 6; break;
	}
	size_t totalBufSize = width*height*nr_of_buffer*bytesPerPixel;
	dma_mem *pMem0;
	if((pMem0 = Fg_AllocMemEx(fg,totalBufSize,nr_of_buffer)) == NULL){
		status = ErrorMessageWait(fg);
		return status;
	} else {
		fprintf(stdout,"%d framebuffer allocated for port %d ok\n",nr_of_buffer,nCamPort);
	}

	// Creating a display window for image output
	int Bits = getNoOfBitsFromImageFormat(format);

	int nId = ::CreateDisplay(Bits,width,height);
	SetBufferWidth(nId,width,height);


	// Setting the trigger and grabber mode
	int		nTriggerMode		= FREE_RUN;
	if(Fg_setParameter(fg,FG_TRIGGERMODE,&nTriggerMode,nCamPort)<0)		{ status = ErrorMessageWait(fg); return status;}

	if((Fg_AcquireEx(fg,nCamPort,GRAB_INFINITE,ACQ_STANDARD,pMem0)) < 0){
		status = ErrorMessageWait(fg);
		return status;
	}


	// ====================================================
	// MAIN LOOP
	frameindex_t lastPicNr = 0;
	while((lastPicNr = Fg_getLastPicNumberBlockingEx(fg,lastPicNr+1,nCamPort,10,pMem0))< MaxPics) {
		if(lastPicNr <0){
			status = ErrorMessageWait(fg);
			Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
			Fg_FreeMemEx(fg,pMem0);
			Fg_FreeGrabber(fg);
			CloseDisplay(nId);
			return status;
		}
		::DrawBuffer(nId,Fg_getImagePtrEx(fg,lastPicNr,0,pMem0),(int)lastPicNr,"");

		int nImageTag = (int)lastPicNr;
		Fg_getParameter(fg,FG_IMAGE_TAG,&nImageTag,nCamPort);
		fprintf(stdout,"Image Tag: 0x%x\n",nImageTag);

	}

	// ====================================================
	// Freeing the grabber resource
	Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
	Fg_FreeMemEx(fg,pMem0);
	Fg_FreeGrabber(fg);

	CloseDisplay(nId);

	return FG_OK;
}

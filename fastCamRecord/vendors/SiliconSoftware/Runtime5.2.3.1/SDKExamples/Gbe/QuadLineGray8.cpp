////////////////////////////////////////////////////////////////////////////
// ME4 GbE Frame grabber example
// 
//
//
// File:	QuadLineGray8.cpp
// 
// Copyrights by Silicon Software 2009-2010
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>

#include "board_and_dll_chooser.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>
#include <gbe.h>

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


////////////////////////////////////////////////////////////////////////
// Main function
//

int main(int argc, char* argv[], char* envp[])
{
	int nr_of_buffer	=	8;			// Number of memory buffer
	int nBoard			=	selectBoardDialog();			// Board Number
	int nCamPort		=	PORT_A;		// Port (PORT_A / PORT_B / PORT_C / PORT_D)
	int MaxPics			=	10000;		// Number of images to grab
	int status = 0;
	Fg_Struct *fg;
	
	const char *dllName = "QuadLineGray8";
	
	// Initialization of the microEnable frame grabber
	if((fg = Fg_Init(dllName,nBoard)) == NULL) {
		status = ErrorMessageWait(fg);
		return status;
	}
	fprintf(stdout,"Init Grabber ok\n");

	// Setting the image size
	int width	= 512;
	int height	= 256;
	if (Fg_setParameter(fg,FG_WIDTH,&width,nCamPort) < 0) {
		status = ErrorMessageWait(fg);
		return status;
	}
	if (Fg_setParameter(fg,FG_HEIGHT,&height,nCamPort) < 0) {
		status = ErrorMessageWait(fg);
		return status;
	}

	fprintf(stdout,"Set Image Size on port %d (w: %d,h: %d) ok\n",nCamPort,width,height);

		
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

	// ====================================================
	// Gbe Init
	BoardHandle *board_handle_ptr = NULL;
	if((status = Gbe_initBoard(nBoard,0,&board_handle_ptr)) < 0){
		fprintf(stderr, "Error: %s", Gbe_getErrorDescription(status));
		return status;
	}

	if((status = Gbe_scanNetwork(board_handle_ptr,-1,200)) < 0){
		fprintf(stderr, "No camera found at port 0\n");
		fprintf(stderr, "Error: %s (%d)", Gbe_getErrorDescription(status), status);
		return status;
	}

	int NrOfCams = Gbe_getCameraCount(board_handle_ptr,nCamPort);
	fprintf(stdout,"%x cameras found\n",NrOfCams);

	CameraHandle *ch= NULL;
	if((status = Gbe_getFirstCamera(board_handle_ptr,nCamPort,&ch)) < 0){
		fprintf(stdout,"Gbe_getFirstCamera error\n");
		fprintf(stderr, "Error: %s (%d)", Gbe_getErrorDescription(status), status);
		return status;
	}

	if((status = Gbe_connectCamera(ch)) < 0){
		fprintf(stdout,"connect error\n");
		fprintf(stderr, "Error: %s (%d)", Gbe_getErrorDescription(status), status);
		return status;
	}

	if((status = Gbe_setIntegerValue(ch, "Width", width)) < 0){
		fprintf(stdout,"cannot set width of camera\n");		
		fprintf(stderr, "Error: %s (%d)", Gbe_getErrorDescription(status), status);
		return status;
	}

	if((status = Gbe_setIntegerValue(ch, "Height", height)) < 0) {
		fprintf(stdout,"cannot set height of camera\n");		
		fprintf(stderr, "Error: %s (%d)", Gbe_getErrorDescription(status), status);
		return status;
	}

	if((status = Gbe_startAcquisition(ch)) < 0) {
		fprintf(stdout,"error starting acquisition\n");		
		fprintf(stderr, "Error: %s (%d)", Gbe_getErrorDescription(status), status);
		return status;
	}

	// ====================================================
	// MAIN LOOP
	if((Fg_AcquireEx(fg,nCamPort,GRAB_INFINITE,ACQ_STANDARD,pMem0)) < 0){
		status = ErrorMessageWait(fg);
		return status;
	}

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
		::DrawBuffer(nId,Fg_getImagePtrEx(fg,lastPicNr,nCamPort,pMem0),(int)lastPicNr,"");	
	}
	
	// ====================================================
	// Freeing the Gbe resource
	Gbe_stopAcquisition(ch);
	Gbe_disconnectCamera(ch);
	Gbe_freeCamera(ch);
	Gbe_freeBoard(board_handle_ptr);
	
	// ====================================================
	// Freeing the grabber resource
	Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
	Fg_FreeMemEx(fg,pMem0);
	Fg_FreeGrabber(fg);

	CloseDisplay(nId);
	
	return FG_OK;
}

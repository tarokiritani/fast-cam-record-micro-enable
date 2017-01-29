////////////////////////////////////////////////////////////////////////////
// ME4 Frame grabber example
//
//
//
// File:	LineTriggerGatedMultiBufferImg.cpp
//
// Copyrights by Silicon Software 2002-2010
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>

#include "board_and_dll_chooser.h"
#include "os_funcs.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

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
		"DualLineGray",
		"DualLineRGB",
		"MediumLineGray",
		"MediumLineRGB",
		""
};

const char *dll_list_me3xxl[] = {
		"DualLineGray12XXL",
		"DualLineRGB36XXL",
		"MediumLineGray12XXL",
		"MediumLineRGBXXL",
		""
};


const char *dll_list_me4_dual[] = {
		"DualLineGray16",
		"DualLineRGB30",
		"MediumLineGray16",
		"MediumLineRGB36",
		""
};

const char *dll_list_me4_single[] = {
		"SingleLineGray16",
		"SingleLineRGB48",
		""
};

const char *dll_list_me4_VD_AD4_dual[] = {
	"DualLineGray16",
	"DualLineRGB30",
	"MediumLineGray16",
	"MediumLineRGB36",
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
	int status = 0;
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

	// Setting the image trigger
	int		nImgTriggerMode				= ASYNC_GATED_MULTIFRAME;
	int     nImgTrgOn					= FG_NO;
	int		nImagetriggerDelayInLines	= 1;
	int		nStrobepulseDelayInLines	= 1;
    int     nStrobpulseInvert			= FG_NO;
	int     nImageTriggerInSrc			= -1; // (-1) -> 3 (-1 = Software)
	int     nImageTriggerInInvert		= FG_NO;

	if(Fg_setParameter(fg,FG_IMGTRIGGERON,&nImgTrgOn,nCamPort)<0)						{ status = ErrorMessageWait(fg); return status;}
	if(Fg_setParameter(fg,FG_IMGTRIGGERMODE,&nImgTriggerMode,nCamPort)<0)				{ status = ErrorMessageWait(fg); return status;}
	if(Fg_setParameter(fg,FG_IMGTRIGGERGATEDELAY,&nImagetriggerDelayInLines,nCamPort)<0){ status = ErrorMessageWait(fg); return status;}
	if(Fg_setParameter(fg,FG_STROBEPULSEDELAY,&nStrobepulseDelayInLines,nCamPort)<0)	{ status = ErrorMessageWait(fg); return status;}
	if(Fg_setParameter(fg,FG_STROBEPULSEINVERT,&nStrobpulseInvert,nCamPort)<0)			{ status = ErrorMessageWait(fg); return status;}
	if(Fg_setParameter(fg,FG_IMGTRIGGERINSRC,&nImageTriggerInSrc,nCamPort)<0)			{ status = ErrorMessageWait(fg); return status;}
	if(Fg_setParameter(fg,FG_IMGTRIGGERINPOLARITY,&nImageTriggerInInvert,nCamPort)<0)	{ status = ErrorMessageWait(fg); return status;}
	nImgTrgOn					= 1;
	if(Fg_setParameter(fg,FG_IMGTRIGGERON,&nImgTrgOn,nCamPort)<0)			{ status = ErrorMessageWait(fg); return status;}


	// ====================================================
	// MAIN LOOP

	if((Fg_AcquireEx(fg,nCamPort,GRAB_INFINITE,ACQ_STANDARD,pMem0)) < 0){
		status = ErrorMessageWait(fg);
		return status;
	}

	int softwareTriggerLevel = 0;

	frameindex_t lastPicNr = 0;
	while(lastPicNr< MaxPics) {
		softwareTriggerLevel = 1;
		if (Fg_setParameter(fg,FG_SETSOFTWARETRIGGER,&softwareTriggerLevel,nCamPort)<0)	{
			status = ErrorMessageWait(fg);
			Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
			Fg_FreeMemEx(fg,pMem0);
			Fg_FreeGrabber(fg);
			CloseDisplay(nId);
			return status;
		}

		Sleep(50);

		softwareTriggerLevel = 0;
		if (Fg_setParameter(fg,FG_SETSOFTWARETRIGGER,&softwareTriggerLevel,nCamPort)<0)	{
			status = ErrorMessageWait(fg);
			Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
			Fg_FreeMemEx(fg,pMem0);
			Fg_FreeGrabber(fg);
			CloseDisplay(nId);
			return status;
		}

		lastPicNr = Fg_getLastPicNumberBlockingEx(fg,lastPicNr+1,nCamPort,10,pMem0);
		if(lastPicNr <0){
			status = ErrorMessageWait(fg);
			Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
			Fg_FreeMemEx(fg,pMem0);
			Fg_FreeGrabber(fg);
			CloseDisplay(nId);
			return status;
		}
		::DrawBuffer(nId,Fg_getImagePtrEx(fg,lastPicNr,0,pMem0),(int)lastPicNr,"");
	}

	// ====================================================
	// Freeing the grabber resource
	Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
	Fg_FreeMemEx(fg,pMem0);
	Fg_FreeGrabber(fg);

	CloseDisplay(nId);

	return FG_OK;
}

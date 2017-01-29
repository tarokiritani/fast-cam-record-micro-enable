////////////////////////////////////////////////////////////////////////////
// ME4 Frame grabber example
//
//
//
// File:	AcqStandard.cpp
//
// Copyrights by Silicon Software 2002-2010
////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "board_and_dll_chooser.h"

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

struct fg_apc_data {
	Fg_Struct *fg;
	unsigned int port;
	dma_mem *mem;
	int displayid;
};

const char *dll_list_me3[] = {
	"DualAreaGray",
	"DualAreaRGB",
	"DualLineGray",
	"DualLineRGB",
	"MediumAreaGray",
	"MediumAreaRGB",
	"MediumLineGray",
	"MediumLineRGB",
	""
};

const char *dll_list_me3xxl[] = {
	"DualAreaGray12XXL",
	"DualAreaRGB36XXL",
	"DualLineGray12XXL",
	"DualLineRGB36XXL",
	"MediumAreaGray12XXL",
	"MediumAreaRGBXXL",
	"MediumLineGray12XXL",
	"MediumLineRGBXXL",
	""
};


const char *dll_list_me4_dual[] = {
	"DualAreaGray16",
	"DualAreaRGB48",
	"DualLineGray16",
	"DualLineRGB30",
	"MediumAreaGray16",
	"MediumAreaRGB36",
	"MediumLineGray16",
	"MediumLineRGB36",
	""
};



const char *dll_list_me4_single[] = {
	"SingleAreaGray16",
	"SingleAreaRGB48",
	"SingleLineGray16",
	"SingleLineRGB48",
	""
};



const char *dll_list_me4_VD_AD4_dual[] = {
	"DualAreaGray16",
	"DualAreaRGB48",
	"DualLineGray16",
	"DualLineRGB30",
	"MediumAreaGray16",
	"MediumAreaRGB36",
	"MediumLineGray16",
	"MediumLineRGB36",
	"Acq_BaseAreaBayer12",
	"Acq_DualBaseAreaBayer8",
	"Acq_DualBaseAreaGray12",
	"Acq_DualBaseAreaRGB24",
	"Acq_MediumAreaBayer12",
	"Acq_MediumAreaGray12",
	"Acq_MediumAreaRGB36",
	"Acq_FullAreaBayer8",
	"Acq_FullAreaGray8",
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




////////////////////////////////////////////////////////////////////////
// Asynchronous procedure call
// Callback function 
int ApcFunc(frameindex_t picNr, struct fg_apc_data *data)
{
	fprintf(stdout, "ApcFunc port %i image %li\n", data->port, picNr);

	::DrawBuffer(data->displayid, Fg_getImagePtrEx(data->fg, picNr, 0, data->mem), (int)picNr, "");

	return 0;
}

////////////////////////////////////////////////////////////////////////
// Main function
//

int main(int argc, char* argv[], char* envp[]){

	int nr_of_buffer	=	8;			// Number of memory buffer
	int nBoard			=	selectBoardDialog();			// Board Number
	int nCamPort		=	PORT_A;		// Port (PORT_A / PORT_B)
	int status = 0;
	Fg_Struct *fg;
	struct FgApcControl ctrl;
	struct fg_apc_data apcdata;
	dma_mem *pMem0;
	int nId;

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
	if((pMem0 = Fg_AllocMemEx(fg,totalBufSize,nr_of_buffer)) == NULL){
		status = ErrorMessageWait(fg);
		return status;
	} else {
		fprintf(stdout,"%d framebuffer allocated for port %d ok\n",nr_of_buffer,nCamPort);
	}

	// Creating a display window for image output
	int Bits = getNoOfBitsFromImageFormat(format);

	nId = ::CreateDisplay(Bits,width,height);
	SetBufferWidth(nId,width,height);

	apcdata.port = nCamPort;
	apcdata.fg = fg;
	apcdata.mem = pMem0;
	apcdata.displayid = nId;

	ctrl.version = 0;
	ctrl.data = &apcdata;
	ctrl.func = ApcFunc;
	ctrl.flags = FG_APC_DEFAULTS;
	ctrl.timeout = 5;

	status = Fg_registerApcHandler(fg, nCamPort, &ctrl, FG_APC_CONTROL_BASIC);
	if (status != FG_OK) {
		fprintf(stderr, "registering APC handler failed: %s\n", Fg_getErrorDescription(fg, status));
		Fg_FreeMemEx(fg, pMem0);
		Fg_FreeGrabber(fg);
		CloseDisplay(nId);
		return status;
	}

	if ((Fg_AcquireEx(fg, nCamPort, GRAB_INFINITE, ACQ_STANDARD, pMem0)) < 0) {
		status = ErrorMessageWait(fg);
		Fg_FreeMemEx(fg, pMem0);
		Fg_FreeGrabber(fg);
		CloseDisplay(nId);
		return status;
	}

#ifdef _WIN32
	Sleep(30000);
#else
	sleep(30);
#endif

	// Freeing the grabber resource
	Fg_registerApcHandler(fg, nCamPort, NULL, FG_APC_CONTROL_BASIC);
	Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
	Fg_FreeMemEx(fg, pMem0);
	Fg_FreeGrabber(fg);

	CloseDisplay(nId);

	return FG_OK;
}

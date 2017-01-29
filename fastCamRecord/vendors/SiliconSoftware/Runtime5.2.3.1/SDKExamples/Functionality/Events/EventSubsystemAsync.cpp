////////////////////////////////////////////////////////////////////////////
// ME4 Frame grabber example
//
// File:		EventsubsystemAsync.cpp
// Remarks: This example shows the usage of the event subsystem by using the
//          asynchronous event interface
//
// Copyrights by Silicon Software 2002-2012
////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>


#include "board_and_dll_chooser.h"
#include "globalFunctions.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>
#include <sisoboards.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>


// data structure containing data to be passed from main procedure to callback handler
struct fg_event_data {
	unsigned int eventCount;
	unsigned int maxEvents;
	uint64_t previousScan;
	int64_t performanceFrequency;
};


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
		"MediumAreaRGB48",
		"MediumLineGray16",
		"MediumLineRGB48",
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


const char *dll_list_me4_GEV[] = {
  "QuadAreaBayer24",
  "QuadAreaGray16",
  "QuadAreaGray8",
  "QuadAreaRgb24",
  "QuadLineGray16",
  "QuadLineGray8",
  "QuadLineRgb24",
  ""
};


const char *events_to_trigger[] = {
  "CamPortATransferStart", // for CameraLink frame grabber 
  "CamPortAImageStatistics", // for GEV frame grabber
  ""
};

// param events: the mask containing the notified events
// param eventInfo: the data structure containing additional event information created at the SDK 
// returns the timestamp of the first notified event out of the event structure
uint64_t getTimestampOfFirstNotifiedEvent( const uint64_t events, const struct fg_event_info * eventInfo)
{
	uint64_t result = 0;

	int indexOfFirstEventAtMask = -1;
	uint64_t eventScanMask = 0x01; 
	bool exitLoop = false;
	int loopCount = 0;

	// find out, which is the LSB at the bitmask in order to grab the according timestamp
	while ((eventScanMask)&&(!exitLoop)){
		if (eventScanMask & events){
			indexOfFirstEventAtMask = loopCount;  // index of the detected bit
			exitLoop = true;
		}	
		eventScanMask <<= 1; // check next Bit at the event mask
		loopCount++;
	}
	
	// get the according timestamp for the first event at the mask
	if (indexOfFirstEventAtMask >= 0) {
		result = eventInfo->timestamp[indexOfFirstEventAtMask];
	}
	return result;
}

// display each nth event at the console and/or create a stop signal after maxEvents notifications
int showEvents(uint64_t events, struct fg_event_data *userData, const struct fg_event_info * eventInfo)
{
	int rc;
	userData->eventCount++;
	if (userData->eventCount > userData->maxEvents){
		fprintf(stdout,"Stopping event notification after %d callbacks\n", userData->eventCount);
		rc = 1; // abort event notifications after a predefined number of notifications
	}
	else{
		if ((userData->eventCount == 1)||((userData->eventCount % 100) == 0)){

			uint64_t timestamp = getTimestampOfFirstNotifiedEvent(events, eventInfo);
			fprintf(stdout,"no. of events received: %d\n", userData->eventCount);
			fprintf(stdout,"timestamp counter: %" PRIu64 "\n", timestamp);
			
			uint64_t elapsedTime = (timestamp - userData->previousScan) * 1000 / userData->performanceFrequency;
			fprintf(stdout,"elapsed since last call: %" PRIu64 "\n", elapsedTime);

			userData->previousScan = timestamp;
		}
		rc = 0;// continue event notifications
	}
	return rc;
}


// callback handler
static int eventsample_event_callback(uint64_t events, struct fg_event_data *userData, const struct fg_event_info * eventInfo)
{
	int rc;
	rc = showEvents(events, userData, eventInfo);
	return rc;
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

///////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[], char* envp[]){


	registerSignalHandler(); // activate ctrl+c handling

	int nr_of_buffer	=	8; // Number of memory buffer
	int nBoard			=	selectBoardDialog();// Board Number
	int nCamPort		=	PORT_A; // Port (PORT_A / PORT_B)
	int MaxPics			=	10000; // Number of images to grab
	int status = 0;
	fg_event_info eventInfo; // control struct
	fg_event_data eventData; // context data to be passed to callback handler
	
	Fg_Struct *fg;

	const char *dllName = NULL;
	int boardType = 0;
	boardType = Fg_getBoardType(nBoard);
	if (!((boardType ==  PN_MICROENABLE4VD4CL )||
		    (boardType ==  PN_MICROENABLE4AD4CL ) ||
        (boardType == PN_MICROENABLE4VQ4GE)||
        (boardType == PN_MICROENABLE4AQ4GE))) {
		fprintf(stdout,"The selected board type doesn't support events\n");
	  fprintf(stdout, " ... press ENTER to continue\n");
    getchar();
		return 0;
	}
	dllName = selectDll(boardType, NULL, NULL, dll_list_me4_dual, dll_list_me4_single, dll_list_me4_VD_AD4_dual, dll_list_me4_GEV);

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
	// Start of event subsystem example
	// ====================================================

	// 1. step: Query available Events
	// note basically the event mask should be retrieved by the name
	// of the events, defined at the corresponding applet.
	// 
	uint64_t availableEvents = 0;
	uint64_t eventScanMask = 0x01; 
	while (eventScanMask){
		const char* eventName = Fg_getEventName(fg,eventScanMask);

		// create a bitmask of all the available events
		if(eventName){
			// store the mask of all events
			availableEvents = availableEvents | eventScanMask;
			
			// double check for eventmask for tutorial reason only:
			uint64_t currentEventMask = Fg_getEventMask(fg, eventName);
			if (currentEventMask){
				fprintf(stdout,"event mask : 0x%016" PRIx64 " event name: %s \n",currentEventMask, eventName);
			}
		}
		// query for the next event
		eventScanMask <<= 1; 
	}
	fprintf(stdout,"Query for available events done\n");


	// activate a certain event mask to be notified
  unsigned int eventToTriggerIndex = 0;
  if ((boardType == PN_MICROENABLE4VQ4GE) || 
    (boardType == PN_MICROENABLE4AQ4GE)) {
      eventToTriggerIndex = 1; // use CamPortAImageStatistics
  }
  if ((boardType == PN_MICROENABLE4VD4CL) ||
    (boardType == PN_MICROENABLE4AD4CL)){
      eventToTriggerIndex = 0; // use CamPortATransferStart
  }
  const char* eventToTrigger = events_to_trigger[eventToTriggerIndex];

	availableEvents = 0;
	availableEvents = availableEvents | Fg_getEventMask(fg,eventToTrigger);// use one specific event
	if(availableEvents){

		// 2. step: Clear event queue
		status = Fg_clearEvents(fg, availableEvents); // clear them all
		if (status != FG_OK){
			ErrorMessage(fg);
			return status;
		}

		// control struct initialization
		memset(&eventInfo, sizeof(eventInfo),0); 
		FG_EVENT_INFO_INIT(&eventInfo);

		// example for context / user data to be passed to the callback
		memset(&eventData, sizeof(eventData),0); 
		eventData.eventCount = 0; // global counter
		eventData.maxEvents = 1000; // termination after 1000 events
		eventData.performanceFrequency = 0;
		eventData.previousScan = 0;


		// Query the timebase from SDK for calculation of timestamps from counter
		status = FG_OK;
		Fg_Info_Selector infoToRequest = INFO_TIMESTAMP_FREQUENCY;
		FgProperty propertyId = PROP_ID_VALUE; 
		char buffer[1024];
		unsigned int buflen = sizeof(buffer);
		int param1 = 0;
		status = Fg_getSystemInformation(fg,infoToRequest, propertyId, param1, &buffer, &buflen);
		if (status == FG_OK){
#ifdef WIN32
			eventData.performanceFrequency = _atoi64(buffer);
#else
			eventData.performanceFrequency = atoll(buffer);
#endif			
		}


		// 3. step: Activate Event listener or wait for the corresponding event
		// in this case: waiting on any of the present events
		// check documentation, wether events are capable to be combined
		status = Fg_registerEventCallback(fg, availableEvents, eventsample_event_callback, &eventData, FG_EVENT_DEFAULT_FLAGS, &eventInfo);
		if (status != FG_OK){
			ErrorMessage(fg);
			return status;
		}
		fprintf(stdout,"Callback Handler registered\n");

		status = Fg_activateEvents(fg, availableEvents, 1);
		if (status != FG_OK){
			ErrorMessage(fg);
			return status;
		}
		fprintf(stdout,"Event mask activated 0x%016" PRIx64 "\n", availableEvents);
	} else{
		fprintf(stdout,"no events detected at this applet \n");
	}


	// ====================================================
	// Acquire images to get events based on image processing chain

	if((Fg_AcquireEx(fg,nCamPort,GRAB_INFINITE,ACQ_STANDARD,pMem0)) < 0){
		status = ErrorMessageWait(fg);
		return status;
	}



	// ====================================================
	// MAIN LOOP
	frameindex_t lastPicNr = 0;
	while(((lastPicNr = Fg_getLastPicNumberBlockingEx(fg,lastPicNr+1,nCamPort,10,pMem0))< MaxPics) &&
	      (!_quit)){
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
	
	// deactivate notification
	Fg_activateEvents(fg,availableEvents, 0);
	// unregister Event handling
	Fg_registerEventCallback(fg, availableEvents, NULL, NULL, FG_EVENT_DEFAULT_FLAGS, NULL);
	

	Fg_stopAcquireEx(fg,nCamPort,pMem0,0);
	Fg_FreeMemEx(fg,pMem0);
	Fg_FreeGrabber(fg);

	CloseDisplay(nId);

	// Cleanup
	unregisterSignalHandler();

	printf (" ... press ENTER to quit\n");
  getchar();

	return FG_OK;
}




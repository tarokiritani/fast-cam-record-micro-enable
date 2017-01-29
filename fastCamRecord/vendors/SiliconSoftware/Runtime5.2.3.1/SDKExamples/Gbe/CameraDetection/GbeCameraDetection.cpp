////////////////////////////////////////////////////////////////////////////
// microEnable IV GigE frame grabber example
//
// Purpose: 
// 1. Demonstration of camera discovery
// 2. Retrieval of GenICam information XML-file from camera and SISO-Applets
//
//
// File:	GbeCameraDetection.cpp
// 
// Based on Runtime 5.2.1
// Copyrights by Silicon Software 2013
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>

#include "board_and_dll_chooser.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>
#include <gbe.h>
#include <gbe_error.h>

#include <vector>

using namespace std;

struct fg_apc_data {
  Fg_Struct *fg;
  unsigned int port;
  dma_mem *mem;
  int displayid;
};

// camera data
// e.g 2nd camera on 3rd port has the values PortNr = 2, CamNrAtPort = 1
struct CamData
{
  int PortNr;			// port number, on which this camera connected
  int CamNrAtPort;	// camera number against the port
}typedef CamData_t;

static vector<CamData_t> vecCamData; // a vector to store datas of all cameras on selected board(not on whole system)
static BoardHandle *g_GEboardHandle = NULL; // GigE-Board handle, it should be only one time initialized and global valid
int nBoard       = -1; // Selected board Number
int nPortNr      = -1; // Port number
int nCamNrAtPort = -1;// Camera number on selected port, not based on camera count of whole system
int status =	0;
const char *dllName = NULL;
Fg_Struct *fg;

const char *dll_list_me3[] = {
  ""
};

const char *dll_list_me3xxl[] = {
  ""
};

const char *dll_list_me4_dual[] = {
  ""
};

const char *dll_list_me4_single[] = {
  ""
};

const char *dll_list_me4_VD_AD4_dual[] = {
  ""
};

const char *dll_list_me4_AQ_VQ4_GE[] = {
  "QuadAreaBayer24",
  "QuadAreaGray8",
  "QuadAreaGray16",
  "QuadAreaRgb24",
  "QuadLineGray8",
  "QuadLineGray16",
  "QuadLineRgb24",
  ""
};


static int selectGEBoardDialog();
static int selectGECameraDialog(int &);
void showCamInfo(int &, int &);
int getGenicamInfoXml(int &, int &);
int AcquisitionDemo(int &, int &);

int ErrorMessage(Fg_Struct *fg)
{
  int error = Fg_getLastErrorNumber(fg);
  const char* err_str = Fg_getLastErrorDescription(fg);
  fprintf(stderr,"Error: %d : %s\n",error,err_str);
  return error;
}

int ErrorMessageWait(Fg_Struct *fg)
  {
  int error = ErrorMessage(fg);
  return error;
}

void ErrorMessageGbe(int &status)
{
  const char *err_text = Gbe_getErrorDescription(status);
  fprintf(stderr, "Error: %d : %s\n", status, err_text);
  return;
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

void programEndMessage()
{
  fflush(stdin);
  printf ("\nProgram end, press ENTER to continue");
  getchar();
}



void ReleaseBoardHandles(Fg_Struct* fg, dma_mem* pMem0, BoardHandle* bh, CameraHandle* ch)
{
  if (ch > 0){
    Gbe_disconnectCamera(ch);
    Gbe_freeCamera(ch);
  }
  Gbe_freeBoard(bh);

  Fg_FreeMemEx(fg,pMem0);
  Fg_FreeGrabber(fg);
  return;
}



////////////////////////////////////////////
// Asynchronous procedure call
// Callback function 
int ApcFunc(frameindex_t picNr, struct fg_apc_data *data)
{
  fprintf(stdout, "ApcFunc port %i image %li\n", data->port, picNr);
  ::DrawBuffer(data->displayid, Fg_getImagePtrEx(data->fg, picNr, 0, data->mem), (int)picNr, "");
  return 0;
}


////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////
int main(int argc, char* argv[], char* envp[])
{
  int nFunc   = 0;  // Availible function
  int nCamSel = -1; // Selected camera number based on system camera count
  nBoard      = selectGEBoardDialog();	 

  if(nBoard < 0){
    return 0;
  }

  // ====================================================
  // Gbe Init
  // Gbe-board should be initialized only one time and the
  // board handle should be a global variable
  // ====================================================
  if((status = Gbe_initBoard(nBoard,0,&g_GEboardHandle)) < 0){
    fprintf(stdout, "Gbe_initBoard error\n");
    ErrorMessageGbe(status);
    programEndMessage();
    return 0;
  }
  // camera discovery
  if((status = Gbe_scanNetwork(g_GEboardHandle,-1,200)) < 0){
    fprintf(stdout, "Gbe_scanNetwork error\n");
    ErrorMessageGbe(status);
    programEndMessage();
    return 0;
  }

  // select camera on the selected board
  if((nCamSel = selectGECameraDialog(nBoard)) < 0){
    programEndMessage();
    return 0;
  }

  // ====================================================
  // choose a function
  // ====================================================
  printf("\n=====================================\n\n");
  printf("0. Show detail informations of the selected camera\n");
  printf("1. Read and save GeniCam-XML file to local\n");
  printf("2. Acquisition with selected camera\n");
  printf("\nPlease select a function: ");

  fflush(stdout);
  int userInput = -1;
  do {
    char inputBuffer[512];
    userInput = readIntFromStdin(inputBuffer, sizeof(inputBuffer));
    if (userInput > 2) {
      printf("Invalid selection, retry[0-%u]: ", 2);
    }
  } while (userInput > 2);
  nFunc = userInput;

  ////////////////////////////////////////////////////////////////////
  //MAIN process
  switch(nFunc){
    case 0:	// Show the detail informations of camera
    {
      showCamInfo(nBoard, nCamSel);
      break;
    }
    case 1: // Read and save GeniCam-XML file to local
    {
      getGenicamInfoXml(nBoard, nCamSel);
      break;
    }
    case 2:	// acquisition with GigE-camera
    {
      AcquisitionDemo(nBoard, nCamSel);
      break;
    }
  }
  programEndMessage();
  return FG_OK;
}

///////////////////////////////////////////////////////
// get information of all boards in system
// using silicon-software runtime library fglib5
///////////////////////////////////////////////////////
static int selectGEBoardDialog()
{
  int boardType;
  int i = 0;
  const int maxNrOfboards = 10;// use a constant no. of boards to query, when evaluations versions minor to RT 5.2
  int nrOfBoardsFound = 0;
  int nBoardSel = -1;
  int nrOfBoardsPresent = 0;
  int nrOfGEBoard = 0;
  int maxBoardIndex = 0;

  // detect all boards
  nrOfBoardsPresent = getNrOfBoards();
  if(!nrOfBoardsPresent)
    return 0;

  for(i = 0; i < maxNrOfboards; i++) {
    const char * boardName;
    bool skipIndex = false;
    boardType = Fg_getBoardType(i);
    switch(boardType) {
      case PN_MICROENABLE4AQ4GE:
        boardName = "MicroEnable IV AQ4-GE";
        nrOfGEBoard++;
        break;
      case PN_MICROENABLE4VQ4GE:
        boardName = "MicroEnable IV VQ4-GE";
        nrOfGEBoard++;
        break;
      case PN_MICROENABLE4AS1CL:
        boardName = "MicroEnable IV AS1-CL";
        break;
      case PN_MICROENABLE4AD1CL:
        boardName = "MicroEnable IV AD1-CL";
        break;
      case PN_MICROENABLE4VD1CL:
        boardName = "MicroEnable IV VD1-CL";
        break;
      case PN_MICROENABLE4AD4CL:
        boardName = "MicroEnable IV AD4-CL";
        break;
      case PN_MICROENABLE4VD4CL:
        boardName = "MicroEnable IV VD4-CL";
        break;
      case PN_MICROENABLE3I:
        boardName = "MicroEnable III";
        break;
      case PN_MICROENABLE3IXXL:
        boardName = "MicroEnable III-XXL";
        break;
      default:
        boardName = "Unknown / Unsupported Board";
        skipIndex = true;
    }
    if (!skipIndex){
      printf("%d. Board_%u %s (%x)\n", nrOfBoardsFound, i, boardName, boardType);
      nrOfBoardsFound++;
      maxBoardIndex = i;
    }
    if (nrOfBoardsFound >= nrOfBoardsPresent)
      break;// all boards are scanned
  }

  bool bSelect = false;
  if(nrOfGEBoard > 0){
    printf("Please choose a GigE-board: ");
    fflush(stdout);
    int userInput = -1;
    do {
	    char inputBuffer[512];
	    userInput = readIntFromStdin(inputBuffer, sizeof(inputBuffer));
	    if (userInput > maxBoardIndex) {
		    printf("Index number invalid, retry[0-%u]: ", maxBoardIndex);
	    }
	    // verify the board type, it must be a GigE board
	    else{
		    boardType = Fg_getBoardType(userInput);
		    if( boardType == PN_MICROENABLE4AQ4GE || boardType == PN_MICROENABLE4VQ4GE){
			    nBoardSel = userInput;
			    bSelect = true;	
		    }					
		    else
			    printf("Your selection is not a GigE-board, please try again:");
	    }
    } while (!bSelect);
  }
  else
    printf("No GigE boards found!\n");

  return nBoardSel;
  }

///////////////////////////////////////////////////////
// Show the detail informations of selected GigE-camera
///////////////////////////////////////////////////////
void showCamInfo(int &nBoard, int &nCamSel)
{
  int status;

  nCamNrAtPort = vecCamData[nCamSel].CamNrAtPort;
  nPortNr = vecCamData[nCamSel].PortNr;

  if(!g_GEboardHandle)
    return;

  CameraHandle *hCam;
  if((status = Gbe_getCameraByIndex(g_GEboardHandle, nPortNr, nCamNrAtPort, &hCam)) >= 0){
    CameraInfo *CamInfo = Gbe_getCameraInfo(hCam);
    char sIP[20];
    char sMAC[20];
    sprintf(sIP, "%d.%d.%d.%d", (CamInfo->ipv4 >> 24),
                                ((CamInfo->ipv4 & 0xFFFFFF) >> 16),
                                ((CamInfo->ipv4 & 0xFFFF) >> 8),
                                ((CamInfo->ipv4 & 0xFF)));
    sprintf(sMAC, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",	CamInfo->mac[0],
            CamInfo->mac[1],
            CamInfo->mac[2],
            CamInfo->mac[3],
            CamInfo->mac[4],
            CamInfo->mac[5]);

    printf("\n==============================================\n");
    printf("Information about camera %d on board %d port %d:\n\n", nCamNrAtPort, nBoard, nPortNr );
    printf("Model:           %s\n", CamInfo->model_name);
    printf("Manufacturer:    %s\n", CamInfo->manufactor_name);
    printf("Serial number:   %s\n", CamInfo->serial_number);
    printf("User name:       %s\n", CamInfo->user_name);
    printf("IP:              %s\n", sIP);
    printf("MAC:             %s\n", sMAC);
    printf("\n==============================================\n");
    Gbe_freeCamera(hCam);
  }
}


////////////////////////////////////////////////
// Show all available cameras on selected board
///////////////////////////////////////////////
static int selectGECameraDialog(int &nBoard)
  {
  int status;
  int iPortCount = 4;		// GigE-board generally has 4 ports
  int NrOfCamsPresent = 0;
  int nCamSel = -1;

  printf("\n=====================================\n\n");
  printf("following GE-camera on the selected board is found:\n");

  if(!g_GEboardHandle)
    return -1;

  //Find all cameras on this board
  for(int i = 0; i < iPortCount; i++){
    int NrOfCams = 0;
    NrOfCams = Gbe_getCameraCount(g_GEboardHandle, i);	// Find all cameras on actual port
    for(int j = 0; j < NrOfCams; j++){
      CameraHandle *hCam;
      if((status = Gbe_getCameraByIndex(g_GEboardHandle, i, j, &hCam)) >= 0){
        CameraInfo *CamInfo = Gbe_getCameraInfo(hCam);
        printf("%d. port %d camera %d --- %s\n", NrOfCamsPresent, i, j, CamInfo->model_name);
        // transform the port number and index id into a WORD value
        // camera port number stored in high byte , index id stored in low byte
        // e.g. 2nd camera on 3rd port has a value 770
        CamData_t CamData;
        CamData.PortNr = i;
        CamData.CamNrAtPort = j;
        vecCamData.push_back(CamData);
        NrOfCamsPresent++;
      }
    }
  }

  bool bSelect = false;
  if(NrOfCamsPresent > 0){
    printf("\nPlease choose a camera: ");
    fflush(stdout);
    int userInput = -1;
    do {
      char inputBuffer[512];
      userInput = readIntFromStdin(inputBuffer, sizeof(inputBuffer));
      if (userInput > NrOfCamsPresent - 1) {
        printf("Index number invalid, retry[0-%u]: ", NrOfCamsPresent - 1);
      }
      else{
        nCamSel = userInput;
        bSelect = true;
      }
    } while (!bSelect);
  }
  else
    printf("No GigE cameras found!\n");

  return nCamSel;
  }

//////////////////////////////////////////////////////////////////////
// Retrieval GenICam information xml file from camera and SISO-Applets
//////////////////////////////////////////////////////////////////////
int getGenicamInfoXml(int &nBoard, int &nCamSel)
  {
  bool bSelect = false;
  int boardType = Fg_getBoardType(nBoard);

  // because the applets GeniCam-xml file differs from each other
  // a certain  applet must be selected
  dllName = selectDll(boardType, 
                       dll_list_me3, 
                       dll_list_me3xxl, 
                       dll_list_me4_dual, 
                       dll_list_me4_single, 
                       dll_list_me4_VD_AD4_dual,
                       dll_list_me4_AQ_VQ4_GE);

  nCamNrAtPort = vecCamData[nCamSel].CamNrAtPort;
  nPortNr = vecCamData[nCamSel].PortNr;

  fprintf(stdout, "\n=====================================\n\n");
  fprintf(stdout, "Please input a valid path to store the file\n");
  fprintf(stdout, "The file will be overwritten if it already exists: ");

  fflush(stdout);
  char sPath[512];
  char file[512];

  // input a path to save xml file
  fflush(stdout);
  do 
  {
    int iError;

    fscanf(stdin, "%s", &sPath);

#if defined(WIN32)
	// verify if the output path is valid
    if(GetFileAttributesA(sPath) == INVALID_FILE_ATTRIBUTES && ((iError = GetLastError()) == ERROR_FILE_NOT_FOUND))
      fprintf(stdout, "Invalid input, please retry: ");
    else
      bSelect = true;
#else
	bSelect = true;
#endif
  } while (!bSelect);

  // board init
  if((fg = Fg_Init(dllName, nBoard)) == NULL) {
    fprintf(stderr, "Fg_Init error\n");
    status = ErrorMessageWait(fg);
    return -1;
  }

  char * xmlBuf = NULL;
  size_t xmlBufSize = 0;

  ////////////////////////////////////////////////
  // retrieval GeniCam xml file form SISO-applet
  // read SISO Applet GenICam-Info as xml
  ////////////////////////////////////////////////
  if((status = Fg_getParameterInfoXML(fg, nPortNr, xmlBuf, &xmlBufSize)) == FG_OK)
  {
    xmlBuf = new char[xmlBufSize];
    if(Fg_getParameterInfoXML(fg, nPortNr, xmlBuf, &xmlBufSize) != FG_OK)
    {
      fprintf(stderr, "Fg_getParameterInfoXML error\n");
      status = ErrorMessageWait(fg);
      delete xmlBuf;
      return -1;
    }
  }

  // write the SISO-Applet GeniCam xml file to wish location
  sprintf(file, "%s\\GeniCamInfo_%s_%d.xml", sPath, dllName, nPortNr);
  FILE *pFile;
  pFile = fopen(file, "w");
  if(pFile)
  {
    fwrite((const void*)xmlBuf, 1, xmlBufSize, pFile);
    fclose(pFile);
    delete xmlBuf;
    fprintf(stdout, "The file saving of \"%s\" is successfully done\n", file);		
  }
  else
  {
    fprintf(stderr, "Error! Cannot save the file, please check the storage status\n");
    delete xmlBuf;
    fclose(pFile);
    return -1;
  }

  xmlBufSize = 0;


  ////////////////////////////////////////////////
  // retrieval GeniCam xml file form camera
  // connect camera
  ////////////////////////////////////////////////
  CameraHandle *hCam;
  CameraInfo *CamInfo = NULL;
  if((status = Gbe_getCameraByIndex(g_GEboardHandle, nPortNr, nCamNrAtPort, &hCam)) >= 0)
  {
    CamInfo = Gbe_getCameraInfo(hCam);

    if((status = Gbe_connectCamera(hCam)) < 0){
	    fprintf(stderr, "Gbe_connectCamera error\n");
	    ErrorMessageGbe(status);
	    return -1;
    }
  }
  else
  {
    fprintf(stderr, "Gbe_getCameraByIndex error\n");
    ErrorMessageGbe(status);
    return -1;
  }

  // read GenICam-Info from camera
  if((status = Gbe_getGenICamXML(hCam, NULL, &xmlBufSize)) == GBE_OK)
  {
    xmlBuf = new char[xmlBufSize];
    if((status = Gbe_getGenICamXML(hCam, xmlBuf, &xmlBufSize)) != GBE_OK)
    {
      fprintf(stderr, "Gbe_getGenICamXML error\n");
      ErrorMessageGbe(status);
      delete xmlBuf;
      return -1;
    }
  }
  else
  {
    fprintf(stderr, "Gbe_getGenICamXML error\n");
    ErrorMessageGbe(status);
    return -1;
  }

  // write the camera GeniCam xml file to wish location
  sprintf(file, "%s\\GeniCamInfo_%s.xml", sPath, CamInfo->model_name);
  pFile = NULL;
  pFile = fopen(file, "w");
  if(pFile)
  {
    fwrite((const void*)xmlBuf, 1, xmlBufSize, pFile);
    fclose(pFile);
    delete xmlBuf;
    fprintf(stdout, "The file saving of \"%s\" is successfully done\n", file);		
  }
  else
  {
    fclose(pFile);
    delete xmlBuf;
    fprintf(stderr, "Error! Cannot save the file, please check the storage status\n");
    return -1;
  }
  return 0;
  }

///////////////////////////////////////////////////////
// demo acquire images
///////////////////////////////////////////////////////
int AcquisitionDemo(int &nBoard, int &nCamSel)
  {
  int nr_of_buffer	=	8;			// Number of memory buffer
  int MaxPics			=	3000;		// Number of images to grab

  int boardType = 0;
  struct FgApcControl ctrl;
  struct fg_apc_data apcdata;

  nCamNrAtPort = vecCamData[nCamSel].CamNrAtPort;
  nPortNr = vecCamData[nCamSel].PortNr;
  boardType = Fg_getBoardType(nBoard);
  dllName = selectDll(boardType, 
                        dll_list_me3, dll_list_me3xxl, 
                        dll_list_me4_dual, dll_list_me4_single, 
                        dll_list_me4_VD_AD4_dual,
                        dll_list_me4_AQ_VQ4_GE);

  // Initialization of the microEnable frame grabber
  if((fg = Fg_Init(dllName, nBoard)) == NULL) {
    status = ErrorMessageWait(fg);
    ReleaseBoardHandles(fg, NULL,g_GEboardHandle, 0);
  }
  fprintf(stdout,"Init Grabber ok\n");

  // Setting the image size
  int width	= 640;
  int height	= 480;
  if (Fg_setParameter(fg,FG_WIDTH,&width,nPortNr) < 0) {
    status = ErrorMessageWait(fg);
    ReleaseBoardHandles(fg, NULL,g_GEboardHandle, 0);
    return -1;
  }
  if (Fg_setParameter(fg,FG_HEIGHT,&height,nPortNr) < 0) {
    status = ErrorMessageWait(fg);
    ReleaseBoardHandles(fg, NULL,g_GEboardHandle, 0);
    return -1;
  }

  fprintf(stdout,"Set Image Size on port %d (w: %d,h: %d) ok\n",nPortNr,width,height);

  // Memory allocation
  int format;
  Fg_getParameter(fg,FG_FORMAT,&format,nPortNr);
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
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, 0);
    return 0;
  } else{
    fprintf(stdout,"%d frame buffer allocated for port %d ok\n",nr_of_buffer,nPortNr);
  }

  // Creating a display window for image output
  int Bits = getNoOfBitsFromImageFormat(format);

  int nId = ::CreateDisplay(Bits,width,height);
  SetBufferWidth(nId,width,height);

  // set Gbe Parameters
  CameraHandle *ch= NULL;
  if((status = Gbe_getCameraByIndex(g_GEboardHandle, nPortNr, nCamNrAtPort, &ch)) < 0){
    fprintf(stderr, "Gbe_getCameraByIndex error\n");
    ErrorMessageGbe(status);
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, 0);
    return -1;
  }

  if((status = Gbe_connectCamera(ch)) < 0){
    fprintf(stderr, "Gbe_connectCamera error\n");
    ErrorMessageGbe(status);
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, ch);
    return -1;
  }

  if((status = Gbe_setIntegerValue(ch, "Width", width)) < 0){
    fprintf(stderr, "Invalid width value\n");
    ErrorMessageGbe(status);
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, ch);
    return -1;
  }

  if((status = Gbe_setIntegerValue(ch, "Height", height)) < 0) {
    fprintf(stderr, "Invalid height value\n");
    ErrorMessageGbe(status);
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, ch);
    return -1;
  }

  // set the APC parameters
  apcdata.port = nPortNr;
  apcdata.fg = fg;
  apcdata.mem = pMem0;
  apcdata.displayid = nId;

  ctrl.version = 0;
  ctrl.data = &apcdata;
  ctrl.func = ApcFunc;
  ctrl.flags = FG_APC_DEFAULTS;
  ctrl.timeout = 5;

  status = Fg_registerApcHandler(fg, nPortNr, &ctrl, FG_APC_CONTROL_BASIC);
  if (status != FG_OK) {
    fprintf(stderr, "registering APC handler failed: %s\n", Fg_getErrorDescription(fg, status));
    status = ErrorMessageWait(fg);
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, ch);
    return -1;
  }

  //====================================================
  // First Fg_AcquireEx, then Gbe_startAcquisition, its avoid the frame lost
  if((Fg_AcquireEx(fg,nPortNr,GRAB_INFINITE,ACQ_STANDARD,pMem0)) < 0){
    status = ErrorMessageWait(fg);
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, ch);
    return -1;
  }

  if((status = Gbe_startAcquisition(ch)) < 0) {
    ErrorMessageGbe(status);
    ReleaseBoardHandles(fg, pMem0,g_GEboardHandle, ch);
    return -1;
  }


  #ifdef _WIN32
  Sleep(15000);
  #else
  sleep(30);
  #endif


  //====================================================
  // Freeing the Gbe resource
  //====================================================

  // First Gbe_stopAcquisition, then Fg_stopAcquireEx
  Gbe_stopAcquisition(ch);
  // Freeing the grabber resource
  Fg_stopAcquireEx(fg,nPortNr,pMem0,0);

  ReleaseBoardHandles(fg, pMem0, g_GEboardHandle, ch);

  CloseDisplay(nId);
  return 0;
}
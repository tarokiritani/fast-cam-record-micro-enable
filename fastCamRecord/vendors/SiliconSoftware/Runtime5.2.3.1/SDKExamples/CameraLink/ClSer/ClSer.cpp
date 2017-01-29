////////////////////////////////////////////////////////////////////////////
// Silicon Software CameraLink serial port example
//
// File:	  ClSer.cpp
//
// Content: This program demonstrates the easy usage of camera-link serial 
//          interface library "clsersis.lib" in order to communicate with a
//          camera connected to a Silicon Software CameraLink frame grabber
//
// Copyrights by Silicon Software 2012
//
////////////////////////////////////////////////////////////////////////////


#ifdef USE_CLALLSERIAL
#include <clallserial.h>
#else
#include <clser.h>
#endif

#ifdef _WIN32
#include <io.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "os_type.h"
#include <board_and_dll_chooser.h>

using namespace std;


static void WriteData(const char *line, void *h)
{
	int error;
	unsigned int len = static_cast<unsigned int>(strlen(line));

	error = clSerialWrite(h, const_cast<char *>(line), &len, 500);
	if (error < 0) {
		std::cerr << "write error " << error << std::endl;
	}
}

static int ReadData(char* line,void *h, unsigned int length)
{
	int ret;

	ret = clSerialRead(h, line, &length, 256);
	line[length] = 0;

	return length;
}

// get board and camera-link serial port information
// using silicon-software runtime library fglib5
int getBoardInfo()
{
  int boardType;
  int i = 0;

  int maxNrOfboards = 10;// use a constant no. of boards to query, when evaluations versions minor to RT 5.2
  int nrOfBoardsFound = 0;
  int maxBoardIndex = -1;
  int iPortCount = 0;
  int nrOfBoardsPresent = 0;

  // detect all boards
  nrOfBoardsPresent = getNrOfBoards();
  if(!nrOfBoardsPresent)
    return 0;

  for(i = 0; i < maxNrOfboards; i++) {
    int iPortNrOnBoard = 0;
    const char * boardName;
    bool skipIndex = false;
    boardType = Fg_getBoardType(i);
    switch(boardType) {
      case PN_MICROENABLE4AS1CL:
        boardName = "MicroEnable IV AS1-CL";
        iPortNrOnBoard = 1;
        break;
      case PN_MICROENABLE4AD1CL:
        boardName = "MicroEnable IV AD1-CL";
        iPortNrOnBoard = 1;
        break;
      case PN_MICROENABLE4VD1CL:
        boardName = "MicroEnable IV VD1-CL";
        iPortNrOnBoard = 2;
        break;
      case PN_MICROENABLE4AD4CL:
        boardName = "MicroEnable IV AD4-CL";
        iPortNrOnBoard = 2;
        break;
      case PN_MICROENABLE4VD4CL:
        boardName = "MicroEnable IV VD4-CL";
        iPortNrOnBoard = 2;
        break;
      // ignore the non-cameralink boards
      case PN_MICROENABLE3I:
      case PN_MICROENABLE3IXXL:
      case PN_MICROENABLE4AQ4GE:
      case PN_MICROENABLE4VQ4GE:
      default:
        boardName = "Unknown / Unsupported Board";
        skipIndex = true;
    }
    if (!skipIndex){
      nrOfBoardsFound++;
      maxBoardIndex = i;

      if(iPortNrOnBoard > 0){
        if(i==0)
          printf("Following serial ports are available:\n");
        for(int j = 0; j < iPortNrOnBoard; j++){
          iPortCount++;
          printf("%d. Board_%u %s (%x) Port_%d\n", iPortCount-1, i, boardName, boardType, j);
        }
      }
    }
    else{
    }
    if (nrOfBoardsFound >= nrOfBoardsPresent){
      break;// all boards are scanned
    }
  }
  return iPortCount;
}

void showWaitMessage()
{
  printf("Press any key to continue\n");
  getchar();
  return;
}



int main(int argc, char* argv[])
{
  void *hSer = NULL;  // reference to serial port
  int iPortNr = -1;
  unsigned int iPortCount = 0;  // count of serial port in whole system
  int iPortCountCheck = -1;	
  int iRet = CL_ERR_NO_ERR;

  unsigned int iErrTextSize = 0;
  unsigned int clSerBaudRate = CL_BAUDRATE_9600;	// baud rate of serial port, normally 9600

  // start up
  iPortCountCheck = getBoardInfo(); 	

  // show available ports at the frame grabber
  if((iRet = clGetNumSerialPorts(&iPortCount)) == CL_ERR_NO_ERR && iPortCount == iPortCountCheck ){
    printf("\nPlease choose the port number(0-%u): ", iPortCount - 1);
    fflush(stdout);
    unsigned int userInput = -1;
    do {
      char inputBuffer[512];
      userInput = readIntFromStdin(inputBuffer, sizeof(inputBuffer));
      if (userInput > iPortCount) {
        printf("Invalid selection, retry[0-%u]: ", iPortCount - 1);
      }
    } while (userInput > iPortCount);

    iPortNr = userInput;
    // initialize serial port
    if((iRet = clSerialInit(iPortNr, &hSer)) != CL_ERR_NO_ERR){
      cerr << "Cannot open connection to CameraLink port\n" << endl;
      // get size of Buffer
      clGetErrorText(iRet, NULL, &iErrTextSize);
      if( iErrTextSize > 0 ){
        char *ErrText = NULL;	
        // retrieve the Buffer 
        ErrText = new char[iErrTextSize];
        clGetErrorText(iRet, ErrText, &iErrTextSize);
        cerr << ErrText << endl;
        delete ErrText;
      }
      showWaitMessage();
      return iRet;
    }

    // set baudrate
    if ( (iRet = clSetBaudRate(hSer, clSerBaudRate)) != CL_ERR_NO_ERR) {
      cerr << endl << "Cannot set baud rate to requested value" << endl;
      // get size of Buffer
      clGetErrorText(iRet, NULL, &iErrTextSize);
      if( iErrTextSize > 0 ){
        char *ErrText = NULL;	
        ErrText = new char[iErrTextSize];
        clGetErrorText(iRet, ErrText, &iErrTextSize);
        cerr << ErrText << endl;
        delete ErrText;
      }
      showWaitMessage();
      return iRet;
    }

    // select functions
    cout << endl <<"0. Show information of this serial port library" << endl <<
                   "1. Show the system identifier of this port" << endl <<
                   "2. Enter and execute any camera commands" << endl;
    printf("\nPlease choose a option: ");

    userInput = -1;
    do {
      char inputBuffer[512];
      userInput = readIntFromStdin(inputBuffer, sizeof(inputBuffer));
      if (userInput > iPortCount) {
        printf("Invalid selection, retry[0-%u]: ", 2);
      }
    } while (userInput > 2);

    switch(userInput){
      // read the manufacturer info of clsersis.dll
      case 0: 
      {
        char *buf = NULL;
        unsigned int ibufsize = 0;
        unsigned int iversion = 0;
        if((iRet = clGetManufacturerInfo(buf, &ibufsize, &iversion)) == CL_ERR_BUFFER_TOO_SMALL){
          if(ibufsize > 0)
          {
            buf = new char[ibufsize];
            clGetManufacturerInfo(buf, &ibufsize, &iversion);
            printf("Manufacturer: %s, version: %u\n", buf, iversion);
            delete buf;
            showWaitMessage();
          }
        }
        else{
          return iRet;
        }
        break;
      }
      // read the identifier of this serial port
      case 1: 
      {
        char *buf = NULL;
        unsigned int ibufsize = 0;
        if((iRet = clGetSerialPortIdentifier(userInput, buf, &ibufsize)) == CL_ERR_BUFFER_TOO_SMALL){
          if(ibufsize > 0)
          {
            buf = new char[ibufsize];
            clGetSerialPortIdentifier(userInput, buf, &ibufsize);
            printf("Port identifier: %s\n", buf);
            delete buf;
            showWaitMessage();
          }
        }
        else{
          return iRet;
        }
        break;
      }
      // execute any camera commands
      // note that cameras from each manufacturer has different commands
      // read the camera manual before you go through this part
      case 2:
      {
        char line[4096];
        size_t len;

        printf("Enter a valid camera command:\n");

        cout << ">" << flush;
        fgets(line,4096,stdin);
        WriteData(line, hSer);
        Sleep(100);
        printf("\nCommand sent\n");

        printf("\nCamera response:\n");
        char lastbyte = '\n';
        do{
          len = ReadData(line, hSer, 1);
          if (len == 1) {
            std::cout << *line;
            lastbyte = *line;
            Sleep(100);
          }
        } while(len > 0);
        if (lastbyte != '\n'){
          std::cout << std::endl;
        }
        showWaitMessage();
      }
      default:
        break;
    }

    // clean up the serial port reference
    clSerialClose(hSer);
  }
  else{
    printf("Cannot find any CameraLink serial ports at system, example aborted.");
    iRet = -1;
  }
  return iRet;
}


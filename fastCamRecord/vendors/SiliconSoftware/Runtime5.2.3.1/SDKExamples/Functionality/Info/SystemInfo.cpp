////////////////////////////////////////////////////////////////////////////
// ME4 Frame grabber example
//
//
//
// File:	SystemInfo.cpp
//
// Copyrights by Silicon Software 2002-2012
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>

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


int GetAndShowInformation( Fg_Struct* fg, Fg_Info_Selector infoToRequest, const int param1)
{
	int status = FG_OK;
	FgProperty propertyId = PROP_ID_NAME; // human readable name of the parameter
	char buffer[1024];
	unsigned int buflen = sizeof(buffer);
	status = Fg_getSystemInformation(fg,infoToRequest, propertyId, param1, buffer, &buflen);
	if (status == FG_OK){
		fprintf(stdout,"Info about %s\n", buffer);		

		fprintf(stdout,"  Name:      %s\n", buffer);		

		propertyId = PROP_ID_DATATYPE;// data type of this field
		buflen = sizeof(buffer);
		status = Fg_getSystemInformation(fg,infoToRequest, propertyId, param1, buffer, &buflen);
		if (status != FG_OK){
			return status;
		}
		else{
			// you might use the data type in order to evaluate the requested information
			// and do some processing steps. The meaning of the datatype can be seen at the
			// documentation and vary from string-> integer-> double
			int typeOfField = atoi(buffer);
			switch(typeOfField){
				case FG_PARAM_TYPE_INT32_T:
				case FG_PARAM_TYPE_UINT32_T:
					{
						// use an int32
						fprintf(stdout,"  Data type: int32\n");		
						break;
					}
				case FG_PARAM_TYPE_INT64_T:
				case FG_PARAM_TYPE_UINT64_T:
				{
					// use an int64
					fprintf(stdout,"  Data type: int64\n");		
					break;
				}

				case FG_PARAM_TYPE_CHAR_PTR:
				{
					// use an null terminated string
					fprintf(stdout,"  Data type: string\n");		
					break;
				}
				case FG_PARAM_TYPE_DOUBLE:
				{
					// use an floating point value
					fprintf(stdout,"  Data type: floating point value\n");		
					break;
				}
			}
		}

		propertyId = PROP_ID_VALUE;// actual value of this field
		buflen = sizeof(buffer);
		status = Fg_getSystemInformation(fg,infoToRequest, propertyId, param1, buffer, &buflen);
		if (status != FG_OK){
			return status;
		}
		fprintf(stdout,"  Value:     %s\n", buffer);		
	}
	return status;
}

///////////////////////////////////////////////////////////////////////
// Main function
//

int main(int argc, char* argv[], char* envp[]){

	int nBoard			=	selectBoardDialog();			// Board Number
	int status = 0;
	Fg_Struct *fg;

	const char *dllName = NULL;

	int boardType = 0;
	boardType = Fg_getBoardType(nBoard);
	dllName = selectDll(boardType, dll_list_me3, dll_list_me3xxl, dll_list_me4_dual, dll_list_me4_single, dll_list_me4_VD_AD4_dual, NULL);

	// Initialization of the microEnable frame grabber, needn't necessarily to be done in this example.
	// Dependend on the requested information
	if((fg = Fg_Init(dllName,nBoard)) == NULL) {
		status = ErrorMessageWait(fg);
		return status;
	}
	fprintf(stdout,"Init Grabber ok\n");

	int param1; // Additional parameter, usage depending on the requested information
	Fg_Info_Selector infoToRequest; // the ID of the information to be requested

	// Requesting information 
	infoToRequest = INFO_NR_OF_BOARDS;
	param1 = 0; // not used for INFO_NR_OF_BOARDS
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_BOARDNAME;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_BOARDTYPE;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_BOARDSERIALNO;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_FIRMWAREVERSION;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_DRIVERVERSION;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_DRIVERARCH;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_BOARDSTATUS;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;

	infoToRequest = INFO_STATUS_PCI_PAYLOAD_MODE;
	param1 = nBoard;// Board index in this case
	status = GetAndShowInformation(fg, infoToRequest, param1);
	if (status != FG_OK) return status;


	// No error, wait until the user acknoledges the display
	fprintf(stdout,"\n");		
	status = ErrorMessageWait(fg);

	// ====================================================
	Fg_FreeGrabber(fg);

	return FG_OK;
}

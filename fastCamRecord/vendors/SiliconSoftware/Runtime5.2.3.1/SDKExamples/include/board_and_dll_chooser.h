#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <sisoboards.h>

static int readIntFromStdin(char * buffer, const size_t len)
{
	int readRet = 0;
	memset(buffer, 0, len);
	int inputIsInvalid = 0;
	unsigned int i = 0;
	for(i = 0; i < len; i++) {

		readRet = read(0, buffer + i, 1);
		if(readRet < 0) {
			exit(readRet);
		}

		if(buffer[i] == '\r') {
			readRet = read(0, buffer + i, 1);
			if(readRet < 0) {
				exit(readRet);
			}
			if(buffer[i] != '\n') {
				exit(-1);
			}
		}
		if(buffer[i] == '\n') {
			break;
		}
		if(buffer[i] < '0' || buffer[i] > '9') {
			inputIsInvalid = 1;
			break;
		}
		if(i == len - 1 && '\n') {
			inputIsInvalid = 1;
			break;
		}
	}

	if(inputIsInvalid) {
		char ownBuff;
		do {
			readRet = read(0, &ownBuff, 1);
			if(readRet < 0) {
				exit(-1);
			}
		} while(ownBuff != '\n');

		return -1;
	}

	if(i == 0) {
		return -1;
	}

	return strtoul(buffer, NULL, 10);
}


static int getNrOfBoards()
{
	int nrOfBoards = 0;
	char buffer[256];
	unsigned int buflen = 256;
	buffer[0]= 0;

	// availability : starting with RT 5.2
	if (Fg_getSystemInformation(NULL,INFO_NR_OF_BOARDS,PROP_ID_VALUE,0,buffer, &buflen) == FG_OK){
		nrOfBoards = atoi(buffer);
	}
	return nrOfBoards;

}

static int selectBoardDialog()
{
	int boardType;
	int i = 0;

	int maxNrOfboards = 10;// use a constant no. of boards to query, when evaluations versions minor to RT 5.2
	int nrOfBoardsFound = 0;
	int nrOfBoardsPresent = getNrOfBoards();
	int maxBoardIndex = -1;

	for(i = 0; i < maxNrOfboards; i++) {
		const char * boardName;
		bool skipIndex = false;
		boardType = Fg_getBoardType(i);
		switch(boardType) {
			case PN_MICROENABLE3I:
				boardName = "MicroEnable III";
				break;
			case PN_MICROENABLE3IXXL:
				boardName = "MicroEnable III XXL";
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
			case PN_MICROENABLE4AQ4GE:
				boardName = "MicroEnable IV AQ4-GE";
				break;
			case PN_MICROENABLE4VQ4GE:
				boardName = "MicroEnable IV VQ4-GE";
				break;
			case PN_MICROENABLE5AQ8CXP4:
				boardName = "MicroEnable V AQ8-CXP";
				break;
			case PN_MICROENABLE5VQ8CXP4:
				boardName = "MicroEnable V VQ8-CXP";
				break;
			default:
				boardName = "Unknown / Unsupported Board";
				skipIndex = true;
		}
		if (!skipIndex){
			printf("Board ID %2u: %s (%x)\n", i, boardName, boardType);
			nrOfBoardsFound++;
			maxBoardIndex = i;
		}
		else{
			
		}
		if (nrOfBoardsFound >= nrOfBoardsPresent){
			break;// all boards are scanned
		}
	}


	if(nrOfBoardsFound <= 0) {
		printf("No Boards found!");
		return -1;
	}

	printf("\n=====================================\n\nPlease choose a board[0-%u]: ", maxBoardIndex);
	fflush(stdout);
	int userInput = -1;
	do {
		char inputBuffer[512];
		userInput = readIntFromStdin(inputBuffer, sizeof(inputBuffer));
		if (userInput > maxBoardIndex) {
			printf("Invalid selection, retry[0-%u]: ", maxBoardIndex);
		}
	} while (userInput > maxBoardIndex);

	return userInput;
}

static const char * selectDllSingleList(const char **dlls)
{
	int i = 0;
	for(i = 0; strlen(dlls[i]) > 0; i++) {
		printf("%d: %s \n", i, dlls[i]);
	}

	if(i == 1) {
		printf("\n No DLLs available for this board. \n");
		return "";
	}

	printf("\nChoose a dll by entering a number between 0 and %d:", i-1);
	int userInput = -1;
	do {
		char inputBuffer[512];
		userInput = readIntFromStdin(inputBuffer, sizeof(inputBuffer));
		if (userInput > i-1) {
			printf("Invalid selection, retry[0-%u]: ", i-1);
		}
	} while (userInput >= i);

	return dlls[userInput];
}


static const char * selectDll(unsigned int boardType, const char **me3dlls, const char **me3xxldlls,
							  const char **me4dualdlls, const char **me4singledlls, const char **me4dualdllsX4, const char **me4GEVdllsX4)
{
	switch(boardType) {
			case PN_MICROENABLE3I:
				return selectDllSingleList(me3dlls);
			case PN_MICROENABLE3IXXL:
				return selectDllSingleList(me3xxldlls);
			case PN_MICROENABLE4AD1CL:
			case PN_MICROENABLE4VD1CL:
				return selectDllSingleList(me4dualdlls);
			case PN_MICROENABLE4AD4CL:
			case PN_MICROENABLE4VD4CL:
				return selectDllSingleList(me4dualdllsX4);
			case PN_MICROENABLE4AS1CL:
				return selectDllSingleList(me4singledlls);
      case PN_MICROENABLE4AQ4GE:
      case PN_MICROENABLE4VQ4GE:
        return selectDllSingleList(me4GEVdllsX4);
			default:
				return NULL;
	}
}

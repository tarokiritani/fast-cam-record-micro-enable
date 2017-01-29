//#include "stdafx.h"
#include "sisoIo.h"
#include "fgrab_struct.h"
#include "fgrab_prototyp.h"
#include "fgrab_define.h"
#include <string>
#include <sstream> 
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <map>
#include <algorithm>
#include <time.h>
/*#include "tiff.h"*/
#include "tiffio.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <windows.h>
#include <direct.h>

std::mutex lock1;
std::condition_variable cv;

using namespace std;

struct movieInfo {
	int width;
	int height;
	int nOfPicsToGrab;
} movie;

int acquireMovie(Fg_Struct* fg, const int camPort, const int nOfPicsToGrab, const int timeout, const int dispId, int &lastPicNum, int width, int height) {
	/*Image width of the acquisition window.*/
	if (Fg_setParameter(fg, FG_WIDTH, &width, camPort) < 0) {
		return FG_ERROR;
	}

	/*Image height of the acquisition window.*/
	if (Fg_setParameter(fg, FG_HEIGHT, &height, camPort) < 0) {
		return FG_ERROR;
	}

	if ((Fg_Acquire(fg, camPort, nOfPicsToGrab)) < 0) {
		return FG_ERROR;
	}
	int drawnPicNum = 1;
	while ((lastPicNum = Fg_getLastPicNumberBlocking(fg, drawnPicNum, camPort, timeout))< nOfPicsToGrab) { // i think the second argument is correct. used to me lastPicNum..
		if (lastPicNum < 0) {
			Fg_stopAcquire(fg, camPort);
			Fg_FreeMem(fg, camPort);
			Fg_FreeGrabber(fg);
			return FG_ERROR;
		}
		DrawBuffer(dispId, Fg_getImagePtr(fg, lastPicNum, camPort), lastPicNum, ""); // displays an image
		cv.notify_one();
		drawnPicNum = lastPicNum + 1;
	}
	CloseDisplay(dispId);
	Fg_stopAcquire(fg, camPort);
	Sleep(100);
	cv.notify_one(); // for safety.
	return 0;
}

int saveMovie1(const int &lastPicNum, movieInfo movie, unsigned char* ptrMem, string folderName, string subfolder) {
	std::cout << "saving data..." << endl;
	int savedFrameNum = 1;
	std::unique_lock<std::mutex> l(lock1);

	string subFolderName;
	subFolderName = folderName + subfolder;

	unsigned char* buffer = (unsigned char *)malloc(movie.width*movie.height * sizeof(unsigned char));
	while (savedFrameNum <= movie.nOfPicsToGrab) {
		cv.wait(l, [&savedFrameNum, &lastPicNum]() {return lastPicNum >= savedFrameNum; });

		for (unsigned int i = 0; i<movie.width; i++) {
			for (unsigned int j = 0; j<movie.height; j++) {
				buffer[j*movie.width + i] = *(ptrMem + (j*movie.width + i));
			}
		}
		folderName;
		_mkdir(subFolderName.c_str());
		TIFF *image;
		string fileName = subFolderName + "\\frame" + std::to_string(savedFrameNum) + ".tif";
		image = TIFFOpen(fileName.c_str(), "w");
		if (image == NULL) {
			return -1;
		}
		TIFFSetField(image, TIFFTAG_IMAGEWIDTH, movie.width);
		TIFFSetField(image, TIFFTAG_IMAGELENGTH, movie.height);
		TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
		TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
		TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
		TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
		TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(image, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
		TIFFSetField(image, TIFFTAG_PAGENUMBER, savedFrameNum, movie.nOfPicsToGrab);
		//TIFFWriteRawStrip(image,0,buffer,movie.width*movie.height); 

		int i;
		for (i = 0; i < movie.height; i++) {
			TIFFWriteScanline(image, &buffer[i*movie.width], i, 0);
		}
		TIFFWriteDirectory(image);
		TIFFClose(image);
		//cout << "writing frame num "<< savedFrameNum <<endl;

		ptrMem = ptrMem + movie.width*movie.height;
		savedFrameNum++;
	}
	free(buffer);

	return 0;
}

int main(int argc, char* argv[], char* envp[]) {

	Fg_Struct *fg = NULL;
	int boardNr = 0;
	int camPort = PORT_A;
	int nOfPicsToGrab = atoi(argv[1]);
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	int samplePerPixel = 1;
	int bytePerSample = 1;
	int timeout = atoi(argv[4]);
	int iterationNum = atoi(argv[5]);
	string folderName = argv[6];
	string subFolderName = argv[7];

	unsigned char *ptrMem = NULL;

	for (int k = 0; k<iterationNum; k++) {
		if ((fg = Fg_Init("FullAreaGray8.dll", boardNr)) == NULL) {
			return FG_ERROR;
		}
		int lastPicNum = 0;
		int dispId0 = CreateDisplay(8 * bytePerSample*samplePerPixel, width, height);
		SetBufferWidth(dispId0, width, height);
		long long totalBufferSize = (long long)width * (long long)height * (long long)samplePerPixel * (long long)bytePerSample * (long long)nOfPicsToGrab;
		cout << "total data size (byte): " << totalBufferSize << endl;

		ptrMem = (unsigned char*)Fg_AllocMem(fg, totalBufferSize, nOfPicsToGrab, camPort);
		if (ptrMem == NULL) {
			return FG_ERROR;
		}

		movie.width = width;
		movie.height = height;
		movie.nOfPicsToGrab = nOfPicsToGrab;
		string fileDest = subFolderName + to_string(k);
		thread t(saveMovie1, std::ref(lastPicNum), movie, ptrMem, folderName, fileDest);
		acquireMovie(fg, camPort, nOfPicsToGrab, timeout, dispId0, lastPicNum, width, height);
		t.join();
		Fg_FreeMem(fg, camPort);
		Fg_FreeGrabber(fg);
		cout << "Closing program... See you!" << endl;
		Sleep(1000);
	}

	return FG_OK;
}
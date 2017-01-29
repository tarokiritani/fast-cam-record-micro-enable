#include <stdio.h>

#include "board_and_dll_chooser.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

int main(int argc, char* argv[], char* envp[])
{
  Fg_Struct *fg           = NULL;
  int boardNr             = selectBoardDialog();
  int camPort             = PORT_A;
  frameindex_t nrOfPicturesToGrab  = 1000;
  frameindex_t nbBuffers           = 4;
  unsigned int width               = 512;
  unsigned int height     = 512;
  int samplePerPixel      = 1;
  size_t bytePerSample       = 1;

  const char *applet;
  switch (Fg_getBoardType(boardNr)) {
  case PN_MICROENABLE4AS1CL:
    applet = "SingleAreaGray16";
    break;
  case PN_MICROENABLE4AD1CL:
  case PN_MICROENABLE4AD4CL:
  case PN_MICROENABLE4VD1CL:
  case PN_MICROENABLE4VD4CL:
    applet = "DualAreaGray16";
    break;
  case PN_MICROENABLE4AQ4GE:
  case PN_MICROENABLE4VQ4GE:
    applet = "QuadAreaGray16";
    break;
  case PN_MICROENABLE3I:
    applet = "DualAreaGray";
    break;
  case PN_MICROENABLE3IXXL:
    applet = "DualAreaGray12XXL";
    break;
  default:
    applet = "DualAreaGray16";
    break;
  }

  if ((fg = Fg_Init(applet, boardNr)) == NULL) {
    fprintf(stderr, "error in Fg_Init: %s\n", Fg_getLastErrorDescription(NULL));
    return FG_ERROR;
  }

  int dispId0 = CreateDisplay(8*bytePerSample*samplePerPixel,width,height);
  SetBufferWidth(dispId0,width,height);

  size_t totalBufferSize = width * height * samplePerPixel * bytePerSample * nbBuffers;
  dma_mem *memHandle = Fg_AllocMemEx(fg, totalBufferSize, nbBuffers);
  if (memHandle == NULL) {
    fprintf(stderr, "error in Fg_AllocMemEx: %s\n", Fg_getLastErrorDescription(fg));
    CloseDisplay(dispId0);
    Fg_FreeGrabber(fg);
    return FG_ERROR;
  }

  /*Image width of the acquisition window.*/
  if (Fg_setParameter(fg,FG_WIDTH,&width,camPort) < 0 ) {
    fprintf(stderr, "Fg_setParameter(FG_WIDTH) failed: %s\n", Fg_getLastErrorDescription(fg));
    Fg_FreeMemEx(fg, memHandle);
    CloseDisplay(dispId0);
    Fg_FreeGrabber(fg);
    return FG_ERROR;
  }

  /*Image height of the acquisition window.*/
  if (Fg_setParameter(fg,FG_HEIGHT,&height,camPort) < 0 ) {
    fprintf(stderr, "Fg_setParameter(FG_HEIGHT) failed: %s\n", Fg_getLastErrorDescription(fg));
    Fg_FreeMemEx(fg, memHandle);
    CloseDisplay(dispId0);
    Fg_FreeGrabber(fg);
    return FG_ERROR;
  }


	int bitAlignment = FG_LEFT_ALIGNED;
	if (Fg_setParameter(fg,FG_BITALIGNMENT,&bitAlignment,camPort) < 0) {
		fprintf(stderr, "Fg_setParameter(FG_FG_BITALIGNMENTHEIGHT) failed: %s\n", Fg_getLastErrorDescription(fg));
		Fg_FreeMemEx(fg, memHandle);
		CloseDisplay(dispId0);
		Fg_FreeGrabber(fg);
		return FG_ERROR;
	}

  if ((Fg_AcquireEx(fg, camPort, nrOfPicturesToGrab, ACQ_STANDARD, memHandle)) < 0) {
    fprintf(stderr, "Fg_AcquireEx() failed: %s\n", Fg_getLastErrorDescription(fg));
    Fg_FreeMemEx(fg, memHandle);
    CloseDisplay(dispId0);
    Fg_FreeGrabber(fg);
    return FG_ERROR;
  }

  frameindex_t last_pic_nr = 0;
  frameindex_t cur_pic_nr;
  int timeout = 4;
  while ((cur_pic_nr = Fg_getLastPicNumberBlockingEx(fg, last_pic_nr + 1, camPort, timeout, memHandle)) < nrOfPicturesToGrab) {
    if (cur_pic_nr < 0) {
      fprintf(stderr, "Fg_getLastPicNumberBlockingEx(%li) failed: %s\n", last_pic_nr + 1, Fg_getLastErrorDescription(fg));
      Fg_stopAcquire(fg,camPort);
      Fg_FreeMemEx(fg, memHandle);
      CloseDisplay(dispId0);
      Fg_FreeGrabber(fg);
      return FG_ERROR;
    }
    last_pic_nr = cur_pic_nr;
    DrawBuffer(dispId0, Fg_getImagePtrEx(fg, last_pic_nr, camPort, memHandle), static_cast<int>(last_pic_nr), "");
  }

  CloseDisplay(dispId0);
  Fg_stopAcquire(fg,camPort);
  Fg_FreeMemEx(fg, memHandle);
  Fg_FreeGrabber(fg);

  return FG_OK;
}

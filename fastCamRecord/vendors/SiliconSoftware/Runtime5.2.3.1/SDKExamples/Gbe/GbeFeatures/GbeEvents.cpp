// ============================================================================
// GbeEvents.cpp - Example for Gige Vision Events
// ============================================================================

// ============================================================================
// includes
// ============================================================================

#include "GbeFeaturesCommon.h"

// ============================================================================
// functions
// ============================================================================

static int CameraEventCallback(CameraHandle *cameraHandle, const GbeEventInfo *eventInfo, const void*)
{
	try {
		switch(eventInfo->type)
		{
		case GBE_EVENT_CAMERA_CONNECTION_LOST:
			LOG("[EVENT ] Camera connection lost detected!");
			break;
		case GBE_EVENT_GEVEVENT:
			LOG("[EVENT ] GEV Event:");
			LOG("[EVENT ] ... ReqId: " << std::hex << std::showbase << eventInfo->data.gevevent.reqId << std::dec);
			LOG("[EVENT ] ... Index: " << eventInfo->data.gevevent.index);
			LOG("[EVENT ] ... EventId: " << std::hex << std::showbase << eventInfo->data.gevevent.eventId << std::dec);
			LOG("[EVENT ] ... StreamChannelIndex: " << eventInfo->data.gevevent.streamChannelIndex);
			LOG("[EVENT ] ... BlockId: " << std::hex << std::showbase << eventInfo->data.gevevent.blockId << std::dec);
			LOG("[EVENT ] ... Timestamp: " << eventInfo->data.gevevent.timestamp);
			LOG("[EVENT ] ... DataLength: " << eventInfo->data.gevevent.dataLen);
			break;
		}
	} catch(std::runtime_error& e) {
		LOG("[ERROR ] Caught runtime error: " << e.what());
	}
	
	return 0;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);

	LOG("============================================================================");
	LOG("| GbeEvents.cpp - Example for Gige Vision Events                           |");
	LOG("============================================================================");

	Fg_Struct* fg = 0;
	dma_mem* memory = 0;
	int displayId = -1;
	unsigned int portId = -1;
	
	try {

		// ####################################################################
		// # PART 1: BOARD/PORT/CAMERA DETECTION/SETUP                        #
		// ####################################################################

		setupGbeEnvironmentDetection();

		// ####################################################################
		// # PART 2: CAMERA SELECTION                                         #
		// ####################################################################

		// === Selecting camera ===

		bool showWaitMsg = true;
		
		for (int i = 0; i < 60; ++i)
		{
			if (showWaitMsg)
				LOG("[STATUS] Selecting camera ...");

			GigeBoardMap::iterator bit;
			GigePortMap::iterator pit;
			GigeCameraMap::iterator cit;

			bool cameraFound = false;

			for (bit = gigeBoards.begin(); bit != gigeBoards.end(); ++bit) {
				GigeBoard& board = bit->second;
				for (pit = board.ports.begin(); pit != board.ports.end(); ++pit) {
					GigePort& port = pit->second;
					if (port.cameras.begin() != port.cameras.end()) {
						cit = port.cameras.begin();
						cameraFound = true;
						break;
					}
				}
				if (cameraFound)
					break;
			}

			if (!cameraFound) {
				if (showWaitMsg) {
					LOG("... No camera found, waiting!");
					showWaitMsg = false;
				}
				msleep(1000);
				continue;
			}

			showWaitMsg = true;

			GigeBoard& board = bit->second;
			GigeCamera& camera = cit->second;

			unsigned int boardId = bit->first;
			portId = pit->first;

			LOG("... Selected camera: " << camera.manufacturer << " " << camera.model << " (IP: " << ipv4String(camera.ipv4) << ", MAC: " << macString(camera.mac) << ") on port group " << boardId << "." << portId);
		
			// === Connect to camera ===

			LOG("[STATUS] Connecting to camera ...");

			int errcode = Gbe_connectCamera(camera.handle);
			throwGbeError(errcode, "Gbe_connectCamera");
			LOG("... Successfully connected to camera");

			// === Registering camera callback ===

			LOG("[STATUS] Registering callback for camera events...");

			errcode = Gbe_registerCameraEventCallback(camera.handle, CameraEventCallback, 0);
			throwGbeError(errcode, "Gbe_registerCameraEventCallback()");
			LOG("... Successfully registered event callback for camera");

			// === Loading applet ===

			LOG("[STATUS] Loading applet 'QuadAreaGray8' ...");

			fg = Fg_Init("QuadAreaGray8", board.id);
			
			if (fg == 0)
				throwFgError("Fg_Init");

			LOG("... Successfully loaded applet 'QuadAreaGray8'");

			// === Setting image size to 640x480

			LOG("[STATUS] Setting image dimension to 640x480, format FG_GRAY ...");

			int width = 640;
			int height = 480;
			int format = FG_GRAY;

			errcode = Fg_setParameter(fg, FG_WIDTH, &width, portId);
			throwFgError(errcode, "Fg_setParameter(FG_WIDTH)");
			LOG("... Successfully set applet width to 640");

			errcode = Fg_setParameter(fg, FG_HEIGHT, &height, portId);
			throwFgError(errcode, "Fg_setParameter(FG_WIDTH)");
			LOG("... Successfully set applet width to 640");

			errcode = Fg_setParameter(fg, FG_FORMAT, &format, portId);
			throwFgError(errcode, "Fg_setParameter(FG_FORMAT)");
			LOG("... Successfully set applet format to FG_GRAY");

			errcode = Gbe_setIntegerValue(camera.handle, "Width", width);
			throwGbeError(errcode, "Gbe_setIntegerValue(\"Width\")");
			LOG("... Successfully set camera width to 640");

			errcode = Gbe_setIntegerValue(camera.handle, "Height", height);
			throwGbeError(errcode, "Gbe_setIntegerValue(\"Height\")");
			LOG("... Successfully set camera width to 480");

			// === Allocating framebuffer memory

			LOG("[STATUS] Allocating framebuffer memory ...");

			size_t totalBufSize = width * height * 8 * 1;

			memory = Fg_AllocMemEx(fg, totalBufSize, 8);

			if (memory == 0)
				throwFgError("Fg_AllocMemEx");

			LOG("... Successfully allocated framebuffer memory");

			// === Creating display window

			LOG("[STATUS] Creating display window ...");

			displayId = ::CreateDisplay(8, width, height);
			SetBufferWidth(displayId, width, height);

			LOG("... Successfully created display window");

			// === Start acquisition

			LOG("[STATUS] Starting acquisition ...");

			errcode = Fg_AcquireEx(fg, portId, GRAB_INFINITE, ACQ_STANDARD, memory);
			throwFgError(errcode, "Fg_AcquireEx()");
			LOG("... Successfully started applet acquisition");

			errcode = Gbe_startAcquisition(camera.handle);
			throwGbeError(errcode, "Gbe_startAcquisition()");
			LOG("... Successfully started camera acquisition");
		
			// === Image Grab Loop

			LOG("[STATUS] Grabbing ...");

			frameindex_t lastPicNr = 0;
			for (int i = 0; i < 10000; ++i) {
				lastPicNr = Fg_getLastPicNumberBlockingEx(fg, lastPicNr + 1, portId, 10, memory);
				if(lastPicNr < 0)
					throwFgError(fg, "Fg_getLastPicNumberBlockingEx()");
				::DrawBuffer(displayId, Fg_getImagePtrEx(fg, lastPicNr, portId, memory), (int)lastPicNr, "");
			}
		}

	} catch(std::runtime_error& e) {
		LOG("[ERROR ] Caught runtime error: " << e.what());
	}

	// === Freeing GigE boards ===

	LOG("[STATUS] Freeing GigE boards ...");

	for (GigeBoardMap::iterator it = gigeBoards.begin(); it != gigeBoards.end(); ++it) {
		GigeBoard& board = it->second;
		Gbe_freeBoard(board.handle);
		LOG("... Freed GigE board " << it->first);
	}

	// === Freeing applet resources ===

	LOG("[STATUS] Freeing applet resources ...");

	if (memory != 0) {
		Fg_stopAcquireEx(fg, portId, memory, 0);
		Fg_FreeMemEx(fg, memory);
	}

	if (displayId != -1)
		CloseDisplay(displayId);

	if (fg != 0)
		Fg_FreeGrabber(fg);

	LOG("... Freed applet resources");

}

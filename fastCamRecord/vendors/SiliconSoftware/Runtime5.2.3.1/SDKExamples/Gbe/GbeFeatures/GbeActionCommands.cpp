// ============================================================================
// GbeActionCommands.cpp - Example for Gige Vision Action Commands
// ============================================================================

// ============================================================================
// includes
// ============================================================================

#include "GbeFeaturesCommon.h"

// ============================================================================
// global variables
// ============================================================================

unsigned int gActionDeviceKey = 42;
unsigned int gActionGroupKey = 0x1;
unsigned int gActionGroupMask = 0xff;

struct BoardHandle* gActionCommandBoardHandle = 0;
int gActionCommandPort = -1;

// ============================================================================
// functions
// ============================================================================

#if defined(WIN32)

#include <windows.h>

bool gActTerminationFlag = false;
const uint32_t gcActionCommandThreadSleepTime = 1000;

DWORD WINAPI actionCommandThreadFunction(LPVOID)
{
	try {
		while(!gActTerminationFlag) {
			Gbe_sendActionCommand(gActionCommandBoardHandle, gActionCommandPort, 0xffffffff, gActionDeviceKey, gActionGroupKey, gActionGroupMask);
			Sleep(static_cast<DWORD>(gcActionCommandThreadSleepTime));
		}
	} catch(...) {}

	return 0;
}

#endif

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);

	LOG("============================================================================");
	LOG("| GbeActionCommands.cpp - Example for Gige Vision Action Commands          |");
	LOG("============================================================================");

	Fg_Struct* fg = 0;
	dma_mem* memory = 0;
	int displayId = -1;
	unsigned int portId = -1;

#if defined(WIN32)
	HANDLE mThread = INVALID_HANDLE_VALUE;
#endif

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

			// === Setting up action commands in camera

			LOG("[STATUS] Setting up action commands in camera ...");

			errcode = Gbe_setCameraPropertyWithType(camera.handle, "actionDeviceKey", &gActionDeviceKey, GBE_PROPERTY_TYPE_UINT);
			throwGbeError(errcode, "Gbe_setCameraPropertyWithType(\"actionDeviceKey\")");
			LOG("... Successfully set action device key");

			errcode = Gbe_setCameraPropertyWithType(camera.handle, "actionGroupKey0", &gActionGroupKey, GBE_PROPERTY_TYPE_UINT);
			throwGbeError(errcode, "Gbe_setCameraPropertyWithType(\"actionGroupKey0\")");
			LOG("... Successfully set action group key");

			errcode = Gbe_setCameraPropertyWithType(camera.handle, "actionGroupMask0", &gActionGroupMask, GBE_PROPERTY_TYPE_UINT);
			throwGbeError(errcode, "Gbe_setCameraPropertyWithType(\"actionGroupMask0\")");
			LOG("... Successfully set action group mask");

			errcode = Gbe_setEnumerationValue(camera.handle, "TriggerMode", "On");
			throwGbeError(errcode, "Gbe_setEnumerationValue(\"TriggerMode\")");
			LOG("... Successfully enabled camera trigger mode");

			errcode = Gbe_setEnumerationValue(camera.handle, "TriggerSource", "Action1");
			throwGbeError(errcode, "Gbe_setEnumerationValue(\"TriggerSource\")");
			LOG("... Successfully set camera trigger source to \"Action1\"");

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
		
			// === Start action command thread

			LOG("[STATUS] Starting action command thread ...");

#if !defined(WIN32)
			LOG("[WARN  ] No action command thread implemention on *nix!");
#else
			gActionCommandBoardHandle = board.handle;
			gActionCommandPort = portId;

			mThread = CreateThread(NULL, 0, actionCommandThreadFunction, 0, 0, 0);
#endif

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

	// === Stopping action command thread

#if defined(WIN32)
	if (mThread != INVALID_HANDLE_VALUE)
	{
		LOG("[STATUS] Stopping action command thread ...");

		LOG("... Waiting for action command thread");

		gActTerminationFlag = true;

		WaitForSingleObject(mThread, INFINITE);

		LOG("... Joined action command thread");
	}
#endif

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

// ============================================================================
// GbeFeaturesCommon - Some common stuff for Gbe features examples
// ============================================================================

// ============================================================================
// includes
// ============================================================================

#include "gbe.h"
#include "gbe_error.h"
#include "sisoboards.h"
#include "fgrab_prototyp.h"

#include <stdexcept>

#include <iostream>
#include <sstream>

#include <map>

// ============================================================================
// helper macros
// ============================================================================

#if !defined(UNUSED)
	#define UNUSED(x) do { (void)(x); } while (0)
#endif

#if !defined(MULTI_LINE_MACRO_BEGIN)
	#define MULTI_LINE_MACRO_BEGIN() do {
#endif

#if !defined(MULTI_LINE_MACRO_END)
	#if defined(_WIN32)
		#define MULTI_LINE_MACRO_END() \
			__pragma(warning(push)) \
			__pragma(warning(disable:4127)) \
			} while(0) \
			__pragma(warning(pop))
	#else
		#define MULTI_LINE_MACRO_END() } while(0)
	#endif
#endif

// ============================================================================
// data structures for environment discovery
// ============================================================================

struct GigeCamera {
	struct CameraHandle* handle;
	std::string manufacturer;
	std::string model;
	uint32_t ipv4;
	uint64_t mac;
};

typedef std::map<unsigned int, GigeCamera> GigeCameraMap;

struct GigePort {
	uint32_t ipv4;
	GigeCameraMap cameras;
};

typedef std::map<unsigned int, GigePort> GigePortMap;

struct GigeBoard {
	const unsigned int type;
	const unsigned int id;
	struct BoardHandle* handle;
	GigePortMap ports;
	GigeBoard() : type(0), id(0) {}
	GigeBoard(unsigned int _type, unsigned int _id) : type(_type), id(_id), handle(0) {}
};

typedef std::map<unsigned int, GigeBoard> GigeBoardMap;

// ============================================================================
// global variables
// ============================================================================

GigeBoardMap gigeBoards;
bool gEventDisplayChanges = false;

// ============================================================================
// helper functions
// ============================================================================

#define LOG(__msg) \
	MULTI_LINE_MACRO_BEGIN() \
	std::cout << __msg << std::endl; \
	MULTI_LINE_MACRO_END()

void throwGbeError(int errcode, const char* func) {
	if (errcode != GBE_OK) {
		std::ostringstream oss;
		oss << func << " failed: " << Gbe_getErrorDescription(errcode);
		throw std::runtime_error(oss.str());
	}
}

void throwFgError(const char* func) {
	std::ostringstream oss;
	oss << func << " failed: " << Fg_getLastErrorDescription(0);
	throw std::runtime_error(oss.str());
}

void throwFgError(Fg_Struct* fg, const char* func) {
	std::ostringstream oss;
	oss << func << " failed: " << Fg_getLastErrorDescription(fg);
	throw std::runtime_error(oss.str());
}

void throwFgError(int errcode, const char* func) {
	if (errcode != FG_OK) {
		std::ostringstream oss;
		oss << func << " failed: " << Fg_getErrorDescription(0, errcode);
		throw std::runtime_error(oss.str());
	}
}

void throwFgError(Fg_Struct* fg, int errcode, const char* func) {
	if (errcode != FG_OK) {
		std::ostringstream oss;
		oss << func << " failed: " << Fg_getErrorDescription(fg, errcode);
		throw std::runtime_error(oss.str());
	}
}

std::string ipv4String(uint32_t ipv4)
{
	std::ostringstream oss;
	oss << ((ipv4 >> 24) & 0xff) << "." << ((ipv4 >> 16) & 0xff) << "." << ((ipv4 >> 8) & 0xff) << "." << (ipv4 & 0xff);
	return oss.str();
}

std::string macString(uint64_t mac)
{
	std::ostringstream oss;
	oss << std::hex << (mac & 0xff) << ":" << ((mac >> 8) & 0xff) << ":" << ((mac >> 16) & 0xff) << ":" << ((mac >> 24) & 0xff) << ":" << ((mac >> 32) & 0xff) << ":" << ((mac >> 40) & 0xff);
	return oss.str();
}

static const char* getBoardName(unsigned int boardType)
{
	switch (boardType) {
		case PN_MICROENABLE3I:
			return "microEnable III";
		case PN_MICROENABLE3IXXL:
			return "microEnable III XXL";
		case PN_MICROENABLE4AS1CL:
			return "microEnable IV AS1-CL";
		case PN_MICROENABLE4AD1CL:
			return "microEnable IV AD1-CL";
		case PN_MICROENABLE4VD1CL:
			return "microEnable IV VD1-CL";
		case PN_MICROENABLE4AD4CL:
			return "microEnable IV AD4-CL";
		case PN_MICROENABLE4VD4CL:
			return "microEnable IV VD4-CL";
		case PN_MICROENABLE4AQ4GE:
			return "microEnable IV AQ4-GE";
		case PN_MICROENABLE4VQ4GE:
			return "microEnable IV VQ4-GE";
		default:
			return "Unknown / Unsupported Board";
	}
}

void msleep(int milliseconds)
{
	if (milliseconds == 0)
		return;

#if defined(WIN32)
	Sleep(milliseconds);
#else
	::usleep(milliseconds * 1000);
#endif
}

// ============================================================================
// functions
// ============================================================================

void getCurrentCameras(BoardHandle* boardHandle, int port, GigeCameraMap& cameraMap)
{
	cameraMap.clear();
	int errcode = Gbe_scanNetwork(boardHandle, -1, -1);
	throwGbeError(errcode, "Gbe_scanNetwork()");
	int camCount = Gbe_getCameraCount(boardHandle, port);
	if (camCount >= 0) {
		for (int camIndex = 0; camIndex < camCount; ++camIndex) {
			struct CameraHandle* cameraHandle = 0;
			errcode = Gbe_getCameraByIndex(boardHandle, port, camIndex, &cameraHandle);
			throwGbeError(errcode, "Gbe_getCameraByIndex()");
			struct CameraInfo* cameraInfo = Gbe_getCameraInfo(cameraHandle);
			if (cameraInfo != 0) {
				cameraMap.insert(std::make_pair(camIndex, GigeCamera()));
				GigeCamera& camera = cameraMap[camIndex];
				camera.handle = cameraHandle;
				camera.manufacturer = cameraInfo->manufactor_name;
				camera.model = cameraInfo->model_name;
				camera.ipv4 = cameraInfo->ipv4;
				camera.mac = (uint64_t) cameraInfo->mac[0] | ((uint64_t) cameraInfo->mac[1] << 8) | ((uint64_t) cameraInfo->mac[2] << 16) | ((uint64_t) cameraInfo->mac[3] << 24) | ((uint64_t) cameraInfo->mac[4] << 32) | ((uint64_t) cameraInfo->mac[5] << 40);
			}
		}
	}
}

bool cameraMapsEqual(const GigeCameraMap& cameraMapA, const GigeCameraMap& cameraMapB)
{
	if (cameraMapA.size() != cameraMapB.size())
		return false;

	bool found = false;
	
	for (GigeCameraMap::const_iterator itA = cameraMapA.begin(); itA !=  cameraMapA.end(); ++itA) {
		found = false;
		for (GigeCameraMap::const_iterator itB = cameraMapB.begin(); itB !=  cameraMapB.end(); ++itB) {
			if (itA->second.mac == itB->second.mac && itA->second.ipv4 == itB->second.ipv4) {
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}

	return found;
}

void printCurrentScanResults(unsigned int boardId, std::string logPrefix = "")
{
	GigeBoard& board = gigeBoards[boardId];

	for (GigePortMap::iterator pit = board.ports.begin(); pit != board.ports.end(); ++pit) {
		unsigned int portId = pit->first;
		GigePort& gigePort = pit->second;
		LOG(logPrefix << "... Cameras found on port group " << boardId << "." << portId << ": " << gigePort.cameras.size());
		for (GigeCameraMap::iterator cit = gigePort.cameras.begin(); cit != gigePort.cameras.end(); ++cit) {
			unsigned int cameraIndex = cit->first;
			GigeCamera& gigeCamera = cit->second;
			LOG(logPrefix << "... ... [" << cameraIndex << "] " << gigeCamera.manufacturer << " " << gigeCamera.model << " (IP: " << ipv4String(gigeCamera.ipv4) << ", MAC: " << macString(gigeCamera.mac) << ")");
		}
	}
}

static int BoardEventCallback(BoardHandle *boardHandle, const GbeEventInfo *eventInfo, const void* userData)
{
	const unsigned int boardId = *reinterpret_cast<const unsigned int*>(userData);

	try {
		switch(eventInfo->type)
		{
		case GBE_EVENT_DISCOVERY_CHANGE:
			{
				unsigned int portId = eventInfo->data.discovery.port;
				GigeCameraMap currentCameraMap;
				getCurrentCameras(boardHandle, eventInfo->data.discovery.port, currentCameraMap);
				if (!cameraMapsEqual(currentCameraMap, gigeBoards[boardId].ports[portId].cameras)) {
					gigeBoards[boardId].ports[portId].cameras = currentCameraMap;
					if (gEventDisplayChanges) {
						LOG("[EVENT ] Detected camera change on GigE board " << boardId << ", port group " << portId << " ... ");
						printCurrentScanResults(boardId, "[EVENT ] ");
					}
				}
			}
			break;
		}
	} catch(std::runtime_error& e) {
		LOG("[ERROR ] Caught runtime error: " << e.what());
	}
	
	return 0;
}

void setupGbeEnvironmentDetection()
{
	// === Detecting GigE boards ===

	LOG("[STATUS] Detecting GigE boards ...");

	const unsigned int cSystemInfoStringSize = 256;
	char nrOfBoardsAsString[cSystemInfoStringSize];

	unsigned int nrOfBoardsAsStringSize = cSystemInfoStringSize;

	int errcode = Fg_getSystemInformation(0, INFO_NR_OF_BOARDS, PROP_ID_VALUE, 0, nrOfBoardsAsString, &nrOfBoardsAsStringSize);

	throwFgError(errcode, "Fg_SystemInformation()");

	unsigned int nrOfBoards = 0;

	std::istringstream iss(nrOfBoardsAsString);

	iss >> nrOfBoards;

	if (iss.fail()) {
		std::ostringstream oss;
		oss << "Internal Error: Cannot convert string '" << nrOfBoardsAsString << "' to unsigned integer (number of boards)";
		throw std::runtime_error(oss.str());
	}

	LOG("... Number of boards: " << nrOfBoards);

	unsigned int boardCount = 0;
	for (unsigned int i = 0; boardCount != nrOfBoards; ++i) {
		int boardType = Fg_getBoardType(i);
		if (boardType < 0)
			continue;
		switch (boardType) {
			case PN_MICROENABLE4AQ4GE:
			case PN_MICROENABLE4VQ4GE:
				gigeBoards.insert(std::make_pair(i, GigeBoard(boardType, i)));
		}
		LOG("... ... Board " << i << ": " << getBoardName(boardType) << " (" << std::hex << boardType << std::dec << ")");
		++boardCount;
	}

	LOG("... Number of GigE boards: " << gigeBoards.size());

	for (GigeBoardMap::const_iterator cit = gigeBoards.begin(); cit != gigeBoards.end(); ++cit)
		LOG("... ... GigE board " << cit->first << ": " << getBoardName(cit->second.type) << " (" << std::hex << cit->second.type << std::dec << ")");

	// === Initializing GigE boards ===

	LOG("[STATUS] Initializing GigE boards ...");

	for (GigeBoardMap::iterator it = gigeBoards.begin(); it != gigeBoards.end(); ++it) {
		GigeBoard& board = it->second;
		errcode = Gbe_initBoard(it->first, 0, &board.handle);
		throwGbeError(errcode, "Gbe_initBoard()");
		LOG("... Successfully initialized GigE board " << it->first);
	}

	// === Gathering GigE port informations

	LOG("[STATUS] Gathering GigE port groups informations ...");

	for (GigeBoardMap::iterator it = gigeBoards.begin(); it != gigeBoards.end(); ++it) {
		unsigned int boardId = it->first;
		GigeBoard& board = it->second;
		for (int portId = 0; portId < 4; ++portId) {
			std::ostringstream ossIp;
			ossIp << "ip" << portId;
			uint32_t currentIp = 0;
			errcode = Gbe_getBoardPropertyWithType(board.handle, ossIp.str().c_str(), &currentIp, GBE_PROPERTY_TYPE_UINT32);
			if (errcode < 0 || currentIp == 0)
				continue;
			board.ports.insert(std::make_pair(portId, GigePort()));
			GigePort& gigePort = board.ports[portId];
			gigePort.ipv4 = currentIp;
			LOG("... Found port group " << boardId << "." << portId << " (IP: " << ipv4String(gigePort.ipv4) << ")");
		}
	}

	// === Registering callbacks for board events ===

	// === Scanning for GVCP cameras ===

	LOG("[STATUS] Registering callbacks for board events and scanning for GVCP cameras...");

	for (GigeBoardMap::iterator it = gigeBoards.begin(); it != gigeBoards.end(); ++it) {
		unsigned int boardId = it->first;
		GigeBoard& board = it->second;
		errcode = Gbe_registerBoardEventCallback(board.handle, BoardEventCallback, &board.id);
		throwGbeError(errcode, "Gbe_registerBoardEventCallback()");
		LOG("... Successfully registered event callback for GigE board " << boardId);
		for (int portId = 0; portId < 4; ++portId) {
			getCurrentCameras(board.handle, portId, board.ports[portId].cameras);
		}
		errcode = Gbe_scanNetwork(board.handle, -1, 1000);
		throwGbeError(errcode, "Gbe_scanNetwork()");
		LOG("... Successfully scanned GigE board " << boardId);
	}

	LOG("[STATUS] Initial scan results...");

	for (GigeBoardMap::iterator it = gigeBoards.begin(); it != gigeBoards.end(); ++it) {
		unsigned int boardId = it->first;
		printCurrentScanResults(boardId);
	}

	gEventDisplayChanges = true; // allow board event to print changes
}

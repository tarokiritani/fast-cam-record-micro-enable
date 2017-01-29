// ============================================================================
// GbeInternalGenICam.cpp - Example for using internal GenICam object
// ============================================================================

// ============================================================================
// includes
// ============================================================================

#include "GbeFeaturesCommon.h"

#define _STDINT
#include <GenApi/GenApi.h>

// ============================================================================
// functions
// ============================================================================

using namespace GenApi;

void traverseCategory(CCategoryPtr category, int depth)
{
	try {
		if (!category.IsValid())
			return;

		for (int i = 0; i < depth; ++i)
			std::cout << "... ";

		std::cout << category->GetNode()->GetName() << " (" << GetInterfaceName(category) << ")" << std::endl;

		FeatureList_t pFeatureList;

		category->GetFeatures(pFeatureList);

		for (FeatureList_t::const_iterator it = pFeatureList.begin(); it != pFeatureList.end(); ++it) {
			CNodePtr node = (*it)->GetNode();
			if (GetInterfaceName(node) == "ICategory") {
				traverseCategory(static_cast<CCategoryPtr>(node), depth + 1);
			} else {
				for (int i = 0; i <= depth; ++i)
					std::cout << "... ";
				std::cout << node->GetName() << " (" << GetInterfaceName(node) << ")" << std::endl;
			}
		}
	} catch(GenICam::GenericException& e) {
		LOG("[ERROR ] GenICam Exception: " << e.what());
	}
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);

	LOG("============================================================================");
	LOG("| GbeInternalGenICam.cpp - Example for using internal GenICam object       |");
	LOG("============================================================================");

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
			unsigned int portId = pit->first;

			LOG("... Selected camera: " << camera.manufacturer << " " << camera.model << " (IP: " << ipv4String(camera.ipv4) << ", MAC: " << macString(camera.mac) << ") on port group " << boardId << "." << portId);
		
			// === Connect to camera ===

			LOG("[STATUS] Connecting to camera ...");

			int errcode = Gbe_connectCamera(camera.handle);
			throwGbeError(errcode, "Gbe_connectCamera");
			LOG("... Successfully connected to camera");

			// === Getting internal GenICam object ===

			LOG("[STATUS] Getting internal GenICam object ...");

			GenApi::CNodeMapRef* nodemap = Gbe_getCNodeMapRef(camera.handle);

			LOG("... Successfully got internal GenICam object");

			// === Traversing internal GenICam object ===

			LOG("[STATUS] Traversing internal GenICam object ...");

			traverseCategory(nodemap->_GetNode("Root"), 1);

			break;
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
}

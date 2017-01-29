// ============================================================================
// GbeForceIp.cpp - Example for Gige Vision ForceIP command
// ============================================================================

// ============================================================================
// includes
// ============================================================================

#include "GbeFeaturesCommon.h"

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);

	LOG("============================================================================");
	LOG("| GbeForceIp.cpp - Example for Gige Vision ForceIP command                 |");
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
		
			// ==========================
			// === FORCEIP test command
			// ==========================

			LOG("[STATUS] Testing FORCEIP command ...");

			uint8_t* mac = reinterpret_cast<uint8_t*>(&camera.mac);

			int errcode = Gbe_sendForceIpCommand(board.handle, portId, mac, 0x42424242, 0, 0);
			throwGbeError(errcode, "Gbe_sendForceIpCommand");
			LOG("... Successfully sent FORCEIP command");

			msleep(1000);
		}

		// === Freeing GigE boards ===

		LOG("[STATUS] Freeing GigE boards ...");

		for (GigeBoardMap::iterator it = gigeBoards.begin(); it != gigeBoards.end(); ++it) {
			GigeBoard& board = it->second;
			Gbe_freeBoard(board.handle);
			LOG("... Freed GigE board " << it->first);
		}

	} catch(std::runtime_error& e) {
		LOG("[ERROR ] Caught runtime error: " << e.what());
	}
}

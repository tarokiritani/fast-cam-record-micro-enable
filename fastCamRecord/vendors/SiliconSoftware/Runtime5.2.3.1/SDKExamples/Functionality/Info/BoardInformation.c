#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <sisoboards.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int index;
	Fg_Struct *fg = NULL;

	for (index = 0; ((index < 10) || (fg != NULL)); index++) {
		int board_type = Fg_getBoardType(index);
		const char *applet = NULL;
		unsigned int board_serial;
		char board_name[255];
		unsigned int board_nameLen = 255;
		fg = NULL;

		if (board_type == FG_ERROR)
			continue;

		/* Select one applet to load. The applet itself doesn't
		 * matter. We chose a acquisition applet here but any HAP
		 * file created with VisualApplets would also work. */
		switch (board_type) {
		case PN_MICROENABLE3I:
			applet = "DualAreaGray";
			break;
		case PN_MICROENABLE3IXXL:
			applet = "DualAreaGray12XXL";
			break;
		case PN_MICROENABLE4AD1CL:
		case PN_MICROENABLE4AD4CL:
		case PN_MICROENABLE4VD1CL:
		case PN_MICROENABLE4VD4CL:
			applet = "DualAreaGray16";
			break;
		case PN_MICROENABLE4AS1CL:
			applet = "SingleAreaGray16";
			break;
		case PN_MICROENABLE4VQ4GE:
		case PN_MICROENABLE4AQ4GE:
			applet = "QuadAreaGray16";
			break;
		default:
			printf("% 2i: type %03X\n", index, board_type);
			continue;
		}

		fg = Fg_InitEx(applet, index, 0);
		if (fg == NULL) {
			printf("% 2i: type %03X\n", index, board_type);
			continue;
		}


		Fg_getSystemInformation(fg, INFO_BOARDNAME, PROP_ID_VALUE, 0, board_name, &board_nameLen);

		board_serial = Fg_getSerialNumber(fg);

		printf("% 2i: type %03X name \"%s\" serial number %08x\n", index, board_type, board_name, board_serial);
		Fg_FreeGrabber(fg);
	}

	return 0;
}

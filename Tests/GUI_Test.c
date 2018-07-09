
#include "GUI.h"

/**********************************
* œ‘ æ◊÷∑˚¥Æ≤‚ ‘
*
***********************************/
void GUI_DispStringTest(void)
{
	/*** œ‘ æ ˝æ›≤‚ ‘ ***/
	GUI_DispString("\nGUI_DispDec(): ");
	GUI_DispDec((int)147483647, 9);
	GUI_DispString("\nGUI_DispDec() -: ");
	GUI_DispDec((int)-47483647, 9);
	GUI_DispString("\nGUI_DispDecShift() 2dot: \n");
	GUI_DispDecShift(1234567, 9, 2);
	GUI_DispString("\nGUI_DispSDec(): ");
	GUI_DispSDec((int)47483647, 9);
	GUI_DispString("\nGUI_DispFloat(): ");
	GUI_DispFloat((float)1.234, 9);
	GUI_DispString("\nGUI_DispBin(): ");
	GUI_DispBin((unsigned int)2, 8);
	GUI_DispString("\nGUI_DispHex(): ");
	GUI_DispHex((unsigned int)0x68, 8);
}


	

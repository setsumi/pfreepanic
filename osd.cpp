//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "osd.h"

//---------------------------------------------------------------------------
// same constant as in dx9osd.dll
#define OSD_MSG_WNDCLASS L"DX9OSDMessageWindow"

//---------------------------------------------------------------------------
tOSD::tOSD()
{
	hwGame = NULL;
}
tOSD::~tOSD()
{
}

//---------------------------------------------------------------------------
bool tOSD::FindGame()
{
	hwGame = FindWindow(OSD_MSG_WNDCLASS, NULL);
	return (hwGame != NULL);
}

//---------------------------------------------------------------------------
bool tOSD::SendMessage(UnicodeString msg, unsigned int interval)
{
	bool found = FindGame();
	if (found) {
		::SendMessage(hwGame, WM_SETTEXT, 0, (LPARAM)msg.w_str());
		::SendNotifyMessage(hwGame, WM_APP+1, interval, 0);
	}
	return found;
}

//---------------------------------------------------------------------------


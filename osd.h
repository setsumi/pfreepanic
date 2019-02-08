//---------------------------------------------------------------------------

#ifndef osdH
#define osdH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//#include <Controls.hpp>
//#include <StdCtrls.hpp>
//#include <Forms.hpp>
//#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class tOSD
{
private:
	HWND hwGame;

public:
	tOSD();
	~tOSD();

	bool FindGame();
	bool SendMessage(UnicodeString msg, unsigned int interval);
};

//---------------------------------------------------------------------------
#endif // osdH

//---------------------------------------------------------------------------
#ifndef volumeH
#define volumeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//#include <Controls.hpp>
//#include <StdCtrls.hpp>
//#include <Forms.hpp>
//#include <ExtCtrls.hpp>


//---------------------------------------------------------------------------
double ChangeVolume(double nVolume, BOOL bRelative);
BOOL ChangeMute(BOOL bMute, BOOL bToggle, BOOL bOnlyPeek = FALSE);


//---------------------------------------------------------------------------
#endif // volumeH


//---------------------------------------------------------------------------
#ifndef UtilsH
#define UtilsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#define tBETWEEN(x,y,z)	(((y)>=(x))&&((y)<=(z)))
//---------------------------------------------------------------------------
UnicodeString GetModuleName();
UnicodeString GetModuleDir();

AnsiString BinToHexA(void *data, int size);
UnicodeString BinToHexW(void *data, int size);
AnsiString StrToHex(UnicodeString text);
UnicodeString HexToStr(AnsiString hex);

//---------------------------------------------------------------------------
#endif // UtilsH

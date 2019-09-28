//---------------------------------------------------------------------------

//#include <vcl.h>
#pragma hdrstop

#include "Utils.h"

//---------------------------------------------------------------------------
UnicodeString GetModuleName()
{
	return GetModuleName(unsigned(HInstance));
}
//---------------------------------------------------------------------------
UnicodeString GetModuleDir()
{
	UnicodeString name(GetModuleName(unsigned(HInstance)));
	return name.SubString(1, name.LastDelimiter(L"\\"));
}

//---------------------------------------------------------------------------
AnsiString BinToHexA(void *data, int size)
{
	AnsiString str1, ret;
	for (int i = 0; i < size; i++) {
		str1.sprintf("%02X", *((BYTE*)data + i));
		ret += str1;
	}
	return ret;
}
//---------------------------------------------------------------------------
//AnsiString BinToHexA(void *data, int size)
//{
//	char *hex_s = new char[size*2+1]; hex_s[size*2] = '\0';
//	BinToHex(data, hex_s, size);
//	AnsiString hex_a(hex_s);
//	delete []hex_s;
//	return hex_a;
//}
//---------------------------------------------------------------------------
UnicodeString BinToHexW(void *data, int size)
{
	return UnicodeString(BinToHexA(data, size));
}
//---------------------------------------------------------------------------
AnsiString StrToHex(UnicodeString text)
{
	void *data = text.w_str();
	int size = (text.Length()+1)*sizeof(wchar_t); // zero-terminator added too
	return BinToHexA(data, size);
}
//---------------------------------------------------------------------------
UnicodeString HexToStr(AnsiString hex)
{
	int len = hex.Length()/2/sizeof(wchar_t);  // zero-terminator already included by _StrToHex()
	wchar_t *str_s = new wchar_t[len];
	HexToBin(hex.c_str(), (void*)str_s, hex.Length()/2);
	return UnicodeString(str_s);
}

//---------------------------------------------------------------------------


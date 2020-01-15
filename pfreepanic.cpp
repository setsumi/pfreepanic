//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("Unit1.cpp", FormPfreepanic);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	for (int i = 1; i <= ParamCount(); i++) {
		if (LowerCase(ParamStr(i)).Pos(L"close")) {
			HWND hw = FindWindow(L"TFormPfreepanic", NULL);
			if (hw)
				::SendNotifyMessage(hw, WM_APP+1, 0, 0);
			return -1;
		}
	}

	CreateMutex(0, FALSE, L"Local\\$pfreepanic$");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return -1;

	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TFormPfreepanic), &FormPfreepanic);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------

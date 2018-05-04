//---------------------------------------------------------------------------

#include <vcl.h>

#include <algorithm>
#include <iterator>
#include <Psapi.h>
#include <Mmsystem.h>
#include <Registry.hpp>

#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

struct TermWnd {
	UnicodeString wndtitle;
	UnicodeString wndclass;

	TermWnd(UnicodeString ttl, UnicodeString cls) {
		wndtitle = ttl;
		wndclass = cls;
  }
};

HHOOK hHook = NULL;
bool Working = false;
DWORD Key = 0;
DWORD TermKey = 0;
int Game = -1;
int SuspendProcess = 1;
TStringList *pGamesList;
TList *pTermList;

#define MODULE_NAME L"soundvoltex.dll"
#define MEM_OFFSET  0xC00 // offset padding relative to .dll file

ULONG data0_offset[] = { 0x1CAAFF, 0x196BDF };
ULONG data1_offset[] = { 0x1CACC6, 0x196C66 };
#define DATA1_SIZE 16

BYTE pf_off0[] = { 0x00, 0x00 };
BYTE pf_off1[2][16] = {
		{ 0x8B, 0x83, 0x64, 0x10, 0x00, 0x00, 0x8D, 0x48, 0x01, 0x83, 0xF9, 0x04, 0x56, 0x57, 0x7F, 0x52 },
		{ 0x8B, 0x83, 0x78, 0x0A, 0x00, 0x00, 0x8D, 0x48, 0x01, 0x83, 0xF9, 0x03, 0x56, 0x57, 0x7F, 0x52 }
	};
BYTE pf_on0[] = { 0x02, 0x02 };
BYTE pf_on1[2][16] = {
		{ 0xB8, 0x01, 0x00, 0x00, 0x00, 0x89, 0x83, 0x64, 0x10, 0x00, 0x00, 0x90, 0x56, 0x57, 0x90, 0x90 },
		{ 0xB8, 0x01, 0x00, 0x00, 0x00, 0x89, 0x83, 0x78, 0x0A, 0x00, 0x00, 0x90, 0x56, 0x57, 0x90, 0x90 }
	};


UnicodeString WinFormatError(DWORD errNo)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, errNo, 0,
			(LPTSTR)&lpMsgBuf, 0, NULL);
	UnicodeString errMsg; errMsg.sprintf(L"%d: %s", errNo, (TCHAR*)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return errMsg;
}

void Error(UnicodeString msg)
{
	Form1->Memo1->Color = clRed;
	Form1->Memo1->Font->Color = clYellow;
	Form1->Memo1->Font->Style = TFontStyles() << fsBold;

	Form1->Memo1->Lines->Text = UnicodeString(L"ERROR:\n") + msg;

	Form1->Memo1->SetFocus();
	MessageBeep(MB_ICONHAND);
}

void WinError(UnicodeString msg)
{
	Error(msg + L"\n" + WinFormatError(GetLastError()));
}

//---------------------------------------------------------------------------
HWND FindGameWnd()
{
	HWND hWnd = NULL;
	Game = -1;
	for (int i = 0; i < pGamesList->Count; i++) {
		hWnd = FindWindow(pGamesList->Strings[i].c_str(), NULL);
		if (hWnd) {
    	Game = i;
			break;
		}
	}
	return hWnd;
}

//---------------------------------------------------------------------------
HWND FindTermWnd()
{
	HWND hWnd = NULL;
	for (int i = 0; i < pTermList->Count; i++) {
		TermWnd *pwi = (TermWnd*)pTermList->Items[i];
		hWnd = FindWindow(pwi->wndclass.c_str(), pwi->wndtitle.c_str());
		if (hWnd) {
			break;
		}
	}
	return hWnd;
}

//---------------------------------------------------------------------------
void TogglePFree()
{
	Working = true;
	Form1->Memo1->Clear();
	UnicodeString txt;

	HANDLE hProc = NULL;
	HWND hWnd = NULL;
	DWORD procID = NULL;

	BYTE data0 = 1;
	BYTE data1[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

	HMODULE hMods[1024];
	DWORD cbNeeded;
	bool found = false;
	ULONG baseAddr = 0;

	hWnd = FindGameWnd();
	if (!hWnd) {
		Error(L"Game window not found");
		goto getout;
	}

	if (!GetWindowThreadProcessId(hWnd, &procID)) {
		WinError(L"Get window process ID failed");
		goto getout;
	}

	if (procID != NULL)
	{
		if (SuspendProcess) {
			if (!DebugActiveProcess(procID)) {
				WinError(L"Suspend process failed");
				goto getout;
			}
		}

		hProc = OpenProcess(/*PROCESS_ALL_ACCESS*/
				PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
				PROCESS_QUERY_INFORMATION, FALSE, procID);
	}
	if (!hProc) {
		WinError(L"Can not open process");
		goto getout;
	}

	if (!EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded)) {
		WinError(L"Enumerationg process modules failed");
		goto getout;
	}

	for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
	{
		TCHAR szModName[MAX_PATH];

		// Get the full path to the module's file.
		if (GetModuleFileNameEx(hProc, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
		{
			MODULEINFO info;
			if (!GetModuleInformation(hProc, hMods[i], &info, sizeof(MODULEINFO))) {
				WinError(L"Get module information failed");
				goto getout;
			}

			// find our module and get base address
			UnicodeString name(szModName);
			if (name.Pos(MODULE_NAME) > 0) {
				found = true;
				baseAddr = (ULONG)info.lpBaseOfDll;
				break;
			}
//			line.sprintf(L"\t%s\t\t 0x%08X\t\t 0x%08X\t\t 0x%08X", szModName, info.lpBaseOfDll, info.SizeOfImage, info.EntryPoint);
//			Memo1->Lines->Add(line);
		}
	}
	if (!found) {
		Error(UnicodeString(L"Module ")+ MODULE_NAME + L" not found");
		goto getout;
	}

	// read current data
	if (!ReadProcessMemory(hProc, (BYTE*)baseAddr + MEM_OFFSET + data0_offset[Game], &data0, 1, NULL)) {
		WinError(L"Read process memory failed");
		goto getout;
	}
	if (!ReadProcessMemory(hProc, (BYTE*)baseAddr + MEM_OFFSET + data1_offset[Game], data1, DATA1_SIZE, NULL)) {
		WinError(L"Read process memory failed");
		goto getout;
	}

	// OFF test
	if (data0 == pf_off0[Game] && memcmp(data1, pf_off1[Game], DATA1_SIZE) == 0)
	{
		// write PFree ON
		if (!WriteProcessMemory(hProc, (BYTE*)baseAddr + MEM_OFFSET + data0_offset[Game], pf_on0 + Game, 1, NULL)) {
			WinError(L"Write process memory failed");
			goto getout;
		}
		if (!WriteProcessMemory(hProc, (BYTE*)baseAddr + MEM_OFFSET + data1_offset[Game], pf_on1[Game], DATA1_SIZE, NULL)) {
			WinError(L"Write process memory failed");
			goto getout;
		}
		// notify
		PlaySound((ExtractFilePath(Application->ExeName) + L"on.wav").c_str(), NULL, SND_FILENAME|SND_ASYNC);
		Form1->Caption = L"PFREE";
	} else
	// ON test
	if (data0 == pf_on0[Game] && memcmp(data1, pf_on1[Game], DATA1_SIZE) == 0)
	{
		// write PFree OFF
		if (!WriteProcessMemory(hProc, (BYTE*)baseAddr + MEM_OFFSET + data0_offset[Game], pf_off0 + Game, 1, NULL)) {
			WinError(L"Write process memory failed");
			goto getout;
		}
		if (!WriteProcessMemory(hProc, (BYTE*)baseAddr + MEM_OFFSET + data1_offset[Game], pf_off1[Game], DATA1_SIZE, NULL)) {
			WinError(L"Write process memory failed");
			goto getout;
		}
		// notify
		PlaySound((ExtractFilePath(Application->ExeName) + L"off.wav").c_str(), NULL, SND_FILENAME|SND_ASYNC);
		Form1->Caption = L"NORMAL";
	} else
	{
		Error(L"Invalid game data");
		goto getout;
	}

	txt.sprintf(L"%02X\n%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", data0, data1[0],data1[1],data1[2],data1[3],data1[4],data1[5],data1[6],data1[7],data1[8],data1[9],data1[10],data1[11],data1[12],data1[13],data1[14],data1[15]);
	Form1->MemoResetStyle();
	Form1->Memo1->Lines->Text = txt;

getout:
	// cleanup
	if (hWnd)  CloseHandle(hWnd);
	if (hProc) CloseHandle(hProc);

	if (SuspendProcess && procID)
		DebugActiveProcessStop(procID);

	Working = false;
}

//---------------------------------------------------------------------------
void TerminateGame()
{
	HANDLE hProc = NULL;
	HWND hWnd = NULL;
	DWORD procID = NULL;

	hWnd = FindTermWnd();
	if (!hWnd) {
		goto getout2;
	}

	if (!GetWindowThreadProcessId(hWnd, &procID)) {
		WinError(L"Get window process ID failed");
		goto getout2;
	}

	if (procID)
		hProc = OpenProcess(PROCESS_TERMINATE, FALSE, procID);
	if (!hProc) {
		WinError(L"Can not open process");
		goto getout2;
	}

	if (!TerminateProcess(hProc, 0)) {
		WinError(L"Can not terminate process");
		goto getout2;
	}

getout2:
	// cleanup
	if (hWnd)  CloseHandle(hWnd);
	if (hProc) CloseHandle(hProc);
}

//---------------------------------------------------------------------------
// callback function for keyboard hook
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// struct to get virtual key codes
		KBDLLHOOKSTRUCT kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

		if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
		{
			if (Form1->edtKey->Tag) {
				Key = kbdStruct.vkCode;
				Form1->Memo1->SetFocus();
				Form1->Save();
			} else
			if (Form1->edtTermKey->Tag) {
				TermKey = kbdStruct.vkCode;
				Form1->Memo1->SetFocus();
				Form1->Save();
			} else
			if (kbdStruct.vkCode == Key && !Working)
			{
				Form1->Memo1->SetFocus();
				TogglePFree();
			} else
			if (kbdStruct.vkCode == TermKey)
			{
				Form1->Memo1->SetFocus();
				TerminateGame();
			}
		}
	}

	// if the message does not get processed
	return CallNextHookEx (0, nCode, wParam, lParam);
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	pGamesList = new TStringList();
	pGamesList->Add(L"SOUND VOLTEX IV HEAVENLY HAVEN 1");
	pGamesList->Add(L"SOUND VOLTEX III GRAVITY WARS");

	pTermList = new TList();
	pTermList->Add(new TermWnd(L"SOUND VOLTEX IV HEAVENLY HAVEN 1", L"SOUND VOLTEX IV HEAVENLY HAVEN 1"));
	pTermList->Add(new TermWnd(L"SOUND VOLTEX III GRAVITY WARS", L"SOUND VOLTEX III GRAVITY WARS"));
	pTermList->Add(new TermWnd(L"beatmania IIDX 24 SINOBUZ", L"beatmania IIDX 24 SINOBUZ"));
	pTermList->Add(new TermWnd(L"pop'n music eclale", L"pop'n music eclale"));
	pTermList->Add(new TermWnd(L"MUSECA", L"MUSECA"));
	pTermList->Add(new TermWnd(L"BeatStream", L"BeatStream"));
	pTermList->Add(new TermWnd(L"", L"_")); // GITADORA Tri-Boost Re:EVOLVE

	// load .ini
	Load();
	// set global keyboard hook to capture key press
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, GetModuleHandle(NULL), 0);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	if (hHook) UnhookWindowsHookEx(hHook);
	Save();

	delete pGamesList;
	delete pTermList;
}

//---------------------------------------------------------------------------
void TForm1::Load()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
	Key = (DWORD)ini->ReadInteger(L"GENERAL", L"Hotkey", 106); // by default Numpad * key
	edtKey->Text = IntToStr((int)Key);
	TermKey = (DWORD)ini->ReadInteger(L"GENERAL", L"TerminateHotkey", 109); // by default Numpad - key
	edtTermKey->Text = IntToStr((int)TermKey);
	SuspendProcess = ini->ReadInteger(L"GENERAL", L"SuspendProcess", 0);
	delete ini;
}

//---------------------------------------------------------------------------
void TForm1::Save()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
	ini->WriteInteger(L"GENERAL", L"Hotkey", (int)Key);
	ini->WriteInteger(L"GENERAL", L"TerminateHotkey", (int)TermKey);
	ini->WriteInteger(L"GENERAL", L"SuspendProcess", SuspendProcess);
	delete ini;
}

//---------------------------------------------------------------------------
void TForm1::MemoResetStyle()
{
	Memo1->Color = clWindow;
	Memo1->Font->Color = clWindowText;
	Memo1->Font->Style = TFontStyles();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::edtKeyEnter(TObject *Sender)
{
	edtKey->Tag = 1;
	edtKey->Text = L"Press single key";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtKeyExit(TObject *Sender)
{
	edtKey->Tag = 0;
	edtKey->Text = IntToStr((int)Key);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtTermKeyEnter(TObject *Sender)
{
	edtTermKey->Tag = 1;
	edtTermKey->Text = L"Press single key";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtTermKeyExit(TObject *Sender)
{
	edtTermKey->Tag = 0;
	edtTermKey->Text = IntToStr((int)TermKey);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btnInfoClick(TObject *Sender)
{
	ShowMessage(L"PFree mode is supported on:\n"
		"\tSOUND VOLTEX IV HEAVENLY HAVEN 1 (2017112800)\n"
		"\tSOUND VOLTEX III GRAVITY WARS (2016121200)\n\n"
		"Terminate game is supported on:\n"
		"\tSOUND VOLTEX IV HEAVENLY HAVEN 1\n"
		"\tSOUND VOLTEX III GRAVITY WARS\n"
		"\tbeatmania IIDX 24 SINOBUZ\n"
		"\tpop'n music eclale\n"
		"\tMUSECA\n"
		"\tBeatStream\n"
		"\tGITADORA Tri-Boost Re:EVOLVE"
		);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl.h>

#include <Psapi.h>
#include <Mmsystem.h>
#include <Registry.hpp>
#include <FileCtrl.hpp>
#include <vector>
#include <list>

#pragma hdrstop

#include "Unit1.h"
#include "osd.h"
#include "Utils.h"
#include "volume.h"

#ifdef _WIN64
#pragma comment(lib, "Psapi.a")
#else
#pragma comment(lib, "Psapi.lib")
#endif

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------
#define MEM_OFFSET  0xC00 // offset padding relative to .dll file

typedef struct HexCode
{
	DWORD offset;
	std::vector<BYTE> off;
	std::vector<BYTE> on;
} HexCode;

typedef struct Game
{
	UnicodeString winclass;
	UnicodeString modulename;
	std::list<std::list<HexCode> > versions;
	void clear() {
		winclass = L"";
		modulename = L"";
		versions.clear();
	}
} Game;

typedef struct TermWnd
{
	UnicodeString wndtitle;
	UnicodeString wndclass;
	TermWnd(UnicodeString ttl, UnicodeString cls) {
		wndtitle = ttl;
		wndclass = cls;
	}
} TermWnd;

//---------------------------------------------------------------------------
void BeginPrint();
void Print(UnicodeString msg);
void Error(UnicodeString msg);
void WinError(UnicodeString msg);

//---------------------------------------------------------------------------
HHOOK hHook = NULL;
bool Working = false;
bool Terminating = false;
bool Voluming = false;

DWORD Key = 0;
DWORD TermKey = 0;
DWORD ScreenshotKey = 0;
DWORD VolumeUpKey = 0;
DWORD VolumeDownKey = 0;
DWORD ToggleMuteKey = 0;
int VolumeIncrement = 2;

std::list<Game> g_games;
TList *pTermList;
tOSD *pOSD;

//---------------------------------------------------------------------------
void InitGameCodes()
{
	Game game;
	std::list<HexCode> ver;
	HexCode hex;
	BYTE *data;

#ifdef _WIN64
	// MUSECA 1+1/2
	game.winclass = L"MUSECA";
	game.modulename = L"museca.dll";

	// 2018-07-30
	hex.offset = 0x17E587;
	data = (BYTE*)"\xFF\x83\x48\x14\x00\x00";
	hex.off.assign(data, data + 6);
	data = (BYTE*)"\x90\x90\x90\x90\x90\x90";
	hex.on.assign(data, data + 6);
	ver.push_back(hex);

	game.versions.push_back(ver);
	ver.clear();

	g_games.push_back(game);
	game.clear();
#else
	// SDVX4
	game.winclass = L"SOUND VOLTEX IV HEAVENLY HAVEN 1";
	game.modulename = L"soundvoltex.dll";

	// 2019020600 final
	hex.offset = 0x1D6B8F;
	data = (BYTE*)"\x00";
	hex.off.assign(data, data + 1);
	data = (BYTE*)"\x02";
	hex.on.assign(data, data + 1);
	ver.push_back(hex);

	hex.offset = 0x1D6D76;
	data = (BYTE*)"\x8B\x83\x64\x10\x00\x00\x8D\x48\x01\x83\xF9\x04\x56\x57\x7F\x52";
	hex.off.assign(data, data + 16);
	data = (BYTE*)"\xB8\x01\x00\x00\x00\x89\x83\x64\x10\x00\x00\x90\x56\x57\x90\x90";
	hex.on.assign(data, data + 16);
	ver.push_back(hex);

	game.versions.push_back(ver);
	ver.clear();

	// 2018011602 enhanced continue
	hex.offset = 0x1CEE4F;
	data = (BYTE*)"\x00";
	hex.off.assign(data, data + 1);
	data = (BYTE*)"\x02";
	hex.on.assign(data, data + 1);
	ver.push_back(hex);

	hex.offset = 0x1CF016;
	data = (BYTE*)"\x8B\x83\x64\x10\x00\x00\x8D\x48\x01\x83\xF9\x04\x56\x57\x7F\x52";
	hex.off.assign(data, data + 16);
	data = (BYTE*)"\xB8\x01\x00\x00\x00\x89\x83\x64\x10\x00\x00\x90\x56\x57\x90\x90";
	hex.on.assign(data, data + 16);
	ver.push_back(hex);

	game.versions.push_back(ver);
	ver.clear();

	g_games.push_back(game);
	game.clear();

	// SDVX3
	game.winclass = L"SOUND VOLTEX III GRAVITY WARS";
	game.modulename = L"soundvoltex.dll";

	// 2016121200 season 2
	hex.offset = 0x196BDF;
	data = (BYTE*)"\x00";
	hex.off.assign(data, data + 1);
	data = (BYTE*)"\x02";
	hex.on.assign(data, data + 1);
	ver.push_back(hex);

	hex.offset = 0x196C66;
	data = (BYTE*)"\x8B\x83\x78\x0A\x00\x00\x8D\x48\x01\x83\xF9\x03\x56\x57\x7F\x52";
	hex.off.assign(data, data + 16);
	data = (BYTE*)"\xB8\x01\x00\x00\x00\x89\x83\x78\x0A\x00\x00\x90\x56\x57\x90\x90";
	hex.on.assign(data, data + 16);
	ver.push_back(hex);

	game.versions.push_back(ver);
	ver.clear();

	g_games.push_back(game);
	game.clear();
#endif
}

//---------------------------------------------------------------------------
// Return:
// 1 match
// 0 no match
// -1 read failed
int MemCompare(HANDLE hproc, LPVOID address, LPVOID data, size_t size)
{
	int rv = 0;
	BYTE *buffer = new BYTE[size];
	if (!ReadProcessMemory(hproc, address, buffer, size, NULL)) {
		WinError(L"Read process memory failed");
		rv = -1;
		goto exit_MemCompare;
	}
	if (memcmp(buffer, data, size) == 0) {
		rv = 1;
	}
exit_MemCompare:
	delete []buffer;
	return rv;
}

//---------------------------------------------------------------------------
// __OUT__ hProc
LPVOID GetBaseAddress(HWND hwindow, UnicodeString modulename, HANDLE &hProc)
{
	DWORD procID = NULL;

	HMODULE hMods[1024];
	DWORD cbNeeded;
	bool module_found = false;
	LPVOID baseAddr = NULL;

	if (!GetWindowThreadProcessId(hwindow, &procID)) {
		WinError(L"Get window process ID failed");
		goto exit_GetBaseAddress;
	}

	if (procID != NULL)
	{
		hProc = OpenProcess(/*PROCESS_ALL_ACCESS*/
				PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
				PROCESS_QUERY_INFORMATION, FALSE, procID);
	}
	if (!hProc) {
		WinError(L"Can not open process");
		goto exit_GetBaseAddress;
	}

	if (!EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded)) {
		WinError(L"Enumerating process modules failed");
		goto exit_GetBaseAddress;
	}

	for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
	{
		TCHAR szModName[MAX_PATH];

		// Get the full path to the module's file.
		if (GetModuleFileNameEx(hProc, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
		{
			UnicodeString name(szModName);
			if (name.Pos(modulename) > 0) { // find our module and get base address
				module_found = true;
				MODULEINFO info;
				if (!GetModuleInformation(hProc, hMods[i], &info, sizeof(MODULEINFO))) {
					WinError(L"Get module information failed");
					goto exit_GetBaseAddress;
				}
				baseAddr = info.lpBaseOfDll;
				break;
			}
		}
	}
	if (!module_found) {
		Error(UnicodeString(L"Module ")+ modulename + L" not found");
	}

exit_GetBaseAddress:
	return baseAddr;
}

//---------------------------------------------------------------------------
void Toggle()
{
	bool version_found = false;
	HANDLE hproc = NULL;

	BeginPrint();
	std::list<Game>::iterator i = g_games.begin();
	while (i != g_games.end()) {
		Game &game = *i; i++; // next game

		HWND hgame_wnd = FindWindow(game.winclass.c_str(), NULL);
		if (!hgame_wnd) {
			continue;
		}
		Print(game.winclass);

		LPVOID baseAddr = GetBaseAddress(hgame_wnd, game.modulename, hproc);
		if (!baseAddr || !hproc) {
			goto exit_Toggle;
		}

		std::list<std::list<HexCode> >::iterator j = game.versions.begin();
		while (j != game.versions.end()) {
			std::list<HexCode> &ver = *j; j++; // next version
			size_t num_total = ver.size(); // number of hex codes

			for (int onoff = 0; onoff < 2; onoff++) {
				std::vector<BYTE> *hexdata; // on/off flipper
				// Test
				size_t num_found = 0;
				std::list<HexCode>::iterator k = ver.begin();
				while (k != ver.end()) {
					HexCode &hex = *k; k++;
					hexdata = (onoff == 0)? &hex.on : &hex.off; // test PFREE first

					int result = MemCompare(hproc, (BYTE*)baseAddr + MEM_OFFSET + hex.offset,
																	&(*hexdata).front(), (*hexdata).size());
					if (result == -1)
						goto exit_Toggle;
					else if (result)
						num_found++;
				}
				if (num_found == num_total) {
					version_found = true;
					// Write
					k = ver.begin();
					while (k != ver.end()) {
						HexCode &hex = *k; k++;
						hexdata = (onoff == 0)? &hex.off : &hex.on; // write NORMAL first

						if (!WriteProcessMemory(hproc, (BYTE*)baseAddr + MEM_OFFSET + hex.offset,
																		&(*hexdata).front(), (*hexdata).size(), NULL)) {
							WinError(L"Write process memory failed");
							goto exit_Toggle;
						}
						UnicodeString str;
						str.sprintf(L"%X: %s", hex.offset, BinToHexW(&(*hexdata).front(), (*hexdata).size()).w_str());
						Print(str);
					}
					// Write success, show notification
					if (onoff == 0) { // notify NORMAL first
						Form1->Caption = UnicodeString(L"NORMAL - ") + Application->Title;
						if (Form1->chkOSDEnabled->Checked)
							pOSD->SendMessage(L"Credit Mode", Form1->udOSDDuration->Position);
						if (Form1->chkVoiceEnabled->Checked)
							PlaySound((ExtractFilePath(Application->ExeName) + (Form1->rbVoiceEnglish->Checked? L"off-eng.wav" : L"off.wav")).c_str(), NULL, SND_FILENAME|SND_ASYNC);
					} else {
						Form1->Caption = UnicodeString(L"PFREE - ") + Application->Title;
						if (Form1->chkOSDEnabled->Checked)
							pOSD->SendMessage(L"Premium Free Mode", Form1->udOSDDuration->Position);
						if (Form1->chkVoiceEnabled->Checked)
							PlaySound((ExtractFilePath(Application->ExeName) + (Form1->rbVoiceEnglish->Checked? L"on-eng.wav" : L"on.wav")).c_str(), NULL, SND_FILENAME|SND_ASYNC);
					}
					// Done, exiting
					goto exit_Toggle;
				}
			}
		}
		if (!version_found) {
			Error(L"Invalid game data.\nPress [Information...] for supported game version.");
			goto exit_Toggle;
		}
	}
	Error(L"Game window not found");

exit_Toggle:
	if (hproc) CloseHandle(hproc);
}

//---------------------------------------------------------------------------
void BeginPrint()
{
	Form1->Memo1->Color = clWindow;
	Form1->Memo1->Font->Color = clWindowText;
	Form1->Memo1->Font->Style = TFontStyles();
	Form1->Memo1->Clear();
}

void Print(UnicodeString msg)
{
	Form1->Memo1->Lines->Add(msg);
}

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
void ThreadWatchDir()
{
	// Directory to watch.
	UnicodeString csDirectory = Form1->edtSSNpath->Text; //L"C:\\";
	HANDLE hDirectory = CreateFile( csDirectory.c_str(),
																FILE_LIST_DIRECTORY,
																FILE_SHARE_READ|FILE_SHARE_DELETE,
																NULL,
																OPEN_EXISTING,
																FILE_FLAG_BACKUP_SEMANTICS,
																NULL );
	if (hDirectory == INVALID_HANDLE_VALUE) {
		Form1->lblSSNstatus->Caption = L"Error";
		return;
	}

	while (1)
	{
		// Buffer to get the notification information.
		const int MAX_BUFFER = 1024;
		BYTE Buffer[MAX_BUFFER];
		BYTE *pBuffer = Buffer;
		DWORD dwBytesReturned = 0;
		BOOL readsuccess = ReadDirectoryChangesW( hDirectory,
												 Buffer,
												 MAX_BUFFER,
												 FALSE,
												 FILE_NOTIFY_CHANGE_FILE_NAME,
												 &dwBytesReturned,
												 0, 0 );
		if (!readsuccess) {
			CloseHandle(hDirectory);
			Form1->lblSSNstatus->Caption = L"Error";
			return;
		}

		// go through buffer
		FILE_NOTIFY_INFORMATION* pNotifyInfo = (FILE_NOTIFY_INFORMATION*)Buffer;
		UnicodeString files;
		bool first = true;
		while (1)
		{
			if (pNotifyInfo->Action == FILE_ACTION_ADDED) {
				if (!first) files += L"\n";

				// get file
				size_t fnlen = pNotifyInfo->FileNameLength / 2;
				wchar_t fname[MAX_PATH];
				wcsncpy(fname, pNotifyInfo->FileName, fnlen); fname[fnlen] = L'\0';
				files += fname;

				first = false;
			}
			// next buffer entry
			if (pNotifyInfo->NextEntryOffset) {
				pBuffer += pNotifyInfo->NextEntryOffset;
				pNotifyInfo = (FILE_NOTIFY_INFORMATION*)pBuffer;
			} else {
				break;
			}
		}
		// notify of files
		if (files.Length()) {
			pOSD->SendMessage(UnicodeString(L"New screenshot:\n") + files, Form1->udOSDDuration->Position);
		}
	}
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
void Terminate()
{
	HANDLE hProc;
	HWND hWnd;
	DWORD procID = 0;

	hWnd = FindTermWnd();
	if (!hWnd) {
		goto exit_Terminate;
	}

	// get process ID
	if (!GetWindowThreadProcessId(hWnd, &procID)) {
		WinError(L"Get window process ID failed");
		goto exit_Terminate;
	}

	// soft close window first
	SendNotifyMessage(hWnd, WM_CLOSE, 0, 0);
	Sleep(500);

	// hard terminate process
	hProc = OpenProcess(PROCESS_TERMINATE, FALSE, procID);
	if (!hProc) { // already closed or some error, whatever
		goto exit_Terminate;
	}

	TerminateProcess(hProc, 0);

exit_Terminate:
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

		if (nCode == HC_ACTION && wParam == WM_KEYDOWN && kbdStruct.vkCode > 0)
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
			if (Form1->edtScreenshotKey->Tag) {
				ScreenshotKey = kbdStruct.vkCode;
				Form1->Memo1->SetFocus();
				Form1->Save();
			} else
			if (Form1->edtVolumeUpKey->Tag) {
				VolumeUpKey = kbdStruct.vkCode;
				Form1->Memo1->SetFocus();
				Form1->Save();
			} else
			if (Form1->edtVolumeDownKey->Tag) {
				VolumeDownKey = kbdStruct.vkCode;
				Form1->Memo1->SetFocus();
				Form1->Save();
			} else
			if (Form1->edtToggleMuteKey->Tag) {
				ToggleMuteKey = kbdStruct.vkCode;
				Form1->Memo1->SetFocus();
				Form1->Save();
			} else
			if (kbdStruct.vkCode == Key && !Working) // Toggle PFree
			{
				Working = true;
				Form1->Memo1->SetFocus();
				Toggle();
				Working = false;
			} else
			if (kbdStruct.vkCode == TermKey && !Terminating) // Terminate Game
			{
				Terminating = true;
				Form1->Memo1->SetFocus();
				Terminate();
				Terminating = false;
			} else
			if (kbdStruct.vkCode == ScreenshotKey) // Take Screenshot
			{
				Form1->Memo1->SetFocus();
				if (Form1->edtSSNpath->Text.Length())
					pOSD->SendMessage(Form1->edtSSNpath->Text, 1);
			} else
			if ((kbdStruct.vkCode == VolumeUpKey || kbdStruct.vkCode == VolumeDownKey) && !Voluming) // Volume Up/Down
			{
				Voluming = true;
				Form1->Memo1->SetFocus();
				try {
					double volume = ChangeVolume((kbdStruct.vkCode == VolumeUpKey)? 0.01*VolumeIncrement: -(0.01*VolumeIncrement), TRUE);
					if (Form1->chkOSDEnabled->Checked) {
						BOOL muted = ChangeMute(FALSE, FALSE, TRUE);
						UnicodeString msg;
						msg.sprintf(L"Volume: %d %%", (int)ceil(volume*100));
						if (muted) msg += L" (muted)";
						pOSD->SendMessage(msg, Form1->udOSDDuration->Position);
                    }
				} catch (Exception &e) {
					Error(e.Message);
				}
				Voluming = false;
			} else
			if (kbdStruct.vkCode == ToggleMuteKey) // Toggle Mute
			{
				Form1->Memo1->SetFocus();
				try {
					BOOL muted = ChangeMute(FALSE, TRUE);
					if (Form1->chkOSDEnabled->Checked)
						pOSD->SendMessage(muted? L"Muted": L"Unmuted", Form1->udOSDDuration->Position);
				} catch (Exception &e) {
					Error(e.Message);
				}
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
#ifdef _WIN64
	Application->Title = L"pfreepanic64";
	Caption = L"pfreepanic64";
#else
	Application->Title = L"pfreepanic";
	Caption = L"pfreepanic";
#endif

	InitGameCodes();

	pTermList = new TList();
	pTermList->Add(new TermWnd(L"SOUND VOLTEX IV HEAVENLY HAVEN 1", L"SOUND VOLTEX IV HEAVENLY HAVEN 1"));
	pTermList->Add(new TermWnd(L"SOUND VOLTEX III GRAVITY WARS", L"SOUND VOLTEX III GRAVITY WARS"));
	pTermList->Add(new TermWnd(L"SOUND VOLTEX II -infinite infection-", L"SOUND VOLTEX II -infinite infection-"));
	pTermList->Add(new TermWnd(L"beatmania IIDX 24 SINOBUZ", L"beatmania IIDX 24 SINOBUZ"));
	pTermList->Add(new TermWnd(L"pop'n music eclale", L"pop'n music eclale"));
	pTermList->Add(new TermWnd(L"MUSECA", L"MUSECA"));
	pTermList->Add(new TermWnd(L"BeatStream", L"BeatStream"));
	pTermList->Add(new TermWnd(L"", L"_")); // GITADORA Tri-Boost Re:EVOLVE / Matixx

	// load .ini
	Load();
	// set global keyboard hook to capture key press
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, GetModuleHandle(NULL), 0);
	// OSD object
	pOSD = new tOSD();
	// Screenshot folder monitoring thread
	if (edtSSNpath->Text.Length()) {
		HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadWatchDir, 0, 0, 0);
		CloseHandle(h);
	}
}

//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	if (hHook) UnhookWindowsHookEx(hHook);
	Save();
	delete pTermList;
	delete pOSD;
}

//---------------------------------------------------------------------------
void TForm1::Load()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));

	Key = (DWORD)ini->ReadInteger(L"GENERAL", L"Hotkey", 0);
	edtKey->Text = IntToStr((int)Key);
	TermKey = (DWORD)ini->ReadInteger(L"GENERAL", L"TerminateHotkey", 0);
	edtTermKey->Text = IntToStr((int)TermKey);
	ScreenshotKey = (DWORD)ini->ReadInteger(L"GENERAL", L"ScreenshotHotkey", 0);
	edtScreenshotKey->Text = IntToStr((int)ScreenshotKey);

	VolumeUpKey = (DWORD)ini->ReadInteger(L"GENERAL", L"VolumeUpHotkey", 0);
	edtVolumeUpKey->Text = IntToStr((int)VolumeUpKey);
	VolumeDownKey = (DWORD)ini->ReadInteger(L"GENERAL", L"VolumeDownHotkey", 0);
	edtVolumeDownKey->Text = IntToStr((int)VolumeDownKey);
	ToggleMuteKey = (DWORD)ini->ReadInteger(L"GENERAL", L"ToggleMuteHotkey", 0);
	edtToggleMuteKey->Text = IntToStr((int)ToggleMuteKey);
	VolumeIncrement = ini->ReadInteger(L"GENERAL", L"VolumeIncrement", 2);

	if (1 == ini->ReadInteger(L"GENERAL", L"VoiceEnglish", 1))
		rbVoiceEnglish->Checked = true;
	else
		rbVoiceJapanese->Checked = true;
	chkVoiceEnabled->Checked = (1 == ini->ReadInteger(L"GENERAL", L"VoiceEnabled", 1));
	udOSDDuration->Position = ini->ReadInteger(L"GENERAL", L"OSDDuration", 120);
	chkOSDEnabled->Checked = ini->ReadInteger(L"GENERAL", L"OSDEnabled", 0) == 1;
	edtSSNpath->Text = HexToStr(ini->ReadString(L"GENERAL", L"OSDScreenshotPath", L""));
	lblSSNstatus->Caption = edtSSNpath->Text.Length()? L"Monitoring..." : L"Disabled";
	delete ini;
}

//---------------------------------------------------------------------------
void TForm1::Save()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));

	ini->WriteInteger(L"GENERAL", L"Hotkey", (int)Key);
	ini->WriteInteger(L"GENERAL", L"TerminateHotkey", (int)TermKey);
	ini->WriteInteger(L"GENERAL", L"ScreenshotHotkey", (int)ScreenshotKey);

	ini->WriteInteger(L"GENERAL", L"VolumeUpHotkey", (int)VolumeUpKey);
	ini->WriteInteger(L"GENERAL", L"VolumeDownHotkey", (int)VolumeDownKey);
	ini->WriteInteger(L"GENERAL", L"ToggleMuteHotkey", (int)ToggleMuteKey);
	ini->WriteInteger(L"GENERAL", L"VolumeIncrement", VolumeIncrement);

	ini->WriteInteger(L"GENERAL", L"VoiceEnglish", rbVoiceEnglish->Checked?1:0);
	ini->WriteInteger(L"GENERAL", L"VoiceEnabled", chkVoiceEnabled->Checked?1:0);
	ini->WriteInteger(L"GENERAL", L"OSDDuration", udOSDDuration->Position);
	ini->WriteInteger(L"GENERAL", L"OSDEnabled", chkOSDEnabled->Checked?1:0);
	ini->WriteString (L"GENERAL", L"OSDScreenshotPath", StrToHex(edtSSNpath->Text));
	delete ini;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::edtKeyEnter(TObject *Sender)
{
	edtKey->Tag = 1;
	edtKey->Text = L"Press single key";
}
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
void __fastcall TForm1::edtTermKeyExit(TObject *Sender)
{
	edtTermKey->Tag = 0;
	edtTermKey->Text = IntToStr((int)TermKey);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtScreenshotKeyEnter(TObject *Sender)
{
	edtScreenshotKey->Tag = 1;
	edtScreenshotKey->Text = L"Press single key";
}
void __fastcall TForm1::edtScreenshotKeyExit(TObject *Sender)
{
	edtScreenshotKey->Tag = 0;
	edtScreenshotKey->Text = IntToStr((int)ScreenshotKey);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtVolumeUpKeyEnter(TObject *Sender)
{
	edtVolumeUpKey->Tag = 1;
	edtVolumeUpKey->Text = L"Press single key";
}
void __fastcall TForm1::edtVolumeUpKeyExit(TObject *Sender)
{
	edtVolumeUpKey->Tag = 0;
	edtVolumeUpKey->Text = IntToStr((int)VolumeUpKey);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtVolumeDownKeyEnter(TObject *Sender)
{
	edtVolumeDownKey->Tag = 1;
	edtVolumeDownKey->Text = L"Press single key";
}
void __fastcall TForm1::edtVolumeDownKeyExit(TObject *Sender)
{
	edtVolumeDownKey->Tag = 0;
	edtVolumeDownKey->Text = IntToStr((int)VolumeDownKey);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::edtToggleMuteKeyEnter(TObject *Sender)
{
	edtToggleMuteKey->Tag = 1;
	edtToggleMuteKey->Text = L"Press single key";
}
void __fastcall TForm1::edtToggleMuteKeyExit(TObject *Sender)
{
	edtToggleMuteKey->Tag = 0;
	edtToggleMuteKey->Text = IntToStr((int)ToggleMuteKey);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btnKeyDisableClick(TObject *Sender)
{
	Key = 0;
	edtKey->Text = L"0";
	Save();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnTermKeyDisableClick(TObject *Sender)
{
	TermKey = 0;
	edtTermKey->Text = L"0";
	Save();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnScreenshotKeyDisableClick(TObject *Sender)
{
	ScreenshotKey = 0;
	edtScreenshotKey->Text = L"0";
	Save();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnVolumeUpKeyDisableClick(TObject *Sender)
{
	VolumeUpKey = 0;
	edtVolumeUpKey->Text = L"0";
	Save();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnVolumeDownKeyDisableClick(TObject *Sender)
{
	VolumeDownKey = 0;
	edtVolumeDownKey->Text = L"0";
	Save();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnToggleMuteKeyDisableClick(TObject *Sender)
{
	ToggleMuteKey = 0;
	edtToggleMuteKey->Text = L"0";
	Save();
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btnInfoClick(TObject *Sender)
{
	ShowMessage(L"How to use:\n"
		"\t1. Run the app\n"
		"\t2. Assign keys by clicking the corresponding edit box\n"
		"\t3. Start the game while keeping app running in the background\n"
		"\t4. To change mode press assigned key\n"
		"\t    (usually while in song selection screen or pre-song lobby)\n"
		"\t5. PROFIT\n"
		"\nPFree mode is supported on:\n"
#ifdef _WIN64
		"\tMUSECA 1+1/2 (2018-07-30)\n"
#else
		"\tSOUND VOLTEX IV HEAVENLY HAVEN (2019020600 final)\n"
		"\tSOUND VOLTEX IV HEAVENLY HAVEN (2018011602 enhanced continue)\n"
		"\tSOUND VOLTEX III GRAVITY WARS Season 2 (2016121200)\n"
#endif
		"\nTerminate game is supported on:\n"
		"\tSOUND VOLTEX IV HEAVENLY HAVEN 1\n"
		"\tSOUND VOLTEX III GRAVITY WARS\n"
		"\tSOUND VOLTEX II -infinite infection-\n"
		"\tbeatmania IIDX 24 SINOBUZ\n"
		"\tpop'n music eclale\n"
		"\tMUSECA\n"
		"\tBeatStream\n"
		"\tGITADORA Tri-Boost Re:EVOLVE\n"
		"\tGITADORA Matixx"
		);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btnOSDTestClick(TObject *Sender)
{
	if (!pOSD->SendMessage(L"Test Message", udOSDDuration->Position)) {
		Error(L"Unable to send message.\nPress [?] for instructions.");
	}
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btnOSDHelpClick(TObject *Sender)
{
	ShowMessage(L"How to use OSD:\n"
#ifdef _WIN64
		"\t1. Place dx9osd64.dll in the game folder\n"
		"\t2. Add it to command line with -k option\n"
		"\t    (e.g. spice64.exe -k dx9osd64.dll)\n"
#else
		"\t1. Place dx9osd.dll in the game folder\n"
		"\t2. Add it to command line with -k option\n"
		"\t    (e.g. launcher.exe -k sdvxhook.dll -k dx9osd.dll soundvoltex.dll)\n"
#endif
		"\t3. Run the game\n"
		"\t4. Press [Test] button\n"
		"\t5. Test message should appear in game\n"
		"\nOSD message duration is in frames (e.g. 60 for 1 second)"
		);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::edtSSNpathChange(TObject *Sender)
{
	lblSSNstatus->Caption = L"Restart required to apply changes";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnSSNpathClick(TObject *Sender)
{
	UnicodeString dir;
	if (SelectDirectory(L"Select Screenshots Folder", L"", dir, TSelectDirExtOpts()<<sdNewUI<<sdNewFolder, this)) {
		if (dir.Length() && dir[dir.Length()] != L'\\') dir += L'\\';
		edtSSNpath->Text = dir;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnSSNdisableClick(TObject *Sender)
{
	edtSSNpath->Text = L"";
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Vcl.AppEvnts.hpp>

//---------------------------------------------------------------------------
class TFormPfreepanic : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TEdit *edtKey;
	TLabel *Label1;
	TLabel *Label3;
	TEdit *edtTermKey;
	TButton *btnInfo;
	TGroupBox *GroupBox1;
	TButton *btnOSDTest;
	TButton *btnOSDHelp;
	TCheckBox *chkOSDEnabled;
	TLabel *group;
	TEdit *edtOSDDuration;
	TUpDown *udOSDDuration;
	TGroupBox *GroupBox2;
	TCheckBox *chkVoiceEnabled;
	TRadioButton *rbVoiceEnglish;
	TRadioButton *rbVoiceJapanese;
	TGroupBox *GroupBox3;
	TLabel *Label2;
	TLabel *lblSSNstatus;
	TEdit *edtSSNpath;
	TButton *btnSSNpath;
	TButton *btnSSNdisable;
	TEdit *edtScreenshotKey;
	TLabel *Label4;
	TButton *btnKeyDisable;
	TButton *btnTermKeyDisable;
	TButton *btnScreenshotKeyDisable;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TEdit *edtVolumeUpKey;
	TEdit *edtVolumeDownKey;
	TEdit *edtToggleMuteKey;
	TButton *btnVolumeUpKeyDisable;
	TButton *btnVolumeDownKeyDisable;
	TButton *btnToggleMuteKeyDisable;
	TLabel *Label8;
	TEdit *edtLoginKey;
	TButton *btnLoginKeyDisable;
	TTimer *tmrKbdPoll;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall edtKeyEnter(TObject *Sender);
	void __fastcall edtKeyExit(TObject *Sender);
	void __fastcall edtTermKeyEnter(TObject *Sender);
	void __fastcall edtTermKeyExit(TObject *Sender);
	void __fastcall btnInfoClick(TObject *Sender);
	void __fastcall btnOSDTestClick(TObject *Sender);
	void __fastcall btnOSDHelpClick(TObject *Sender);
	void __fastcall edtSSNpathChange(TObject *Sender);
	void __fastcall btnSSNpathClick(TObject *Sender);
	void __fastcall btnSSNdisableClick(TObject *Sender);
	void __fastcall edtScreenshotKeyEnter(TObject *Sender);
	void __fastcall edtScreenshotKeyExit(TObject *Sender);
	void __fastcall btnScreenshotKeyDisableClick(TObject *Sender);
	void __fastcall btnKeyDisableClick(TObject *Sender);
	void __fastcall btnTermKeyDisableClick(TObject *Sender);
	void __fastcall edtVolumeUpKeyEnter(TObject *Sender);
	void __fastcall edtVolumeUpKeyExit(TObject *Sender);
	void __fastcall edtVolumeDownKeyEnter(TObject *Sender);
	void __fastcall edtVolumeDownKeyExit(TObject *Sender);
	void __fastcall edtToggleMuteKeyEnter(TObject *Sender);
	void __fastcall edtToggleMuteKeyExit(TObject *Sender);
	void __fastcall btnVolumeUpKeyDisableClick(TObject *Sender);
	void __fastcall btnVolumeDownKeyDisableClick(TObject *Sender);
	void __fastcall btnToggleMuteKeyDisableClick(TObject *Sender);
	void __fastcall edtLoginKeyEnter(TObject *Sender);
	void __fastcall edtLoginKeyExit(TObject *Sender);
	void __fastcall btnLoginKeyDisableClick(TObject *Sender);
	void __fastcall tmrKbdPollTimer(TObject *Sender);
private:	// User declarations
	void __fastcall WndProc(TMessage& Message);
public:		// User declarations
	void Load();
	void Save();
	void Login();
	void KeyPress(BYTE key, DWORD waitms);
	void Hook();
	void Unhook(bool cont = true);

	__fastcall TFormPfreepanic(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPfreepanic *FormPfreepanic;
//---------------------------------------------------------------------------
#endif

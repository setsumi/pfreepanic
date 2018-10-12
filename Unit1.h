//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TEdit *edtKey;
	TLabel *Label1;
	TLabel *Label3;
	TEdit *edtTermKey;
	TButton *btnInfo;
	TRadioGroup *rdgVoice;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall edtKeyEnter(TObject *Sender);
	void __fastcall edtKeyExit(TObject *Sender);
	void __fastcall edtTermKeyEnter(TObject *Sender);
	void __fastcall edtTermKeyExit(TObject *Sender);
	void __fastcall btnInfoClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	void Load();
	void Save();
	void MemoResetStyle();

	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TComboBox *cbbGame;
	TEdit *edtKey;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *edtTermKey;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall edtKeyEnter(TObject *Sender);
	void __fastcall edtKeyExit(TObject *Sender);
	void __fastcall cbbGameChange(TObject *Sender);
	void __fastcall cbbGameKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall edtTermKeyEnter(TObject *Sender);
	void __fastcall edtTermKeyExit(TObject *Sender);
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

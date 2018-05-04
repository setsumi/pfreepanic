object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'pfree panic'
  ClientHeight = 162
  ClientWidth = 391
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  DesignSize = (
    391
    162)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 40
    Width = 163
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'PFree (Unlimited plays) toggle key'
  end
  object Label3: TLabel
    Left = 16
    Top = 68
    Width = 97
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Terminate game key'
  end
  object Memo1: TMemo
    Left = 8
    Top = 96
    Width = 375
    Height = 58
    Anchors = [akLeft, akTop, akRight, akBottom]
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    ExplicitHeight = 69
  end
  object edtKey: TEdit
    Left = 279
    Top = 37
    Width = 101
    Height = 21
    Anchors = [akTop, akRight]
    ReadOnly = True
    TabOrder = 1
    Text = 'Press single key'
    OnEnter = edtKeyEnter
    OnExit = edtKeyExit
  end
  object edtTermKey: TEdit
    Left = 279
    Top = 64
    Width = 101
    Height = 21
    Anchors = [akTop, akRight]
    ReadOnly = True
    TabOrder = 2
    Text = 'Press single key'
    OnEnter = edtTermKeyEnter
    OnExit = edtTermKeyExit
  end
  object btnInfo: TButton
    Left = 16
    Top = 8
    Width = 94
    Height = 25
    Caption = 'Information...'
    TabOrder = 3
    OnClick = btnInfoClick
  end
end

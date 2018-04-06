object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'pfree panic'
  ClientHeight = 173
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
    173)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 52
    Width = 178
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'PFree (Unlimited plays) toggle key'
    ExplicitWidth = 163
  end
  object Label2: TLabel
    Left = 16
    Top = 8
    Width = 88
    Height = 13
    Caption = 'Supported games:'
  end
  object Label3: TLabel
    Left = 16
    Top = 80
    Width = 112
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Terminate game key'
    ExplicitWidth = 97
  end
  object Label4: TLabel
    Left = 116
    Top = 8
    Width = 257
    Height = 13
    Caption = 'SOUND VOLTEX IV HEAVENLY HAVEN 1 (2017112800)'
  end
  object Label5: TLabel
    Left = 116
    Top = 23
    Width = 240
    Height = 13
    Caption = 'SOUND VOLTEX III GRAVITY WARS (2016121200)'
  end
  object Memo1: TMemo
    Left = 8
    Top = 108
    Width = 375
    Height = 57
    Anchors = [akLeft, akTop, akRight, akBottom]
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    ExplicitWidth = 360
    ExplicitHeight = 61
  end
  object edtKey: TEdit
    Left = 279
    Top = 49
    Width = 101
    Height = 21
    Anchors = [akTop, akRight]
    ReadOnly = True
    TabOrder = 1
    Text = 'Press single key'
    OnEnter = edtKeyEnter
    OnExit = edtKeyExit
    ExplicitLeft = 264
  end
  object edtTermKey: TEdit
    Left = 279
    Top = 76
    Width = 101
    Height = 21
    Anchors = [akTop, akRight]
    ReadOnly = True
    TabOrder = 2
    Text = 'Press single key'
    OnEnter = edtTermKeyEnter
    OnExit = edtTermKeyExit
    ExplicitLeft = 264
  end
end

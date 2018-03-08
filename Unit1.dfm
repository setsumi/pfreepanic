object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'pfree panic'
  ClientHeight = 177
  ClientWidth = 376
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  DesignSize = (
    376
    177)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 52
    Width = 193
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'PFree (Unlimited plays) toggle key'
  end
  object Label2: TLabel
    Left = 16
    Top = 15
    Width = 105
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Your game'
  end
  object Label3: TLabel
    Left = 16
    Top = 80
    Width = 193
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Terminate game key'
  end
  object Memo1: TMemo
    Left = 8
    Top = 108
    Width = 360
    Height = 61
    Anchors = [akLeft, akTop, akRight, akBottom]
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    ExplicitHeight = 132
  end
  object cbbGame: TComboBox
    Left = 136
    Top = 12
    Width = 229
    Height = 21
    Style = csDropDownList
    Anchors = [akTop, akRight]
    ItemHeight = 13
    TabOrder = 1
    OnChange = cbbGameChange
    OnKeyPress = cbbGameKeyPress
    Items.Strings = (
      'SOUND VOLTEX IV HEAVENLY HAVEN 1'
      'SOUND VOLTEX III GRAVITY WARS')
  end
  object edtKey: TEdit
    Left = 264
    Top = 49
    Width = 101
    Height = 21
    Anchors = [akTop, akRight]
    ReadOnly = True
    TabOrder = 2
    Text = 'Press single key'
    OnEnter = edtKeyEnter
    OnExit = edtKeyExit
  end
  object edtTermKey: TEdit
    Left = 264
    Top = 76
    Width = 101
    Height = 21
    Anchors = [akTop, akRight]
    ReadOnly = True
    TabOrder = 3
    Text = 'Press single key'
    OnEnter = edtTermKeyEnter
    OnExit = edtTermKeyExit
  end
end

object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 356
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
    356)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 40
    Width = 167
    Height = 13
    Caption = 'PFree (Unlimited plays) toggle key:'
  end
  object Label3: TLabel
    Left = 16
    Top = 67
    Width = 101
    Height = 13
    Caption = 'Terminate game key:'
  end
  object Label4: TLabel
    Left = 16
    Top = 94
    Width = 178
    Height = 13
    Caption = 'Take screenshot key (OSD required):'
  end
  object Memo1: TMemo
    Left = 8
    Top = 275
    Width = 375
    Height = 73
    Anchors = [akLeft, akTop, akRight, akBottom]
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object edtKey: TEdit
    Left = 201
    Top = 37
    Width = 101
    Height = 21
    ReadOnly = True
    TabOrder = 2
    Text = 'edtKey'
    OnEnter = edtKeyEnter
    OnExit = edtKeyExit
  end
  object edtTermKey: TEdit
    Left = 201
    Top = 64
    Width = 101
    Height = 21
    ReadOnly = True
    TabOrder = 3
    Text = 'edtTermKey'
    OnEnter = edtTermKeyEnter
    OnExit = edtTermKeyExit
  end
  object btnInfo: TButton
    Left = 16
    Top = 8
    Width = 94
    Height = 25
    Caption = 'Information...'
    TabOrder = 1
    OnClick = btnInfoClick
  end
  object GroupBox1: TGroupBox
    Left = 201
    Top = 119
    Width = 179
    Height = 78
    Caption = 'OSD'
    TabOrder = 4
    object group: TLabel
      Left = 24
      Top = 48
      Width = 45
      Height = 13
      Caption = 'Duration:'
    end
    object btnOSDTest: TButton
      Left = 120
      Top = 16
      Width = 47
      Height = 25
      Caption = 'Test'
      TabOrder = 0
      OnClick = btnOSDTestClick
    end
    object btnOSDHelp: TButton
      Left = 88
      Top = 16
      Width = 26
      Height = 25
      Caption = '?'
      TabOrder = 1
      OnClick = btnOSDHelpClick
    end
    object chkOSDEnabled: TCheckBox
      Left = 8
      Top = 20
      Width = 74
      Height = 17
      Caption = 'Enabled'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
    end
    object edtOSDDuration: TEdit
      Left = 87
      Top = 47
      Width = 54
      Height = 21
      TabOrder = 3
      Text = '0'
    end
    object udOSDDuration: TUpDown
      Left = 141
      Top = 47
      Width = 16
      Height = 21
      Associate = edtOSDDuration
      Max = 32000
      TabOrder = 4
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 119
    Width = 175
    Height = 78
    Caption = 'Notification voice'
    TabOrder = 5
    object chkVoiceEnabled: TCheckBox
      Left = 8
      Top = 20
      Width = 74
      Height = 17
      Caption = 'Enabled'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
    object rbVoiceEnglish: TRadioButton
      Left = 8
      Top = 48
      Width = 61
      Height = 17
      Caption = 'English'
      TabOrder = 1
    end
    object rbVoiceJapanese: TRadioButton
      Left = 84
      Top = 48
      Width = 71
      Height = 17
      Caption = 'Japanese'
      TabOrder = 2
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 200
    Width = 372
    Height = 69
    Caption = 'Screenshots - take (PNG) and notify (OSD) of new appearing'
    TabOrder = 6
    object Label2: TLabel
      Left = 16
      Top = 36
      Width = 34
      Height = 13
      Caption = 'Folder:'
    end
    object lblSSNstatus: TLabel
      Left = 16
      Top = 16
      Width = 59
      Height = 13
      Caption = 'lblSSNstatus'
    end
    object edtSSNpath: TEdit
      Left = 54
      Top = 35
      Width = 221
      Height = 21
      ReadOnly = True
      TabOrder = 0
      Text = 'edtSSNpath'
      OnChange = edtSSNpathChange
    end
    object btnSSNpath: TButton
      Left = 281
      Top = 34
      Width = 26
      Height = 25
      Caption = '...'
      TabOrder = 1
      OnClick = btnSSNpathClick
    end
    object btnSSNdisable: TButton
      Left = 313
      Top = 34
      Width = 47
      Height = 25
      Caption = 'Disable'
      TabOrder = 2
      OnClick = btnSSNdisableClick
    end
  end
  object edtScreenshotKey: TEdit
    Left = 201
    Top = 91
    Width = 101
    Height = 21
    TabOrder = 7
    Text = 'edtScreenshotKey'
    OnEnter = edtScreenshotKeyEnter
    OnExit = edtScreenshotKeyExit
  end
  object btnKeyDisable: TButton
    Left = 308
    Top = 36
    Width = 47
    Height = 25
    Caption = 'Disable'
    TabOrder = 8
    OnClick = btnKeyDisableClick
  end
  object btnTermKeyDisable: TButton
    Left = 308
    Top = 63
    Width = 47
    Height = 25
    Caption = 'Disable'
    TabOrder = 9
    OnClick = btnTermKeyDisableClick
  end
  object btnScreenshotKeyDisable: TButton
    Left = 308
    Top = 90
    Width = 47
    Height = 25
    Caption = 'Disable'
    TabOrder = 10
    OnClick = btnScreenshotKeyDisableClick
  end
end

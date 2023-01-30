#pragma once


// CPasswordSetDlg dialog
class CPasswordSetDlg : public ATL::CDialogImpl<CPasswordSetDlg>
{
	using BaseDialog = ATL::CDialogImpl<CPasswordSetDlg>;

public:
	CPasswordSetDlg();

	BEGIN_MSG_MAP(CPasswordSetDlg)
		COMMAND_ID_HANDLER(IDOK, OnOK)
	END_MSG_MAP()

	enum { IDD = IDD_PASSWORD_SET };

public:
	std::wstring m_Password;

protected:
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);	

};

// CPasswordAskDlg dialog
class CPasswordAskDlg : public ATL::CDialogImpl<CPasswordAskDlg>
{
	using BaseDialog = ATL::CDialogImpl<CPasswordAskDlg>;

public:
	CPasswordAskDlg();

	BEGIN_MSG_MAP(CPasswordAskDlg)
		COMMAND_ID_HANDLER(IDOK, OnOK)
	END_MSG_MAP()

	enum { IDD = IDD_PASSWORD_ASK };

public:
	std::wstring m_Password;

protected:
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

};


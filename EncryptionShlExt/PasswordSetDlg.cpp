// CPasswordSetDlg.cpp : implementation file
//

#include "pch.h"
#include "PasswordSetDlg.h"
#include "atlstr.h"

// CPasswordSetDlg dialog

CPasswordSetDlg::CPasswordSetDlg()
{

}

// CPasswordSetDlg message handlers
LRESULT CPasswordSetDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	WCHAR pass[255] = { 0 };
	WCHAR confirm[255] = { 0 };
	GetDlgItemText(IDC_PASS, pass, 255);
	GetDlgItemText(IDC_CONFIRM, confirm, 255);

	if (!wcslen(pass))
		return 0;

	if (wcscmp(pass, confirm))
	{
		MessageBox(L"Password and confirmation do not match", L"EncryptionShlExt", MB_ICONWARNING);
		return 0;
	}

	m_Password = pass;
	EndDialog(wID);
	return 0;
}

// CPasswordAskDlg dialog

CPasswordAskDlg::CPasswordAskDlg()
{

}

// CPasswordAskDlg message handlers
LRESULT CPasswordAskDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	WCHAR pass[255] = { 0 };
	GetDlgItemText(IDC_PASS, pass, 255);

	if (!wcslen(pass))
		return 0;

	m_Password = pass;
	EndDialog(wID);
	return 0;
}
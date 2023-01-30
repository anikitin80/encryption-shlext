// ShellExtension.cpp : Implementation of CShellExtension

#include "pch.h"
#include "ShellExtension.h"
#include <atlconv.h>  // for ATL string conversion macros
#include "Encryption.h"
#include "PasswordSetDlg.h"

// CShellExtension

STDMETHODIMP CShellExtension::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT,
				  -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;

	// Look for CF_HDROP data in the data object. If there
	// is no such data, return an error back to Explorer.
	if (FAILED(pDataObj->GetData(&fmt, &stg)))
		return E_INVALIDARG;

	// Get a pointer to the actual data.
	hDrop = (HDROP)GlobalLock(stg.hGlobal);

	// Make sure it worked.
	if (NULL == hDrop)
		return E_INVALIDARG;

	// Sanity check – make sure there is at least one filename.
	UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	HRESULT hr = S_OK;

	if (0 == uNumFiles)
	{
		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	// Get the name of the first file and store it in our member variable m_sFile
	TCHAR szFile[_MAX_PATH] = { 0 };
	if (0 == DragQueryFile(hDrop, 0, szFile, MAX_PATH))
		hr = E_INVALIDARG;

	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);

	m_sFilePath = szFile;

	// check if the file already encrypted
	if (m_sFilePath.substr(m_sFilePath.length() - 4) == FILE_EXT_ENCRYPTED)
		m_bEncrypt = false;

	return hr;
}

HRESULT CShellExtension::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	// show 'Encrypt' menu command or 'Decrypt' for already encrypted file
	InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, 
		uidFirstCmd, m_bEncrypt? _T("Encrypt with password") : _T("Decrypt with password"));

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
}

HRESULT CShellExtension::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	USES_CONVERSION;

	// Check idCmd, it must be 0 since we have only one menu item.
	if (0 != idCmd)
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if (uFlags & GCS_HELPTEXT)
	{
		LPCTSTR szText = _T("Encrypt/decrypt file with your password using AES method");

		if (uFlags & GCS_UNICODE)
		{
			// We need to cast pszName to a Unicode string, and then use the
			// Unicode string copy API.
			lstrcpynW((LPWSTR)pszName, T2CW(szText), cchMax);
		}
		else
		{
			// Use the ANSI string copy API to return the help string.
			lstrcpynA(pszName, T2CA(szText), cchMax);
		}

		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if (0 != HIWORD(pCmdInfo->lpVerb))
		return E_INVALIDARG;

	TCHAR szMsg[512] = { 0 };

	// Get the command index - the only valid one is 0.
	if (LOWORD(pCmdInfo->lpVerb) != 0)
		return E_INVALIDARG;

	CEncryption encryption;

	if (m_bEncrypt)
	{
		CPasswordSetDlg dlg;
		if (dlg.DoModal(pCmdInfo->hwnd) != IDOK)
			return S_OK;

		encryption.SetPassword(dlg.m_Password.c_str());
		if (!encryption.EncryptFile(m_sFilePath))
			wsprintf(szMsg, _T("Error occurred when encrypting file:\n\n%s"), m_sFilePath.c_str());
	}
	else
	{
		CPasswordAskDlg dlg;
		if (dlg.DoModal(pCmdInfo->hwnd) != IDOK)
			return S_OK;

		encryption.SetPassword(dlg.m_Password.c_str());
		if (!encryption.DecryptFile(m_sFilePath))
			wsprintf(szMsg, _T("Error occurred when decrypting file:\n\n%s"), m_sFilePath.c_str());
	}

	if(wcslen(szMsg))
		MessageBox(pCmdInfo->hwnd, szMsg, _T("EncryptionShlExt"), MB_ICONERROR);

	return S_OK;
}
// ShellExtension.cpp : Implementation of CShellExtension

#include "pch.h"
#include "ShellExtension.h"
#include <atlconv.h>  // for ATL string conversion macros
#include "Encryption.h"
#include "PasswordSetDlg.h"

// CShellExtension

HBITMAP CShellExtension::m_hbmpEncrypt = NULL;
HBITMAP CShellExtension::m_hbmpDecrypt = NULL;

CShellExtension::CShellExtension()
{
	// load bitmaps
    if(!m_hbmpEncrypt)
	    m_hbmpEncrypt = LoadBitmapFromIcon(_AtlBaseModule.m_hInst, MAKEINTRESOURCE(IDI_ENCRYPT));
    if(!m_hbmpDecrypt)
	    m_hbmpDecrypt = LoadBitmapFromIcon(_AtlBaseModule.m_hInst, MAKEINTRESOURCE(IDI_DECRYPT));
}

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
	SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_bEncrypt? m_hbmpEncrypt : m_hbmpDecrypt, NULL);

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
			wsprintf(szMsg, _T("Error occurred when encrypting file:\n%s\nError code: 0x%X"), m_sFilePath.c_str(), encryption.GetLastError());
	}
	else
	{
		CPasswordAskDlg dlg;
		if (dlg.DoModal(pCmdInfo->hwnd) != IDOK)
			return S_OK;

		encryption.SetPassword(dlg.m_Password.c_str());
        if (!encryption.DecryptFile(m_sFilePath))
        {
            DWORD dwLastError = encryption.GetLastError();
            if (dwLastError == NTE_BAD_DATA)
                wcscpy_s(szMsg, _T("Wrong password or invalid data format."));
            else
                wsprintf(szMsg, _T("Error occurred when decrypting file:\n%s\nError code: 0x%X"), m_sFilePath.c_str(), dwLastError);
        }
	}

	if(wcslen(szMsg))
		MessageBox(pCmdInfo->hwnd, szMsg, _T("EncryptionShlExt"), MB_ICONWARNING);

	return S_OK;
}

HBITMAP CShellExtension::LoadBitmapFromIcon(HMODULE hModule, LPCTSTR lpszResource)
{
	// Create DCs for drawing icon
	HDC hDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hDC);

	HBITMAP hResultBmp = NULL;

	BITMAPINFO bmi;        // bitmap header 
	VOID *pvBits;          // pointer to DIB section 
	ULONG   ulBitmapWidth, ulBitmapHeight;      // bitmap width/height 

	ulBitmapWidth = GetSystemMetrics(SM_CXSMICON);
	ulBitmapHeight = GetSystemMetrics(SM_CYSMICON);

	// zero the memory for the bitmap info 
	ZeroMemory(&bmi, sizeof(BITMAPINFO));

	// setup bitmap info  
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = ulBitmapWidth;
	bmi.bmiHeader.biHeight = ulBitmapHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = ulBitmapWidth * ulBitmapHeight * 4;

	// create our DIB section and select the bitmap into the dc 
	HBITMAP hMemBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
	HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

	HICON hIcon = (HICON)::LoadImage(hModule, lpszResource, IMAGE_ICON, ulBitmapWidth, ulBitmapHeight, 0);

	if (hIcon)
		DrawIconEx(hMemDC, 0, 0, hIcon, ulBitmapWidth, ulBitmapHeight, 0, NULL, DI_NORMAL);

	hResultBmp = hMemBmp;
	hMemBmp = NULL;

	SelectObject(hMemDC, hOrgBMP);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDC);
	DestroyIcon(hIcon);

	return hResultBmp;
}
#pragma once

#include <wincrypt.h>

#pragma comment (lib, "crypt32")

#define FILE_EXT_ENCRYPTED L".kre"

class CEncryption
{
public:

	CEncryption();
	~CEncryption();

	bool SetPassword(LPCWSTR pass);
	bool EncryptFile(const std::wstring& filePath);
	bool DecryptFile(const std::wstring& filePath);

    DWORD GetLastError() { return m_dwLastError; }

protected:
	HCRYPTPROV m_hProv = NULL;
	HCRYPTHASH m_hHash = NULL;
	HCRYPTKEY  m_hKey = NULL;
	DWORD m_dwLastError = 0;
};


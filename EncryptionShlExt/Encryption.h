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
	bool EncryptBuffer(const LPBYTE lpSource, DWORD dwSourceLen, std::vector<BYTE>& buffOutput);
	bool DecryptBuffer(const LPBYTE lpSource, DWORD dwSourceLen, std::vector<BYTE>& buffOutput);

	bool EncryptFile(const std::wstring& filePath);
	bool DecryptFile(const std::wstring& filePath);

protected:
	HCRYPTPROV m_hProv = NULL;
	HCRYPTHASH m_hHash = NULL;
	DWORD m_dwLastError = 0;
};


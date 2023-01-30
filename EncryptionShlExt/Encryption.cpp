#include "pch.h"
#include "Encryption.h"

#define ENCRYPT_BUFF_SIZE  8*1024 // 8 KB

CEncryption::CEncryption()
{
	if (!CryptAcquireContext(&m_hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		m_dwLastError = GetLastError();
}

CEncryption::~CEncryption()
{
	if (m_hHash != NULL)
		CryptDestroyHash(m_hHash);

	if (m_hProv != NULL)
		CryptReleaseContext(m_hProv, 0);
}

bool CEncryption::SetPassword(LPCWSTR pass)
{
	if (m_hHash != NULL)
		CryptDestroyHash(m_hHash);

	if (!CryptCreateHash(m_hProv, CALG_SHA_256, NULL, 0, &m_hHash))
	{
		m_dwLastError = GetLastError();
		return false;
	}

	if (!CryptHashData(m_hHash, (BYTE*)pass, static_cast<DWORD>(wcslen(pass) * sizeof(WCHAR)), 0))
	{
		m_dwLastError = GetLastError();
		return false;
	}
	
	return true;
}

bool CEncryption::EncryptBuffer(const LPBYTE lpSource, DWORD dwSourceLen, std::vector<BYTE>& buffOutput)
{
	bool bResult = false;

	HCRYPTKEY hKey = NULL;
	if (!CryptDeriveKey(m_hProv, CALG_AES_256, m_hHash, 0, &hKey))
	{
		m_dwLastError = GetLastError();
		return false;
	}

	// check if we have enough place for encryption
	DWORD dstLen = dwSourceLen;
	CryptEncrypt(hKey, NULL, TRUE, 0, NULL, &dstLen, 0);
	if(buffOutput.size() < dstLen)
		buffOutput.resize(dstLen);

	// copy source data and encrypt
	memcpy(buffOutput.data(), lpSource, dwSourceLen);
	dstLen = dwSourceLen;
	if (!CryptEncrypt(hKey, NULL, TRUE, 0, buffOutput.data(), &dstLen, static_cast<DWORD>(buffOutput.size())))
		m_dwLastError = GetLastError();
	else
		bResult = true;
			
	return bResult;
}

bool CEncryption::DecryptBuffer(const LPBYTE lpSource, DWORD dwSourceLen, std::vector<BYTE>& buffOutput)
{
	bool bResult = false;

	HCRYPTKEY hKey = NULL;
	if (!CryptDeriveKey(m_hProv, CALG_AES_256, m_hHash, 0, &hKey))
	{
		m_dwLastError = GetLastError();
		return false;
	}

	// check if we have enough place for decryption and copy source data
	DWORD dstLen = dwSourceLen;
	if (buffOutput.size() < dstLen)
		buffOutput.resize(dstLen);
	memcpy(buffOutput.data(), lpSource, dwSourceLen);

	// do decrypt
	if (!CryptDecrypt(hKey, NULL, TRUE, 0, buffOutput.data(), &dstLen))
		m_dwLastError = GetLastError();
	else
	{
		bResult = true;
		if(dstLen != dwSourceLen)
			buffOutput.resize(dstLen);
	}

	return bResult;
}

bool CEncryption::EncryptFile(const std::wstring& filePath)
{
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		m_dwLastError = GetLastError();
		return false;
	}

	std::wstring filePathEnc = filePath;
	filePathEnc += FILE_EXT_ENCRYPTED;

	HANDLE hFileEnc = CreateFile(filePathEnc.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFileEnc)
	{
		m_dwLastError = GetLastError();
		return false;
	}

	std::vector<BYTE> buffOutput;
	BYTE buff[ENCRYPT_BUFF_SIZE] = {0};
	DWORD dwRead = 0, dwWrite = 0;

	while (ReadFile(hFile, buff, ENCRYPT_BUFF_SIZE, &dwRead, NULL))
	{
		if (!EncryptBuffer(buff, dwRead, buffOutput))
			break;

		WriteFile(hFileEnc, buffOutput.data(), static_cast<DWORD>(buffOutput.size()), &dwWrite, NULL);
		if (dwRead < ENCRYPT_BUFF_SIZE)
			break;
	}

	CloseHandle(hFile);
	CloseHandle(hFileEnc);	

	if (m_dwLastError)
	{
		DeleteFile(filePathEnc.c_str());
		return false;
	}

	DeleteFile(filePath.c_str());
	return true;
}

bool CEncryption::DecryptFile(const std::wstring& filePath)
{
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		m_dwLastError = GetLastError();
		return false;
	}

	std::wstring filePathDec = filePath;
	if (filePathDec.substr(filePathDec.length() - 4) == FILE_EXT_ENCRYPTED)
		filePathDec.erase(filePathDec.length() - 4);
	else
	{
		m_dwLastError = ERROR_INVALID_PARAMETER;
		return false;
	}
	
	HANDLE hFileDec = CreateFile(filePathDec.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFileDec)
	{
		m_dwLastError = GetLastError();
		return false;
	}

	std::vector<BYTE> buffOutput;
	constexpr int buffSize = ENCRYPT_BUFF_SIZE;
	BYTE buff[buffSize] = { 0 };
	DWORD dwRead = 0, dwWrite = 0;

	while (ReadFile(hFile, buff, buffSize, &dwRead, NULL))
	{
		if (!DecryptBuffer(buff, dwRead, buffOutput))
			break;

		WriteFile(hFileDec, buffOutput.data(), static_cast<DWORD>(buffOutput.size()), &dwWrite, NULL);
		if (dwRead < buffSize)
			break;
	}

	CloseHandle(hFile);
	CloseHandle(hFileDec);

	if (m_dwLastError)
	{
		DeleteFile(filePathDec.c_str());
		return false;
	}

	DeleteFile(filePath.c_str());
	return true;
}
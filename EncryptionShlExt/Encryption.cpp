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
	if (m_hKey != NULL)
		CryptDestroyKey(m_hKey);

	if (m_hHash != NULL)
		CryptDestroyHash(m_hHash);

	if (m_hProv != NULL)
		CryptReleaseContext(m_hProv, 0);
}

bool CEncryption::SetPassword(LPCWSTR pass)
{
	if (m_hHash != NULL)
		CryptDestroyHash(m_hHash);
	if (m_hKey != NULL)
		CryptDestroyKey(m_hKey);

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

	if (!CryptDeriveKey(m_hProv, CALG_AES_256, m_hHash, 0, &m_hKey))
	{
		m_dwLastError = GetLastError();
		return false;
	}
	
	return true;
}

bool CEncryption::EncryptFile(const std::wstring& filePath)
{
	// open source file for reading
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		m_dwLastError = GetLastError();
		return false;
	}

	// open target file for writing
	std::wstring filePathEnc = filePath;
	filePathEnc += FILE_EXT_ENCRYPTED;

	HANDLE hFileEnc = CreateFile(filePathEnc.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFileEnc)
	{
		m_dwLastError = GetLastError();
		return false;
	}

	// allocate buffer
	DWORD dstLen = ENCRYPT_BUFF_SIZE;
	CryptEncrypt(m_hKey, NULL, TRUE, 0, NULL, &dstLen, 0);
	
	std::vector<BYTE> buff;
	buff.resize(dstLen);

	// read data from source file, encrypt and write to target file
	DWORD dwRead = 0, dwWrite = 0;
	while (ReadFile(hFile, buff.data(), ENCRYPT_BUFF_SIZE, &dwRead, NULL))
	{
		DWORD dwDataSize = dwRead;
		if (!CryptEncrypt(m_hKey, NULL, TRUE, 0, buff.data(), &dwDataSize, static_cast<DWORD>(buff.size())))
		{
			m_dwLastError = GetLastError();
			break;
		}

		WriteFile(hFileEnc, buff.data(), dwDataSize, &dwWrite, NULL);
		if (dwRead < ENCRYPT_BUFF_SIZE)
			break;
	}

	CloseHandle(hFile);
	CloseHandle(hFileEnc);	

	// remove target file if error occurred
	if (m_dwLastError)
	{
		DeleteFile(filePathEnc.c_str());
		return false;
	}

	// remove source file if encryption succeeded
	DeleteFile(filePath.c_str());
	return true;
}

bool CEncryption::DecryptFile(const std::wstring& filePath)
{
	// open source file for reading
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		m_dwLastError = GetLastError();
		return false;
	}

	// open target file for writing
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

	// allocate buffer
	DWORD dstLen = ENCRYPT_BUFF_SIZE;
	CryptEncrypt(m_hKey, NULL, TRUE, 0, NULL, &dstLen, 0);

	std::vector<BYTE> buff;
	buff.resize(dstLen);

	// read data from source file, decrypt and write to target file
	DWORD dwRead = 0, dwWrite = 0;
	while (ReadFile(hFile, buff.data(), dstLen, &dwRead, NULL))
	{
		DWORD dwDataSize = dwRead;
		if (!CryptDecrypt(m_hKey, NULL, TRUE, 0, buff.data(), &dwDataSize))
		{
			m_dwLastError = GetLastError();
			break;
		}

		WriteFile(hFileDec, buff.data(), dwDataSize, &dwWrite, NULL);
		if (dwRead < dstLen)
			break;
	}

	CloseHandle(hFile);
	CloseHandle(hFileDec);

	// remove target file if error occurred
	if (m_dwLastError)
	{
		DeleteFile(filePathDec.c_str());
		return false;
	}

	// remove source file if encryption succeeded
	DeleteFile(filePath.c_str());
	return true;
}
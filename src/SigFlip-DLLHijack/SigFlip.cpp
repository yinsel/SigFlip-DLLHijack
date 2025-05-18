#include "SigFlip.h"

/*
	读取签名中证书的保存的额外数据
*/
BYTE* SigFlipLoader(const char* filePath, DWORD* size) {
	HANDLE _fHandle = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (_fHandle == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	DWORD _fSize = GetFileSize(_fHandle, NULL);
	BYTE* _peBlob = (BYTE*)malloc(_fSize);
	DWORD _bytesRead = 0;
	ReadFile(_fHandle, _peBlob, _fSize, &_bytesRead, NULL);
	CloseHandle(_fHandle);

	if (_bytesRead == 0) {
		free(_peBlob);
		return NULL;
	}

	PIMAGE_DOS_HEADER _dosHeader = (PIMAGE_DOS_HEADER)_peBlob;
	if (_dosHeader->e_magic != 0x5A4D) {
		free(_peBlob);
		return NULL;
	}

	PIMAGE_NT_HEADERS _ntHeader = (PIMAGE_NT_HEADERS)(_peBlob + _dosHeader->e_lfanew);
	IMAGE_OPTIONAL_HEADER _optHeader = _ntHeader->OptionalHeader;

	DWORD _CertTableRVA = _optHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress;
	DWORD _CertTableSize = _optHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;

	if (_CertTableRVA == 0 || _CertTableSize == 0) {
		free(_peBlob);
		return NULL;
	}

	BYTE* _pePtr = _peBlob + _CertTableRVA;
	DWORD _dataOffset = 0;
	for (DWORD _index = 0; _index < _CertTableSize; _index++) {
		if (*(_pePtr + _index) == 0xFE && *(_pePtr + _index + 1) == 0xED &&
			*(_pePtr + _index + 2) == 0xFA && *(_pePtr + _index + 3) == 0xCE) {
			_dataOffset = _index + 8;
			break;
		}
	}

	if (_dataOffset == 0) {
		free(_peBlob);
		return NULL;
	}

	DWORD _encryptedDataSize = _CertTableSize - _dataOffset - 8;
	BYTE* extractedData = (BYTE*)VirtualAlloc(NULL, _encryptedDataSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memcpy(extractedData, _pePtr + _dataOffset, _encryptedDataSize);
	free(_peBlob);

	*size = _encryptedDataSize;

	return extractedData;
}

/*
	加解密函数 (可自定义)
*/
void decrypt(unsigned char* data, long dataLen, unsigned char* key, long keyLen, unsigned char* result) {
	unsigned char T[256];
	unsigned char S[256];
	unsigned char  tmp;
	int j = 0, t = 0, i = 0;


	for (int i = 0; i < 256; i++) {
		S[i] = i;
		T[i] = key[i % keyLen];
	}

	for (int i = 0; i < 256; i++) {
		j = (j + S[i] + T[i]) % 256;
		tmp = S[j];
		S[j] = S[i];
		S[i] = tmp;
	}
	j = 0;
	for (int x = 0; x < dataLen; x++) {
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;

		tmp = S[j];
		S[j] = S[i];
		S[i] = tmp;

		t = (S[i] + S[j]) % 256;

		result[x] = data[x] ^ S[t];
	}
}
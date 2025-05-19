#pragma once
#include<Windows.h>


struct ustring {
	DWORD Length;
	DWORD MaximumLength;
	PUCHAR Buffer;
};

typedef NTSTATUS(WINAPI* SystemFunction032Func)(struct ustring* data, const struct ustring* key);

BYTE* SigFlipLoader(const char* filePath, DWORD* size);
void decrypt(unsigned char* data, long dataLen, unsigned char* key, long keyLen, unsigned char* result);
VOID RC4(LPBYTE dwBytes, DWORD dwLength, LPSTR dwKey, DWORD dwKeySize);
#pragma once
#include<Windows.h>

BYTE* SigFlipLoader(const char* filePath, DWORD* size);
void decrypt(unsigned char* data, long dataLen, unsigned char* key, long keyLen, unsigned char* result);
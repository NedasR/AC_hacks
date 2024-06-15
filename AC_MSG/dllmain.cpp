#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include <iostream>
#include <fstream>
#include <iomanip>

volatile void __fastcall MagicFunction(char msg1[], char msg2[], char blabla[], unsigned int msgSize)
{
    std::ofstream myGameFile("C:\\Users\\nedne\\Documents\\messageFile.txt", std::ios::out | std::ios::app); // change file path

    myGameFile << msg2 << " Here" << std::endl;
    myGameFile.close();
}

volatile void __fastcall WritetoFileMSG(char msg1[], char msg2[],char blabla[], unsigned int msgSize)
{
    int a = 0;
    int b = 0;
    int c = 0;

    MagicFunction(msg1, msg2, blabla, msgSize);

    a = 3;
    b = 4;
    c = 5;
}

volatile void __fastcall CodeCave(char msg1[], char msg2[], char blabla[], unsigned int msgSize)
{
    __asm
    {
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
    }

    MagicFunction(msg1, msg2, blabla, msgSize);

    __asm
    {
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
    }
}


DWORD GetBaseAddress() {
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        std::cerr << "Failed to get module handle" << std::endl;
        return 0;
    }
    return (DWORD)hModule;
}

void ReadBytes(LPVOID sourceAbsoluteAddress, BYTE* buffer, size_t size) 
{
    DWORD Protect;
    if (VirtualProtect((LPVOID)sourceAbsoluteAddress, size, PAGE_EXECUTE_READWRITE, &Protect)) {
        
        memcpy(buffer, sourceAbsoluteAddress, size);

        VirtualProtect((LPVOID)sourceAbsoluteAddress, size, Protect, &Protect);
        std::cout << "Operation CopyBytes successfully!" << std::endl;
    }
}

void WriteBytes(LPVOID targetAbsoluteAddress, BYTE* buffer, size_t size)
{
    DWORD Protect;
    if (VirtualProtect((LPVOID)targetAbsoluteAddress, size, PAGE_EXECUTE_READWRITE, &Protect)) {

        memcpy((LPVOID)targetAbsoluteAddress, buffer, size);

        VirtualProtect((LPVOID)targetAbsoluteAddress, size, Protect, &Protect);
        std::cout << "Operation overwritten successfully!" << std::endl;
    }
}

void OverwriteOperation() {
    std::ofstream MyFile("C:\\Users\\nedne\\Documents\\Debug.txt", std::ios::out | std::ios::app); // change file path
    DWORD baseAddress = GetBaseAddress();
    if (baseAddress == 0) {
        std::cerr << "Failed to get base address" << std::endl;
        return;
    }

    DWORD targetjmpAddress = 0x004DAB62; // Injection point
    DWORD targetAddress = (DWORD)CodeCave - 5 - targetjmpAddress;// jmp instruction address 

    size_t size = 5;
    BYTE readFromButes[5];
    DWORD Target = (DWORD)CodeCave;
    ReadBytes((LPVOID)Target, readFromButes, size);

    for (size_t i = 0; i < size; ++i) {
        MyFile << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)readFromButes[i] << " ";
    }

    unsigned char newjmpBytes[5] = { 0xE9};

    DWORD IntermeidatejmpAddressPramater = (DWORD)(readFromButes[1] |
                                                   readFromButes[2] << 8 |
                                                   readFromButes[3] << 16 |
                                                   readFromButes[4] << 24);

    targetAddress = targetAddress + IntermeidatejmpAddressPramater + 5 + 35;

    newjmpBytes[4] = (targetAddress >> 24) & 0xFF; // Highest byte
    newjmpBytes[3] = (targetAddress >> 16) & 0xFF;
    newjmpBytes[2] = (targetAddress >> 8) & 0xFF;
    newjmpBytes[1] = targetAddress & 0xFF;

    targetAddress = targetAddress - 35;

    unsigned char newBytes[] = {
    0x90, 0x90, 0x50, 0x51, 0x52, 0x54, 0x9C, 0x90, 0x50, 0x50,
    0x90, 0x90, 0x90, 0x90, 0x90, 0xE8, 0x09, 0x01, 0x00, 0x00,
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x9D, 0x5C, 0x5A, 0x59,
    0x58, 0x90, 0x90, 0x90, 0x90, 0x90, 0xE9, 0x3F, 0x43, 0x19,
    0xFD
    };

    DWORD TargetWrtieTofile = targetjmpAddress + targetAddress + 35 + 5;
    DWORD ComebackToJmp = (targetAddress - (targetAddress * 2)) - 86;
    int sizeofNewBytes = sizeof(newBytes);
    newBytes[sizeofNewBytes - 1] = (ComebackToJmp >> 24) & 0xFF; // Highest byte
    newBytes[sizeofNewBytes - 2] = (ComebackToJmp >> 16) & 0xFF;
    newBytes[sizeofNewBytes - 3] = (ComebackToJmp >> 8) & 0xFF;
    newBytes[sizeofNewBytes - 4] = ComebackToJmp & 0xFF;

    MyFile << std::endl;
    MyFile << targetjmpAddress << " jmp address target" << std::endl;
    MyFile << targetAddress << " DLL address target" << std::endl;
    MyFile << baseAddress << " Base address target" << std::endl;

    WriteBytes((LPVOID)targetjmpAddress, newjmpBytes, sizeof(newjmpBytes));
    WriteBytes((LPVOID)TargetWrtieTofile, newBytes, sizeof(newBytes));

    MyFile.close();
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&OverwriteOperation, 0, 0, 0);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
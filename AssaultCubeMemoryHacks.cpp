#include <iostream>
#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>

DWORD GetModuleBaseAddress(TCHAR* ModuleName, DWORD PID)
{
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, ModuleName) == 0)
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));
    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

DWORD GetPointerAddress(HANDLE Handle, DWORD PID, DWORD GameBaseAddress, DWORD Address, std::vector<DWORD> Offsets)
{
    DWORD PointerAddress = NULL;
    ReadProcessMemory(Handle, (LPVOID*)(GameBaseAddress + Address), &PointerAddress, sizeof(PointerAddress), 0);
    for (int i = 0; i < Offsets.size() - 1; i++){
        ReadProcessMemory(Handle, (LPVOID*)(PointerAddress + Offsets.at(i)), &PointerAddress, sizeof(PointerAddress), 0);
    }
    return PointerAddress += Offsets.at(Offsets.size() - 1);
}

int main() {
    // GETS PROCESS ID OF ASSAULT CUBE
    DWORD PID = NULL; //  SET MANUALLY IF NEEDED
    HWND WindowHandle = FindWindow(NULL, "AssaultCube");
    GetWindowThreadProcessId(WindowHandle, &PID);
    if (PID == NULL) {
        std::cout << "COULDNT NOT FIND WINDOW";
        return 666;
    }

    // GETS WINDOW HANDLE BY OPENING PROCESS
    HANDLE Handle = NULL;
    Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (Handle == NULL) {
        std::cout << "COULDNT NOT OPEN PROCESS OR PID INVALID";
        return 666;
    }

    // GETS BASE ADDRESS TO ADD OFFSETS TO
    char GameModuleName1[] = "ac_client.exe";
    DWORD GameBaseAddress = GetModuleBaseAddress(_T(GameModuleName1), PID);
    DWORD Address = 0x00183828;

    // OFFSETS TO GET TO WANTED MEMORY ADDRESS
    std::vector<DWORD> SecondaryAmmoOffsets = { 0x8,0x928, 0x98, 0x30, 0x30, 0x660 };
    std::vector<DWORD> PrimaryAmmoOffsets = { 0x8,0x380,0x98,0x8F4 };
    std::vector<DWORD> HealthOffsets = { 0x8,0x250,0x98,0x758 };
    std::vector<DWORD> GrenadeCountOffsets = { 0x8,0xDE0,0x64,0x390 };
    std::vector<DWORD> ArmourOffsets = { 0x8,0xF80,0x98,0x30,0xA44 };
    DWORD SecondaryAmmoAddressPointer = GetPointerAddress(Handle, PID, GameBaseAddress, Address, SecondaryAmmoOffsets);
    DWORD PrimaryAmmoAddressPointer = GetPointerAddress(Handle, PID, GameBaseAddress, Address, PrimaryAmmoOffsets);
    DWORD HealthAddressPointer = GetPointerAddress(Handle, PID, GameBaseAddress, Address, HealthOffsets);
    DWORD GrenadeCountAddressPointer = GetPointerAddress(Handle, PID, GameBaseAddress, Address, GrenadeCountOffsets);
    DWORD ArmourAddressPointer = GetPointerAddress(Handle, PID, GameBaseAddress, Address, ArmourOffsets);

    // VALUES YOU CAN CUSTOMISE
    int PrimaryAmmoValue = 999;
    int SecondaryAmmoValue = 999;
    int HealthValue = 999;
    int GrenadeCountValue = 999;
    int ArmourValue = 999;

    // WRITES VALUES TO MEMORY ADDRESSES
    WriteProcessMemory(Handle, (LPVOID)SecondaryAmmoAddressPointer, &SecondaryAmmoValue, 4, 0);
    WriteProcessMemory(Handle, (LPVOID)PrimaryAmmoAddressPointer, &PrimaryAmmoValue, 4, 0);
    WriteProcessMemory(Handle, (LPVOID)HealthAddressPointer, &HealthValue, 4, 0);
    WriteProcessMemory(Handle, (LPVOID)GrenadeCountAddressPointer, &GrenadeCountValue, 4, 0);
    WriteProcessMemory(Handle, (LPVOID)ArmourAddressPointer, &ArmourValue, 4, 0);
    return 69;
}

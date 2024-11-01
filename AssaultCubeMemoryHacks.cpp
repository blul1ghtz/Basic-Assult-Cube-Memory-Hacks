#include <iostream>
#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>

DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, std::vector<DWORD> offsets)
{
    DWORD pID = NULL; // Game process ID
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    DWORD offset_null = NULL;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    DWORD pointeraddress = offset_null; // the address we need
    for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}

int main() {
    // GETS PROCESS ID OF ASSAULT CUBE
    HWND WindowHandle = FindWindow(NULL, "AssaultCube");
    DWORD PID = NULL;
    GetWindowThreadProcessId(WindowHandle, &PID);
    HANDLE Handle = NULL;
    Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    // GETS BASE ADDRESS TO ADD OFFSETS TO
    char GameModuleName1[] = "ac_client.exe";
    DWORD GameBaseAddress1 = GetModuleBaseAddress(_T(GameModuleName1), PID);
    DWORD Address = 0x00183828;
    // OFFSETS TO GET TO WANTED MEMORY ADDRESS
    std::vector<DWORD> SecondaryAmmoOffsets = { 0x8,0x928, 0x98, 0x30, 0x30, 0x660 };
    std::vector<DWORD> PrimaryAmmoOffsets = { 0x8,0x380,0x98,0x8F4 };
    std::vector<DWORD> HealthOffsets = { 0x8,0x250,0x98,0x758 };
    std::vector<DWORD> GrenadeCountOffsets = { 0x8,0xDE0,0x64,0x390 };
    DWORD SecondaryAmmoAddressPointer = GetPointerAddress(WindowHandle, GameBaseAddress1, Address, SecondaryAmmoOffsets);
    DWORD PrimaryAmmoAddressPointer = GetPointerAddress(WindowHandle, GameBaseAddress1, Address, PrimaryAmmoOffsets);
    DWORD HealthAddressPointer = GetPointerAddress(WindowHandle, GameBaseAddress1, Address, HealthOffsets);
    DWORD GrenadeCountAddressPointer = GetPointerAddress(WindowHandle, GameBaseAddress1, Address, GrenadeCountOffsets);
    // VALUES YOU CAN CUSTOMISE
    int PrimaryAmmoValue = 999;
    int SecondaryAmmoValue = 999;
    int HealthValue = 999;
    int GrenadeCountValue = 999;
    WriteProcessMemory(Handle, (LPVOID)SecondaryAmmoAddressPointer, &SecondaryAmmoValue, 4, 0);
    WriteProcessMemory(Handle, (LPVOID)PrimaryAmmoAddressPointer, &PrimaryAmmoValue, 4, 0);
    WriteProcessMemory(Handle, (LPVOID)HealthAddressPointer, &HealthValue, 4, 0);
    WriteProcessMemory(Handle, (LPVOID)GrenadeCountAddressPointer, &GrenadeCountValue, 4, 0);
    return 69;
}

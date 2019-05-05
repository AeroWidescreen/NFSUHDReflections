#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

DWORD WINAPI Thing(LPVOID);

bool HDReflections, ForceEnableMirror, RestoreSkybox;
static int ResolutionX, ResolutionY, ImproveReflectionLOD;
DWORD GameState;
HWND windowHandle;

DWORD VehicleLODCodeCaveExit = 0x570FF2;
DWORD FEVehicleLODCodeCaveExit = 0x570906;
DWORD RestoreSkyboxCodeCaveExit = 0x409789;
DWORD sub_571870 = 0x571870;
DWORD sub_40ED50 = 0x40ED50;
DWORD RoadReflectionLODCodeCaveExit = 0x445FEB;


void __declspec(naked) VehicleLODCodeCave()
{
	__asm {
		mov ecx, 0x0 // Road Reflection (Vehicle) LOD setting
		mov edx, 0x0 // Road Reflection (Vehicle) LOD setting
		jmp VehicleLODCodeCaveExit
	}
}

void __declspec(naked) FEVehicleLODCodeCave()
{
	__asm {
		mov eax, 0x0 // FE Road Reflection (Vehicle) LOD setting
		mov edx, 0x0 // FE Road Reflection (Vehicle) LOD setting
		cmp eax, edx
		mov ecx, 0x0 // FE Road Reflection (Wheels) LOD setting
		jmp FEVehicleLODCodeCaveExit
	}
}

void __declspec(naked) RoadReflectionLODCodeCave()
{
	__asm {
		mov dword ptr ds : [esp + 0x18], 0x04
		mov edx, dword ptr ds : [esi + ecx * 0x04 + 0x24]
		push edx
		mov edx, dword ptr ds : [edx]
		cmp dword ptr ds : [edx + 0x0C], 0x43535254 // Excludes drain walls
		je RoadReflectionLODCodeCave2
		cmp dword ptr ds : [edx + 0x0C], 0x535F5658 // Excludes truck
		je RoadReflectionLODCodeCave2
		pop edx
		mov dword ptr ds : [esp + 0x14], edx
		mov dword ptr ds : [esp + 0x18], ecx
		mov edi, dword ptr ds : [ebp + 0x08]
		jmp RoadReflectionLODCodeCaveExit

	RoadReflectionLODCodeCave2:
		pop edx
		jmp RoadReflectionLODCodeCaveExit
	}
}

void __declspec(naked) RestoreSkyboxCodeCave()
{
	__asm {
		mov ecx, 0x740580
		call sub_571870 // Skybox function call
		call sub_40ED50
		push 0x740580
		jmp RestoreSkyboxCodeCaveExit
	}
}

void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSUHDReflections.ini");

	// Resolution
	ResolutionX = iniReader.ReadInteger("RESOLUTION", "ResolutionX", 1920);
	ResolutionY = iniReader.ReadInteger("RESOLUTION", "ResolutionY", 1080);

	// General
	HDReflections = iniReader.ReadInteger("GENERAL", "HDReflections", 1);
	ImproveReflectionLOD = iniReader.ReadInteger("GENERAL", "ImproveReflectionLOD", 2);
	ForceEnableMirror = iniReader.ReadInteger("GENERAL", "ForceEnableMirror", 1);
	RestoreSkybox = iniReader.ReadInteger("GENERAL", "RestoreSkybox", 1);

	
	if (HDReflections)
	{
		// Road Reflection X
		injector::WriteMemory<uint32_t>(0x40A8BB, ResolutionX, true);
		injector::WriteMemory<uint32_t>(0x40A8F1, ResolutionX, true);
		injector::WriteMemory<uint32_t>(0x40854C, ResolutionX, true);
		injector::WriteMemory<uint32_t>(0x40AACA, ResolutionX, true);
		// Road Reflection Y
		injector::WriteMemory<uint32_t>(0x40A8B6, ResolutionY, true);
		injector::WriteMemory<uint32_t>(0x40A8EC, ResolutionY, true);
		injector::WriteMemory<uint32_t>(0x408553, ResolutionY, true);
		injector::WriteMemory<uint32_t>(0x40AAD1, ResolutionY, true);
		// Vehicle Reflection
		injector::WriteMemory<uint32_t>(0x702A84, ResolutionY, true);
		// RVM Reflection
		// Aspect ratio without RVM_MASK is 3:1
		injector::WriteMemory<uint32_t>(0x702A9C, ResolutionY, true);
		injector::WriteMemory<uint32_t>(0x702AA0, ResolutionY / 3, true);

	}

	if (ImproveReflectionLOD >= 1)
	{
		// Road Reflection (Vehicle) LOD
		injector::MakeJMP(0x570FEA, VehicleLODCodeCave, true);
		injector::MakeJMP(0x5708F0, FEVehicleLODCodeCave, true);
		// Vehicle Reflection LOD
		injector::WriteMemory<uint32_t>(0x408FEC, 0x00000000, true);
		// RVM Reflection LOD
		injector::WriteMemory<uint32_t>(0x408F94, 0x00000000, true);

		if (ImproveReflectionLOD >= 2)
		// Road Reflection LOD
		injector::MakeJMP(0x445F89, RoadReflectionLODCodeCave, true);
	}

	if (ForceEnableMirror)
	{
		// Enables mirror for all camera views
		injector::MakeNOP(0x4C1F43, 2, true); 
		// Enables mirror option for all camera views
		injector::MakeNOP(0x40843F, 2, true);
	}

	if (RestoreSkybox)
	{
		// Restores skybox for RVM
		injector::MakeJMP(0x409784, RestoreSkyboxCodeCave, true);
		// Extends render distance so skybox is visible
		injector::WriteMemory<uint32_t>(0x6B6CC0, 0x461C4000, true);
	}
}
	

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x670CB5) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
			Init();

		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.4 English speed.exe (3,03 MB (3.178.496 bytes)).", "NFSU HD Reflections", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;

}

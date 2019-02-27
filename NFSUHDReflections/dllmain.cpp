﻿#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

DWORD WINAPI Thing(LPVOID);

bool HDReflections, ForceEnableMirror;
static int ResolutionX, ResolutionY;
DWORD GameState;
HWND windowHandle;

DWORD ImproveReflectionLODCodeCaveExit = 0x570FF2;


void __declspec(naked) ImproveReflectionLODCodeCave()
{
	__asm {
		mov ecx, 0x0 // LOD setting
		mov edx, 0x0 // LOD setting
		jmp ImproveReflectionLODCodeCaveExit
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
	ForceEnableMirror = iniReader.ReadInteger("GENERAL", "ForceEnableMirror", 1);

	
	if (HDReflections)
	{
		// Jumps
		injector::MakeJMP(0x570FEA, ImproveReflectionLODCodeCave, true);
		// Reflection LOD
		injector::WriteMemory<uint8_t>(0x53E30B, 0xEB, true);
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
		// RVM Reflection X
		injector::WriteMemory<uint32_t>(0x702A9C, ResolutionX, true);
		// RVM Reflection Y
		injector::WriteMemory<uint32_t>(0x702AA0, ResolutionY, true);

	}

	if (ForceEnableMirror)
	{
		// Enables mirror for all camera views
		injector::MakeNOP(0x4C1F43, true); 
		injector::MakeNOP(0x4C1F44, true);
		// Enables mirror option for all camera views
		injector::MakeNOP(0x40843F, true);
		injector::MakeNOP(0x408440, true);
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
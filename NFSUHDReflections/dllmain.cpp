﻿#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

bool HDReflections, RealFrontEndReflections, ForceEnableMirror, DisableRoadReflection, ExtendTunnelDistance;
int ResolutionX, ResolutionY, ImproveReflectionLOD, RestoreSkybox;
int ResX, ResY;
float RoadScale, VehicleScale, MirrorScale, VehicleReflectionBrightness;
int TunnelDistance = 24;

DWORD VehicleLODCodeCaveExit = 0x570FF2;
DWORD FEVehicleLODCodeCaveExit = 0x570906;
DWORD RestoreMirrorSkyboxCodeCaveExit = 0x409789;
DWORD sub_571870 = 0x571870;
DWORD sub_40ED50 = 0x40ED50;
DWORD RoadReflectionLODCodeCaveExit = 0x445FEB;
DWORD RestoreVehicleSkyboxCodeCaveExit = 0x409714;
DWORD RestoreVehicleSkyboxCodeCaveECX;
DWORD RestoreRoadSkyboxCodeCaveExit = 0x4095EF;
DWORD RestoreRoadSkyboxCodeCaveECX;
DWORD ExtendVehicleRenderDistanceCodeCaveExit = 0x40B2F4;
DWORD AnimatedMirrorMaskFixCodeCaveExit = 0x40EC95;
DWORD ForceEnableMirrorCodeCave1Exit = 0x4C1F49;
DWORD ForceEnableMirrorCodeCave1DriftModeJump = 0x4C1FA6;
DWORD ForceEnableMirrorCodeCave2Exit = 0x408446;
DWORD ForceEnableMirrorCodeCave2DriftModeJump = 0x4084B1;


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
		je RoadReflectionLODCodeCavePart2
		cmp dword ptr ds : [edx + 0x0C], 0x535F5658 // Excludes truck
		je RoadReflectionLODCodeCavePart2
		cmp dword ptr ds : [edx + 0x08], 0x536222EF // Excludes bridge
		je RoadReflectionLODCodeCavePart2
		pop edx
		mov dword ptr ds : [esp + 0x14], edx
		mov dword ptr ds : [esp + 0x18], ecx
		mov edi, dword ptr ds : [ebp + 0x08]
		jmp RoadReflectionLODCodeCaveExit

	RoadReflectionLODCodeCavePart2:
		pop edx
		jmp RoadReflectionLODCodeCaveExit
	}
}

void __declspec(naked) ForceEnableMirrorCodeCave1()
{
	__asm {
		cmp dword ptr ds : [0x78A345] , 0x01
		je ForceEnableMirrorCodeCave1DriftMode
		cmp byte ptr ds : [eax + 0x24] , bl
		push 0x01
		push 0x03
		jmp ForceEnableMirrorCodeCave1Exit

		ForceEnableMirrorCodeCave1DriftMode :
		jmp ForceEnableMirrorCodeCave1DriftModeJump

	}
}

void __declspec(naked) ForceEnableMirrorCodeCave2()
{
	__asm {
		mov eax, dword ptr ds : [0x536610] // 0078A344 (0078A334 = EU)
		add eax, 0x01
		cmp byte ptr ds : [eax], 0x01
		je ForceEnableMirrorCodeCave2DriftMode
		mov eax, dword ptr ds : [0x740504]
		jmp ForceEnableMirrorCodeCave2Exit

		ForceEnableMirrorCodeCave2DriftMode :
		jmp ForceEnableMirrorCodeCave2DriftModeJump
	}
}

void __declspec(naked) RestoreMirrorSkyboxCodeCave()
{
	__asm {
		mov ecx, dword ptr ds : [0x4097F4] // 00740580 (00750570 = EU)
		call sub_571870 // Skybox function call
		call sub_40ED50
		mov ecx, dword ptr ds : [0x4097F4] // 00740580 (00750570 = EU)
		push ecx
		jmp RestoreMirrorSkyboxCodeCaveExit
	}
}

void __declspec(naked) RestoreVehicleSkyboxCodeCave()
{
	__asm {
		mov dword ptr ds : [RestoreVehicleSkyboxCodeCaveECX], ecx
		mov ecx, ebx // 00740820 (00740810 = EU)
		call sub_571870
		call sub_40ED50
		mov ecx, dword ptr ds : [RestoreVehicleSkyboxCodeCaveECX]
		push ebx
		xor ecx, ecx
		lea edx, dword ptr ds : [esp + 0x24]
		jmp RestoreVehicleSkyboxCodeCaveExit
	}
}

void __declspec(naked) RestoreRoadSkyboxCodeCave()
{
	__asm {
		mov dword ptr ds : [RestoreRoadSkyboxCodeCaveECX], ecx
		mov ecx, dword ptr ds : [0x409658] // 007405D0 (007505E0 = EU)
		call sub_571870
		call sub_40ED50
		mov ecx, dword ptr ds : [0x409658] // 007405D0 (007505E0 = EU)
		push ecx
		mov ecx, dword ptr ds : [RestoreRoadSkyboxCodeCaveECX]
		jmp RestoreRoadSkyboxCodeCaveExit
	}
}

void __declspec(naked) ExtendVehicleRenderDistanceCodeCave()
{
	__asm {
		mov ecx, dword ptr ds : [0x44976C]
		cmp dword ptr ds : [ecx], 0x03
		je ExtendVehicleRenderDistanceCodeCaveFrontEnd
		mov esi, 0x461C4000

	ExtendVehicleRenderDistanceCodeCaveFrontEnd:
		mov dword ptr ds : [eax + 0xC0], esi
		jmp ExtendVehicleRenderDistanceCodeCaveExit
	}
}

void __declspec(naked) AnimatedMirrorMaskFixCodeCave()
{
	__asm {
		cmp dword ptr ds : [esi + 0x0C], 0x5F4E5254 // "TRN_"
		jne AnimatedMirrorMaskFixCodeCavePart2
		cmp dword ptr ds : [esi + 0x10], 0x45544157 // "WATE"
		jne AnimatedMirrorMaskFixCodeCavePart2
		cmp dword ptr ds : [esi + 0x14], 0x4C414652 // "RFAL"
		jne AnimatedMirrorMaskFixCodeCavePart2
		cmp byte ptr ds : [esi + 0x54], 0x01 // Checks if the address has the correct value
		jne AnimatedMirrorMaskFixCodeCavePart2
		mov byte ptr ds : [esi + 0x54], 0x00 // Disables mirror mask animation

	AnimatedMirrorMaskFixCodeCavePart2:
		push ecx
		movsx eax, byte ptr ds : [esi + 0x52]
		jmp AnimatedMirrorMaskFixCodeCaveExit
	}
}

void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSUHDReflections.ini");

	// Resolution
	ResX = iniReader.ReadInteger("RESOLUTION", "ResolutionX", 0);
	ResY = iniReader.ReadInteger("RESOLUTION", "ResolutionY", 0);
	RoadScale = iniReader.ReadFloat("RESOLUTION", "RoadScale", 1.0);
	VehicleScale = iniReader.ReadFloat("RESOLUTION", "VehicleScale", 1.0);
	MirrorScale = iniReader.ReadFloat("RESOLUTION", "MirrorScale", 1.0);

	// General
	HDReflections = iniReader.ReadInteger("GENERAL", "HDReflections", 1);
	ImproveReflectionLOD = iniReader.ReadInteger("GENERAL", "ImproveReflectionLOD", 2);
	RealFrontEndReflections = iniReader.ReadInteger("GENERAL", "RealFrontEndReflections", 0);
	ForceEnableMirror = iniReader.ReadInteger("GENERAL", "ForceEnableMirror", 1);
	RestoreSkybox = iniReader.ReadInteger("GENERAL", "RestoreSkybox", 1);
	DisableRoadReflection = iniReader.ReadInteger("GENERAL", "DisableRoadReflection", 1);
	VehicleReflectionBrightness = iniReader.ReadFloat("GENERAL", "VehicleReflectionBrightness", 1.0);

	// Extra
	ExtendTunnelDistance = iniReader.ReadInteger("EXTRA", "ExtendTunnelDistance", 1);

	if (ResX <= 0 || ResY <= 0)
	{
		ResX = ::GetSystemMetrics(SM_CXSCREEN);
		ResY = ::GetSystemMetrics(SM_CYSCREEN);
	}

	if (HDReflections)
	{
		// Road Reflection X
		injector::WriteMemory<uint32_t>(0x40A8BB, ResX * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x40A8F1, ResX * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x40854C, ResX * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x40AACA, ResX * RoadScale, true);
		// Road Reflection Y
		injector::WriteMemory<uint32_t>(0x40A8B6, ResY * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x40A8EC, ResY * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x408553, ResY * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x40AAD1, ResY * RoadScale, true);
		// Vehicle Reflection
		injector::WriteMemory<uint32_t>(0x702A84, ResY * VehicleScale, true);
		// RVM Reflection
		// Aspect ratio without RVM_MASK is 3:1
		injector::WriteMemory<uint32_t>(0x702A9C, ResY * MirrorScale, true);
		injector::WriteMemory<uint32_t>(0x702AA0, (ResY / 3) * MirrorScale, true);
	}

	if (ImproveReflectionLOD >= 1)
	{
		// Fixes moving RVM mask
		injector::MakeJMP(0x40EC90, AnimatedMirrorMaskFixCodeCave, true);
		// Road Reflection (Vehicle) LOD
		injector::MakeJMP(0x570FEA, VehicleLODCodeCave, true);
		injector::MakeJMP(0x5708F0, FEVehicleLODCodeCave, true);
		// Vehicle Reflection LOD
		injector::WriteMemory<uint32_t>(0x408FEC, 0x00006002, true);
		// RVM Reflection LOD
		injector::WriteMemory<uint32_t>(0x408F94, 0x00006002, true);

		if (ImproveReflectionLOD >= 2)
		// Road Reflection LOD
		injector::MakeJMP(0x445F89, RoadReflectionLODCodeCave, true);
	}

	if (RealFrontEndReflections)
	{
		injector::WriteMemory<uint32_t>(0x4AE59F, 0x00000000, true);
	}

	if (ForceEnableMirror)
	{
		// Enables mirror for all camera views
		injector::MakeJMP(0x4C1F43, ForceEnableMirrorCodeCave1, true);
		// Enables mirror option for all camera views
		injector::MakeJMP(0x40843F, ForceEnableMirrorCodeCave2, true);
	}

	if (RestoreSkybox)
	{
		// Restores skybox for RVM
		injector::MakeJMP(0x409784, RestoreMirrorSkyboxCodeCave, true);
		// Extends RVM distance so skybox is visible
		injector::WriteMemory<uint32_t>(0x6B6CC0, 0x461C4000, true);
		// Restores skybox for vehicle reflection
		injector::MakeJMP(0x40970D, RestoreVehicleSkyboxCodeCave, true);
		// Extends vehicle reflection render distance so skybox is visible
		injector::MakeJMP(0x40B2EC, ExtendVehicleRenderDistanceCodeCave, true);
		// Restores skybox for road reflection
		injector::MakeJMP(0x4095EA, RestoreRoadSkyboxCodeCave, true);
		// Enables skybox
		injector::MakeNOP(0x57187B, 2, true);
	}

	if (DisableRoadReflection)
	{
		// Disables road reflection that appears in vehicle and mirror
		injector::MakeNOP(0x40C95D, 2, true);
	}

	if (ExtendTunnelDistance)
	{
		// Main
		injector::WriteMemory(0x4AE5BE, &TunnelDistance, true);
		// Road Reflection
		injector::WriteMemory(0x4AE57E, &TunnelDistance, true);
		// Vehicle Reflection
		injector::WriteMemory(0x4AE59A, &TunnelDistance, true);
	}

	if (VehicleReflectionBrightness)
	{
		static float VehicleReflectionIntensity = (0.5f * VehicleReflectionBrightness);
		injector::WriteMemory(0x40FBB9, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x40FBCF, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x40FBE5, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x40FBFB, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x40FC20, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x40FC3A, &VehicleReflectionIntensity, true);
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
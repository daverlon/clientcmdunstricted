#include "sdk.h"

#include <iostream>
#include <Windows.h>

namespace g
{
	HINSTANCE instance;
}

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);

void* GetSDKInstancebyFindingInterfaces(const char* szInterfaceName, const char* interfaceHandle)
{
	char buffer[0xFF];

	do
	{
		Sleep(1);

	} while (!GetModuleHandleA(interfaceHandle));

	CreateInterfaceFn Interface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(interfaceHandle), "CreateInterface");

	int index = 0xFF;
	do
	{
		if (index <= 10) { sprintf_s(buffer, "%s00%i", szInterfaceName, index); }
		if (index >= 10) { sprintf_s(buffer, "%s0%i", szInterfaceName, index); }
		if (index >= 100) { sprintf_s(buffer, "%s%i", szInterfaceName, index); }

		if (Interface(buffer, 0))
		{
			std::cout << "Found interface handle " << buffer << std::endl;
			break;
		}

		index--;

	} while (index >= 0);

	void* rendertmp = reinterpret_cast<void*>(Interface(buffer, 0));

	return rendertmp;
}

void quit()
{
	std::cout << "Quitting" << std::endl;
	FreeConsole();
	FreeLibraryAndExitThread(g::instance, 0);
}

int main()
{
	IVEngineClient* engineClient = (IVEngineClient*)GetSDKInstancebyFindingInterfaces("VEngineClient", "engine");
	if (engineClient == nullptr)
	{
		std::cout << "Unable to get VEngineClient interface" << std::endl;
		quit();
		return 0;
	}

	std::cout << "Loaded!" << std::endl;

	while (!GetAsyncKeyState(VK_F1))
	{
		if (GetAsyncKeyState(VK_F2) & 1)
		{ 
			engineClient->ClientCmd_Unrestricted("say hello\x0D\x0D\x0D\x0D\x0Dworld");
		}
		Sleep(500);
	}

	quit();
	return 0;
}

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		g::instance = hinstDLL;

		DisableThreadLibraryCalls(hinstDLL);

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
	}

	return true;
}

# HookLib
## Wtf is HookLib anyway?

HookLib is a utility library for memory hacking and function detouring.
The library still in development, the x64 support will come soon.
Win32 only.

## How to use it?

To begin with, you must initialize HookLib with `HookLib::Initialize()` function.
If you're done using HookLib, you must shutdown HookLib with `HookLib::Shutdown()` function.

A simple HookLib example with DLL
```cpp
#include "HookLib.h"

unsigned __stdcall Main()
{
  // Do stuff with HookLib
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
  switch(fdwReason)
  {
  case DLL_PROCESS_ATTACH:
    // Initialize HookLib
	  HookLib::Initialize();
	  Main();
	  break;
  case DLL_THREAD_ATTACH:
	  break;
  case DLL_PROCESS_DETACH:
    // If you're done using HookLib, then...
	  HookLib::Shutdown();
	  break;
  case DLL_THREAD_DETACH:
	  break;
  default:
	  break;
  }
}
```

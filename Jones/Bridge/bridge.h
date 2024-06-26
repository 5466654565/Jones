// bridge.h
#pragma once

#ifdef BRIDGE_EXPORTS
#define BRIDGE_API __declspec(dllexport)
#else
#define BRIDGE_API __declspec(dllimport)
#endif

extern "C" {
	BRIDGE_API bool SetStart(bool value);
	BRIDGE_API bool GetStart();
	BRIDGE_API bool SetPause(bool value);
	BRIDGE_API bool GetPause();
	BRIDGE_API bool SetStop(bool value);
	BRIDGE_API bool GetStop();
	BRIDGE_API bool SetExit(bool value);
	BRIDGE_API bool GetExit();

	BRIDGE_API int GetProgressData();
	BRIDGE_API void SetProgressData(int value);
	BRIDGE_API int GetProgressMetaData();
	BRIDGE_API void SetProgressMetaData(int value);
}

extern int currentProgress;
#ifndef DESMUME_FRONTEND_WINDOWS_PERFPROFILER_H
#define DESMUME_FRONTEND_WINDOWS_PERFPROFILER_H

#include "types.h"

enum WinPerfCounterId
{
	WinPerfCounter_RunLoopCore = 0,
	WinPerfCounter_RunLoopUser,
	WinPerfCounter_RunLoopPreFrameWait,
	WinPerfCounter_RunLoopPostFrameAccounting,
	WinPerfCounter_NDSExec,
	WinPerfCounter_ArmInnerLoop,
	WinPerfCounter_SPUEmulateUser,
	WinPerfCounter_Display,
	WinPerfCounter_AudioUpdate,
	WinPerfCounter_Count
};

#if defined(DESMUME_WIN_PERF_PROFILING)
u64 WinPerfNowTicks();
void WinPerfBeginFrame();
void WinPerfEndFrame();
void WinPerfMarkDuration(WinPerfCounterId counterId, u64 durationTicks);
void WinPerfAddAudioUpdate(u64 durationTicks, u32 sampleCount, u32 queuedSampleCount);
void WinPerfAddAudioUnderrun();
void WinPerfSetEmuLockTicks(u64 durationTicks);
#else
static FORCEINLINE u64 WinPerfNowTicks() { return 0; }
static FORCEINLINE void WinPerfBeginFrame() {}
static FORCEINLINE void WinPerfEndFrame() {}
static FORCEINLINE void WinPerfMarkDuration(WinPerfCounterId, u64) {}
static FORCEINLINE void WinPerfAddAudioUpdate(u64, u32, u32) {}
static FORCEINLINE void WinPerfAddAudioUnderrun() {}
static FORCEINLINE void WinPerfSetEmuLockTicks(u64) {}
#endif

#endif

//THIS SPEED THROTTLE WAS TAKEN FROM FCEUX.
//Copyright (C) 2002 Xodnizel
//(the code might look quite different by now, though...)

/*
	Many Modifications Copyright (C) 2009-2016 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "throttle.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdlib.h>

#include "types.h"
#include "debug.h"
#include "console.h"
#include "driver.h"
#include "NDSSystem.h"
#include "winutil.h"

int FastForward=0;
static u64 tmethod,tfreq,afsfreq;
static HANDLE throttleWaitableTimer = NULL;
static const u64 core_desiredfps = 3920763; //59.8261
static u64 ltime;
static const double maxNormalFpsTarget = 1000.0;
static const char *normalFpsTargetIniKey = "Normal FPS Target";
static const int defaultFpsScalerIndex = 5;
static int desiredFpsScalerIndex = GetPrivateProfileInt("Video","FPS Scaler Index", defaultFpsScalerIndex, IniName);
static int fastForwardFpsCap = GetPrivateProfileInt("Video", "Fast Forward FPS Cap", 0, IniName);
static u64 desiredFpsScalers [] = {
	1024,
	512, // 200%
	448, // 175%
	384, // 150%
	320, // 125%
	256, // 100%
	192,
	128, // 50%
	96,
	64, // 25%
	42,
	32,
	16,
};
static u64 desiredfps = core_desiredfps;
static float desiredspf = 65536.0f / core_desiredfps;

static double FpsFixedToDouble(u64 fpsFixed)
{
	return (double)fpsFixed / 65536.0;
}

static u64 FpsDoubleToFixed(double fps)
{
	return (u64)(fps * 65536.0 + 0.5);
}

static bool TryParseNormalFpsText(const char *text, double &fps)
{
	if (text == NULL || *text == '\0')
	{
		return false;
	}

	char *end = NULL;
	fps = strtod(text, &end);
	if (end == text)
	{
		return false;
	}

	while (*end != '\0' && isspace((unsigned char)*end))
	{
		end++;
	}

	return *end == '\0' && fps > 0.0 && fps <= maxNormalFpsTarget;
}

static u64 LoadNormalFpsTarget()
{
	char text[32] = {0};
	GetPrivateProfileString("Video", normalFpsTargetIniKey, "", text, sizeof(text), IniName);

	double configuredFps = 0.0;
	if (!TryParseNormalFpsText(text, configuredFps))
	{
		return core_desiredfps;
	}

	return FpsDoubleToFixed(configuredFps);
}

static u64 normalFpsTarget = LoadNormalFpsTarget();

static void UpdateDesiredFps()
{
	const u64 desiredFpsScaler = desiredFpsScalers[desiredFpsScalerIndex];
	desiredfps = (normalFpsTarget * desiredFpsScaler) / 256;
	if (desiredfps == 0)
	{
		desiredfps = 1;
	}

	desiredspf = 65536.0f / desiredfps;
}

static void PersistNormalFpsTarget()
{
	if (normalFpsTarget == core_desiredfps)
	{
		WritePrivateProfileString("Video", normalFpsTargetIniKey, NULL, IniName);
		return;
	}

	char text[32] = {0};
	sprintf(text, "%.3f", FpsFixedToDouble(normalFpsTarget));
	WritePrivateProfileString("Video", normalFpsTargetIniKey, text, IniName);
}

double GetNormalFpsTarget()
{
	return FpsFixedToDouble(normalFpsTarget);
}

double GetCurrentThrottleTargetFps()
{
	return FpsFixedToDouble(desiredfps);
}

void SetNormalFpsTarget(double fpsTarget)
{
	normalFpsTarget = (fpsTarget <= 0.0) ? core_desiredfps : FpsDoubleToFixed(fpsTarget);
	desiredFpsScalerIndex = defaultFpsScalerIndex;
	FrameLimit = true;
	PersistNormalFpsTarget();
	WritePrivateProfileInt("Video","FPS Scaler Index", desiredFpsScalerIndex, IniName);
	WritePrivateProfileInt("FrameLimit", "FrameLimit", FrameLimit, IniName);
	UpdateDesiredFps();
	AutoFrameSkip_IgnorePreviousDelay();

	if (normalFpsTarget == core_desiredfps)
	{
		driver->AddLine("Normal FPS target reset to %.03f", GetNormalFpsTarget());
	}
	else
	{
		driver->AddLine("Normal FPS target set to %.03f", GetNormalFpsTarget());
	}
}

void IncreaseSpeed(void) {

	if(desiredFpsScalerIndex)
		desiredFpsScalerIndex--;
	u64 desiredFpsScaler = desiredFpsScalers[desiredFpsScalerIndex];
	UpdateDesiredFps();
	printf("Throttle fps scaling increased to: %f\n",desiredFpsScaler/256.0);
	driver->AddLine("Target FPS: %.03f", GetCurrentThrottleTargetFps());
	WritePrivateProfileInt("Video","FPS Scaler Index", desiredFpsScalerIndex, IniName);
}

int GetFastForwardFpsCap()
{
	return fastForwardFpsCap;
}

bool HasFastForwardFpsCap()
{
	return fastForwardFpsCap > 0;
}

void SetFastForwardFpsCap(int fpsCap)
{
	if (fpsCap < 0) {
		fpsCap = 0;
	}

	fastForwardFpsCap = fpsCap;
	WritePrivateProfileInt("Video", "Fast Forward FPS Cap", fastForwardFpsCap, IniName);

	if (fastForwardFpsCap > 0) {
		driver->AddLine("Fast forward FPS cap set to %d", fastForwardFpsCap);
	} else {
		driver->AddLine("Fast forward FPS cap disabled");
	}
}

void DecreaseSpeed(void) {

	if(desiredFpsScalerIndex != sizeof(desiredFpsScalers)/sizeof(desiredFpsScalers[0]) - 1)
		desiredFpsScalerIndex++;
	u64 desiredFpsScaler = desiredFpsScalers[desiredFpsScalerIndex];
	UpdateDesiredFps();
	printf("Throttle fps scaling decreased to: %f\n",desiredFpsScaler/256.0);
	driver->AddLine("Target FPS: %.03f", GetCurrentThrottleTargetFps());
	WritePrivateProfileInt("Video","FPS Scaler Index", desiredFpsScalerIndex, IniName);
}

static u64 GetCurTime(void)
{
	if(tmethod)
	{
		u64 tmp;
		QueryPerformanceCounter((LARGE_INTEGER*)&tmp);
		return tmp;
	}
	else
	{
		return (u64)GetTickCount();
	}
}

void InitSpeedThrottle(void)
{
	UpdateDesiredFps();
	tmethod=0;
	if(QueryPerformanceFrequency((LARGE_INTEGER*)&afsfreq))
		tmethod=1;
	else
		afsfreq=1000;
	tfreq = afsfreq << 16;
	ltime = GetCurTime();
	if (throttleWaitableTimer == NULL)
		throttleWaitableTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	AutoFrameSkip_IgnorePreviousDelay();
}

void ShutdownSpeedThrottle(void)
{
	if (throttleWaitableTimer != NULL)
	{
		CloseHandle(throttleWaitableTimer);
		throttleWaitableTimer = NULL;
	}
}

static void AutoFrameSkip_BeforeThrottle();

static void WaitUntilTargetTicks(u64 targetTicks)
{
	const u64 spinThresholdTicks = std::max<u64>(1, afsfreq / 2000); // 0.5ms
	const u64 yieldThresholdTicks = std::max<u64>(1, afsfreq / 10000); // 0.1ms

	for (;;)
	{
		u64 nowTicks = GetCurTime();
		if (nowTicks >= targetTicks)
		{
			return;
		}

		const u64 remainingTicks = targetTicks - nowTicks;
		if (tmethod && throttleWaitableTimer != NULL && remainingTicks > spinThresholdTicks)
		{
			const u64 sleepTicks = remainingTicks - spinThresholdTicks;
			LARGE_INTEGER dueTime;
			dueTime.QuadPart = -(LONGLONG)((sleepTicks * 10000000ULL) / afsfreq);
			if (dueTime.QuadPart != 0 && SetWaitableTimer(throttleWaitableTimer, &dueTime, 0, NULL, NULL, FALSE))
			{
				WaitForSingleObject(throttleWaitableTimer, INFINITE);
				continue;
			}
		}

		if (!tmethod)
		{
			const DWORD remainingMs = (DWORD)((remainingTicks * 1000ULL) / afsfreq);
			if (remainingMs > 1)
			{
				Sleep(remainingMs - 1);
				continue;
			}
		}

		if (remainingTicks > yieldThresholdTicks)
			SwitchToThread();
		else
			YieldProcessor();
	}
}

void SpeedThrottle()
{
	AutoFrameSkip_BeforeThrottle();

	const u64 throttleFps = (FastForward && HasFastForwardFpsCap())
		? (static_cast<u64>(fastForwardFpsCap) << 16)
		: desiredfps;
	if(FastForward && !HasFastForwardFpsCap())
		return;

	const u64 frameTicks = tfreq / throttleFps;
	const u64 targetTicks = ltime + frameTicks;
	u64 ttime = GetCurTime();
	if (ttime < targetTicks)
	{
		WaitUntilTargetTicks(targetTicks);
		ttime = GetCurTime();
	}
	if( (ttime-ltime) >= (tfreq*4/throttleFps))
		ltime=ttime;
	else
		ltime+=frameTicks;
}


// auto frameskip

static u64 beginticks=0, endticks=0, preThrottleEndticks=0;
static float fSkipFrames = 0;
static float fSkipFramesError = 0;
static int lastSkip = 0;
static float lastError = 0;
static float integral = 0;

void AutoFrameSkip_IgnorePreviousDelay()
{
	beginticks = GetCurTime();

	// this seems to be a stable way of allowing the skip frames to
	// quickly adjust to a faster environment (e.g. after a loadstate)
	// without causing oscillation or a sudden change in skip rate
	fSkipFrames *= 0.5f;
}

static void AutoFrameSkip_BeforeThrottle()
{
	preThrottleEndticks = GetCurTime();
}

void AutoFrameSkip_NextFrame()
{
	endticks = GetCurTime();

	// calculate time since last frame
	u64 diffticks = endticks - beginticks;
	float diff = (float)diffticks / afsfreq;

	// calculate time since last frame not including throttle sleep time
	if(!preThrottleEndticks) // if we didn't throttle, use the non-throttle time
		preThrottleEndticks = endticks;
	u64 diffticksUnthrottled = preThrottleEndticks - beginticks;
	float diffUnthrottled = (float)diffticksUnthrottled / afsfreq;


	float error = diffUnthrottled - desiredspf;


	// reset way-out-of-range values
	if(diff > 1)
		diff = 1;
	if(error > 1 || error < -1)
		error = 0;
	if(diffUnthrottled > 1)
		diffUnthrottled = desiredspf;

	float derivative = (error - lastError) / diff;
	lastError = error;

	integral = integral + (error * diff);
	integral *= 0.99f; // since our integral isn't reliable, reduce it to 0 over time.

	// "PID controller" constants
	// this stuff is probably being done all wrong, but these seem to work ok
	static const float Kp = 40.0f;
	static const float Ki = 0.55f;
	static const float Kd = 0.04f;

	float errorTerm = error * Kp;
	float derivativeTerm = derivative * Kd;
	float integralTerm = integral * Ki;
	float adjustment = errorTerm + derivativeTerm + integralTerm;

	// apply the output adjustment
	fSkipFrames += adjustment;

	// if we're running too slowly, prevent the throttle from kicking in
	if(adjustment > 0 && fSkipFrames > 0)
		ltime-=tfreq/desiredfps;

	preThrottleEndticks = 0;
	beginticks = GetCurTime();
}

int AutoFrameSkip_GetSkipAmount(int min, int max)
{
	int rv = (int)fSkipFrames;
	fSkipFramesError += fSkipFrames - rv;

	// resolve accumulated fractional error
	// where doing so doesn't push us out of range
	while(fSkipFramesError >= 1.0f && rv <= lastSkip && rv < max)
	{
		fSkipFramesError -= 1.0f;
		rv++;
	}
	while(fSkipFramesError <= -1.0f && rv >= lastSkip && rv > min)
	{
		fSkipFramesError += 1.0f;
		rv--;
	}

	// restrict skip amount to requested range
	if(rv < min)
		rv = min;
	if(rv > max)
		rv = max;

	// limit maximum error accumulation (it's mainly only for fractional components)
	if(fSkipFramesError >= 4.0f)
		fSkipFramesError = 4.0f;
	if(fSkipFramesError <= -4.0f)
		fSkipFramesError = -4.0f;

	// limit ongoing skipframes to requested range + 1 on each side
	if(fSkipFrames < min-1)
		fSkipFrames = (float)min-1;
	if(fSkipFrames > max+1)
		fSkipFrames = (float)max+1;

//	printf("%d", rv);

	lastSkip = rv;
	return rv;
}

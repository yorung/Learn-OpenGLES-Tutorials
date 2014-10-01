#include "stdafx.h"

BgTransferer bgTransferer;

const static float loopTime = 20.0f;
const static float transferDuration = 0.4166f;
const static int bgMax = 3;
const static float delayDuration = 1.0f;
const static float durationPer1bg = loopTime / bgMax;

BgTransferer::BgTransferer()
{
	elapsedTime = 0;
	lastTime = 0;
}

void BgTransferer::Init()
{
	elapsedTime = 0;
	lastTime = GetTime();
}

void BgTransferer::GetBGs(int& bg1, int& bg2)
{
	bg1 = std::max(0, std::min(bgMax - 1, (int)(bgMax * elapsedTime / loopTime)));
	bg2 = GetTransferTime() != 0 ? (bg1 + 1) % bgMax : bg1;
}

float BgTransferer::GetTransferTime()	// 0 <= ret < (delayDuration + transferDuration)
{
	float oneBgDuration = (float)fmod(elapsedTime, (double)loopTime / bgMax);
	return std::max(0.0f, oneBgDuration - (durationPer1bg - (delayDuration + transferDuration)));
}

void BgTransferer::Update()
{
	double now = GetTime();
	double dt = now - lastTime;
	lastTime = now;

	elapsedTime += dt;
	elapsedTime = fmod(elapsedTime, (double)loopTime);
}

// � 2023 Friendly Shade, Inc.
// � 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Timer.h"


fsi::Timer::Timer()
{
}


fsi::Timer::~Timer()
{
}


void fsi::Timer::start()
{
	m_time = std::chrono::steady_clock::now();
}


uint64_t fsi::Timer::elapsedMs()
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	uint64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (currentTime - m_time).count();
	return elapsed;
}


float fsi::Timer::elapsedS()
{
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	uint64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (currentTime - m_time).count();
	return (float)elapsed / 1000.0f;
}
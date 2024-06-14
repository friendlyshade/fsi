// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include "../global.h"
#include <chrono>

namespace fsi { class Timer; }

class FSI_CORE_API fsi::Timer
{
public:

	Timer();

	~Timer();

public:

	void start();

	uint64_t elapsedMs();
	
	float elapsedS();

private:

	std::chrono::steady_clock::time_point m_time;

};

#if FSI_HEADERONLY
#include "Timer.hpp"
#endif
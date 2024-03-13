// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "../fsi_core_exports.h"
#include "../global.h"
#include <thread>
#include <functional>
#include <memory>
#include <atomic>


namespace fsi { class ProgressThread; }

class fsi::ProgressThread
{
public:

	enum class StateRequest
	{
		NoAction,
		Pause,
		Resume,
		Cancel,
	};

public:

	// Pointer to a function called periodically by Reader and Writer. This can be used to implement
	// progress feedback. It takes an opaque data pointer (passed to Reader/Writer) and a float giving
	// the portion of work done so far. It returns a bool, which if 'true' will STOP the read or write.
	typedef std::function<StateRequest(void* opaquePointer, float progress)> ReportProgressCB;

	// Function that is run when progress is required
	typedef std::function<float()> CalcProgressCB;

	// Function that is run when operation is canceled
	typedef std::function<void()> CancelCB;

	// Function that is run when operation is paused
	typedef std::function<void()> PauseCB;

	// Function that is run when operation is resumed
	typedef std::function<void()> ResumeCB;

public:

	ProgressThread(
		void* reportProgressOpaquePtr,
		ReportProgressCB reportProgressCB,
		CalcProgressCB calcProgressCB,
		CancelCB cancelCB,
		PauseCB pauseCB,
		ResumeCB resumeCB,
		uint64_t updateFrequency);

	~ProgressThread();

	FSI_DISABLE_COPY_MOVE(ProgressThread);

public:

	void join(bool completed = false);

private:

	static void updateProgress(
		void* reportProgressOpaquePtr,
		ReportProgressCB reportProgressCB,
		CalcProgressCB calcProgressCB,
		CancelCB cancelCB,
		PauseCB pauseCB,
		ResumeCB resumeCB,
		const std::atomic<bool>& finished,
		uint64_t updateFrequency);

private:

	std::thread m_thread;

	std::atomic<bool> m_finish;

	void* m_reportProgressOpaquePtr;

	ReportProgressCB m_reportProgressCB;
};

#if FSI_CORE_HEADERONLY
#include "ProgressThread.hpp"
#endif
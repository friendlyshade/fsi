// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#include "ProgressThread.h"
#include <iostream>

FSI_INLINE_HPP
fsi::ProgressThread::ProgressThread(
	void* reportProgressOpaquePtr,
	ReportProgressCB reportProgressCB,
	CalcProgressCB calcProgressCB,
	CancelCB cancelCB,
	PauseCB pauseCB,
	ResumeCB resumeCB,
	uint64_t updateFrequency)
	: m_finish(false)
	, m_reportProgressOpaquePtr(reportProgressOpaquePtr)
	, m_reportProgressCB(reportProgressCB)
{
	m_thread = std::thread(
		&fsi::ProgressThread::updateProgress, 
		reportProgressOpaquePtr,
		reportProgressCB,
		calcProgressCB,
		cancelCB,
		pauseCB,
		resumeCB,
		std::ref(m_finish),
		updateFrequency);
}

FSI_INLINE_HPP
fsi::ProgressThread::~ProgressThread()
{
}

FSI_INLINE_HPP
void fsi::ProgressThread::join(bool completed)
{
	m_finish = true;
	m_thread.join();

	if (completed && m_reportProgressCB)
		m_reportProgressCB(m_reportProgressOpaquePtr, 1.0f);
}

FSI_INLINE_HPP
void fsi::ProgressThread::updateProgress(
	void* progressOpaquePtr,
	ReportProgressCB reportProgressCB,
	CalcProgressCB calcProgressCB,
	CancelCB cancelCB,
	PauseCB pauseCB,
	ResumeCB resumeCB,
	const std::atomic<bool>& finished,
	uint64_t updateFrequency)
{
	if (!reportProgressCB)
		return;

	while (!finished)
	{
		// Calculate progress
		float progress = calcProgressCB();

		// Report progress and get state request
		StateRequest stateRequest = reportProgressCB(progressOpaquePtr, progress);

		switch (stateRequest)
		{
		case fsi::ProgressThread::StateRequest::Pause:
			pauseCB();
			break;
		case fsi::ProgressThread::StateRequest::Resume:
			resumeCB();
			break;
		case fsi::ProgressThread::StateRequest::Cancel:
			resumeCB(); // in case it was paused
			cancelCB();
			return;
			break;
		case fsi::ProgressThread::StateRequest::NoAction:
		default:
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(updateFrequency));
	}
}
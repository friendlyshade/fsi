// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "WriterImpl.h"
#include "consts.h"
#include "proc.h"
#include "exceptions.hpp"
#include <iostream>
#include <atomic>
#include <algorithm>
#include <exception>

FSI_INLINE_HPP
fsi::WriterImpl::WriterImpl()
{
}

FSI_INLINE_HPP
fsi::WriterImpl::~WriterImpl()
{
	close();
}

FSI_INLINE_HPP
fsi::Header fsi::WriterImpl::header()
{
	return m_header;
}

FSI_INLINE_HPP
void fsi::WriterImpl::open(const std::filesystem::path& path, const Header& header)
{
	// Check file extension
	if (path.extension() != expectedFileExtension)
		throw ExceptionInvalidFileExtension();

	// Set header
	m_header = header;

	// Set path
	m_path = path;

	// Open file
	m_file = std::ofstream(m_path, std::ios::binary);
	if (m_file.fail())
		throw ExceptionFailedToCreateFile();

	// Write signature
	m_file.write((char*)(expectedFormatSignature), sizeof(expectedFormatSignature));

	// Write version
	uint32_t version = static_cast<uint32_t>(formatVersion());
	m_file.write((char*)(&version), sizeof(uint32_t));

	// Write the rest of the header specific to the file version
	try
	{
		open(m_file, m_header);
	}
	catch (...)
	{
		// Close file and rethrow the exception
		close();
		throw;
	}
}

FSI_INLINE_HPP
bool fsi::WriterImpl::write(const uint8_t* data, ProgressThread::ReportProgressCB reportProgressCB,
	void* reportProgressOpaquePtr)
{
	if (!m_file.is_open())
		throw ExceptionFileIsNotOpen("The file must be opened before writing can be attempted");

	std::atomic<bool> canceled = false;
	std::atomic<bool> paused = false;
	std::atomic<float> progress = 0.0f;
	ProgressThread progressThread(reportProgressOpaquePtr, reportProgressCB,
		[&progress]() { return progress.load(std::memory_order_relaxed); },
		[&canceled]() { canceled = true; },
		[&paused]() { paused = true; },
		[&paused]() { paused = false; },
		progressCallbackInterval);

	// Write the data specific to the file version
	try
	{
		write(m_file, m_header, data, paused, canceled, progress);
	}
	catch (...)
	{
		// Join the progress thread, close file and rethrow the exception
		progressThread.join(false);
		close();
		throw;
	}

	progressThread.join(!canceled);
	close();
	return canceled;
}

FSI_INLINE_HPP
void fsi::WriterImpl::close()
{
	if (m_file.is_open())
		m_file.close();
}
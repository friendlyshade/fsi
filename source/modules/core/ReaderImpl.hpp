// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "ReaderImpl.h"
#include "consts.h"
#include <iostream>
#include <atomic>
#include <string>

fsi::ReaderImpl::ReaderImpl()
{
}

fsi::ReaderImpl::~ReaderImpl()
{
	close();
}

fsi::Result fsi::ReaderImpl::open(const std::filesystem::path& path)
{
	// Check file extension
	if (path.extension() != expectedFileExtension)
		return Result::Code::InvalidFileExtension;

	// Store path
	m_path = path;

	// Open file
	m_file = std::ifstream(path, std::ios::binary);
	if (m_file.fail())
		return Result::Code::FailedToOpenFile;

	// Read and check signature
	const uint8_t formatSignature[sizeof(expectedFormatSignature)] = {};
	{
		m_file.read((char*)formatSignature, sizeof(expectedFormatSignature));

		for (size_t c = 0; c < sizeof(expectedFormatSignature); c++)
		{
			if (formatSignature[c] != expectedFormatSignature[c])
			{
				close();
				return Result::Code::InvalidSignature;
			}
		}
	}

	// Read the version of the file specification
	m_file.read((char*)(&m_formatVersion), sizeof(uint32_t));
	
	switch (m_formatVersion)
	{
	case fsi::FormatVersion::V1:
		break;
	case fsi::FormatVersion::V2:
		break;
	default:
		return { Result::Code::InvalidFormatVersion, "Version "
			+ std::to_string(static_cast<uint32_t>(m_formatVersion))
			+ " is not a valid FSI format version" };
		break;
	}

	// Read the rest of the header specific to the file version
	Result result = open(m_file, m_header);
	if (result != Result::Code::Success)
	{
		close();
		return result;
	}

	return Result::Code::Success;
}

fsi::Result fsi::ReaderImpl::read(uint8_t* data, uint8_t* thumbData,
	ProgressThread::ReportProgressCB reportProgressCB, void* reportProgressOpaquePtr)
{
	if (!m_file.is_open())
		return Result::Code::FileIsNotOpen;

	std::atomic<bool> canceled = false;
	std::atomic<bool> paused = false;
	std::atomic<float> progress = 0.0f;
	ProgressThread progressThread(reportProgressOpaquePtr, reportProgressCB,
		[&progress]() { return progress.load(std::memory_order_relaxed); },
		[&canceled]() { canceled = true; },
		[&paused]() { paused = true; },
		[&paused]() { paused = false; },
		progressCallbackInterval);

	// Read the data specific to the file version
	Result result = read(m_file, m_header, data, thumbData, paused, canceled, progress);
	if (result != Result::Code::Success)
	{
		progressThread.join();
		close();
		return result;
	}

	progressThread.join(true);
	close();
	return Result::Code::Success;
}

void fsi::ReaderImpl::close()
{
	if (m_file.is_open())
		m_file.close();
}

fsi::Header fsi::ReaderImpl::header()
{
	return m_header;
}
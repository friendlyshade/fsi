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
#include <iostream>
#include <atomic>
#include <algorithm>
#include <exception>

#define WRITE_THUMB_AS_FILE 0

#if WRITE_THUMB_AS_FILE
	size_t thumbWidth = 256;
	size_t thumbHeight = 256;
	size_t thumbChannels = 4;
	fsi::Depth thumbDepth = fsi::Depth::Uint8;

	fsi::Header originalHeader;
#endif

fsi::WriterImpl::WriterImpl()
{
}

fsi::WriterImpl::~WriterImpl()
{
	close();
}

fsi::Result fsi::WriterImpl::open(const std::filesystem::path& path, const Header& header)
{
	// Check file extension
	if (path.extension() != expectedFileExtension)
		return Result::Code::InvalidFileExtension;

#if WRITE_THUMB_AS_FILE
	originalHeader.width = header.width;
	originalHeader.height = header.height;
	originalHeader.channels = header.channels;
	originalHeader.depth = header.depth;

	header.width = thumbWidth;
	header.height = thumbHeight;
	header.channels = thumbChannels;
	header.depth = thumbDepth;
#endif

	// Set header
	m_header = header;

	// Set path
	m_path = path;

	// Open file
	m_file = std::ofstream(m_path, std::ios::binary);
	if (m_file.fail())
		return Result::Code::FailedToCreateFile;

	// Write signature
	m_file.write((char*)(expectedFormatSignature), sizeof(expectedFormatSignature));

	// Write version
	uint32_t version = static_cast<uint32_t>(formatVersion());
	m_file.write((char*)(&version), sizeof(uint32_t));

	// Write the rest of the header specific to the file version
	Result result = open(m_file, m_header);
	if (result != Result::Code::Success)
	{
		close();
		return result;
	}

	return Result::Code::Success;
}

fsi::Result fsi::WriterImpl::write(const uint8_t* data, ProgressThread::ReportProgressCB reportProgressCB,
	void* reportProgressOpaquePtr)
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

#if WRITE_THUMB_AS_FILE
	uint8_t* thumbData = new uint8_t[thumbWidth * thumbHeight * thumbChannels * sizeOfDepth(thumbDepth)];
	
	fsi::Timer timer; timer.start();
	proc::generateThumbnail(data, step, originalHeader, thumbData, thumbWidth, thumbHeight);
	std::cout << "Thumbnail generated in " << timer.elapsedMs() << " ms\n";

	// Temp: write thumb as the image
	data = thumbData;
	step = thumbWidth * thumbChannels;
#endif

	// Write the data specific to the file version
	Result result = write(m_file, m_header, data, paused, canceled, progress);
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

void fsi::WriterImpl::close()
{
	if (m_file.is_open())
		m_file.close();
}
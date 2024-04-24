// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Writer.h"
#include "WriterV1.h"
#include "WriterV2.h"
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

/*fsi::Writer::Writer()
{
}

inline fsi::Writer::~Writer()
{
	close();
}

fsi::Result fsi::Writer::open()
{
}

inline void fsi::Writer::close()
{
	m_impl->close();
}*/

std::unique_ptr<fsi::Writer> fsi::Writer::createWriter(FormatVersion formatVersion)
{
	if (formatVersion == FormatVersion::V1)
	{
		return std::make_unique<WriterV1>();
	}
	else if (formatVersion == FormatVersion::V2)
	{
		return std::make_unique<WriterV2>();
	}

	throw std::exception("Header must be castable to Header_V1 or Header_V2");
	return nullptr;
}

fsi::Writer::Writer() {}

fsi::Writer::~Writer()
{
	close();
}

fsi::Result fsi::Writer::open(const std::filesystem::path& path, const Header& header)
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
	uint32_t version = formatVersion();
	m_file.write((char*)(&version), sizeof(uint32_t));

	// Read the rest of the header, immediately after the format version
	Result result = openImpl();
	if (result != Result::Code::Success)
	{
		close();
		return result;
	}

	return Result::Code::Success;
}

fsi::Result fsi::Writer::write(const uint8_t* data, ProgressThread::ReportProgressCB reportProgressCB,
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

	Result result = writeImpl(data, paused, canceled, progress);
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

void fsi::Writer::close()
{
	if (m_file.is_open())
		m_file.close();
}
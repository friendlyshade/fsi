// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Writer.h"
#include "consts.h"
#include "proc.h"
#include <iostream>
#include <atomic>

#define WRITE_THUMB_AS_FILE 1

#if WRITE_THUMB_AS_FILE
	size_t thumbWidth = 256;
	size_t thumbHeight = 256;
	size_t thumbChannels = 3;
	fsi::Depth thumbDepth = fsi::Depth::Uint8;

	fsi::Header originalHeader;
#endif

fsi::Writer::Writer() {}

fsi::Writer::~Writer()
{
	close();
}

fsi::Result fsi::Writer::open(const std::filesystem::path& path, Header header,
	FormatVersion useFormatVersion)
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

	// Set format version
	m_formatVersion = useFormatVersion;

	// Set path
	m_path = path;

	// Open file
	m_file = std::ofstream(m_path, std::ios::binary);
	if (m_file.fail())
		return Result::Code::FailedToCreateFile;

	// Write signature
	m_file.write((char*)(expectedFormatSignature), sizeof(expectedFormatSignature));

	// Write version
	m_file.write((char*)(&useFormatVersion), sizeof(uint32_t));

	// Read the rest of the header, immediately after the format version
	switch (useFormatVersion)
	{
	case fsi::FormatVersion::V2:
	{
		uint64_t width = static_cast<uint64_t>(header.width);
		uint64_t height = static_cast<uint64_t>(header.height);
		uint64_t channels = static_cast<uint64_t>(header.channels);
		uint8_t depth = static_cast<uint8_t>(header.depth);

		if (!(channels >= 1 && channels <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageChannels, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(width >= 1 && width <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageWidth, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(height >= 1 && height <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageHeight, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(depth >= 1 && depth <= 10))
		{
			close();
			return { Result::Code::InvalidImageDepth, "Must be an integer between 1 and 10" };
		}

		m_file.write((char*)(&width), sizeof(uint64_t));
		m_file.write((char*)(&height), sizeof(uint64_t));
		m_file.write((char*)(&channels), sizeof(uint64_t));
		m_file.write((char*)(&depth), sizeof(uint8_t));

		break;
	}
	case fsi::FormatVersion::V1:
	{
		uint32_t width = static_cast<uint64_t>(header.width);
		uint32_t height = static_cast<uint64_t>(header.height);
		uint32_t channels = static_cast<uint64_t>(header.channels);
		uint32_t depth = static_cast<uint8_t>(header.depth);

		if (!(channels >= 1 && channels <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageChannels, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(width >= 1 && width <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageWidth, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(height >= 1 && height <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageHeight, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(depth >= 1 && depth <= 10))
		{
			close();
			return { Result::Code::InvalidImageDepth, "Must be an integer between 1 and 10" };
		}

		m_file.write((char*)(&width), sizeof(uint32_t));
		m_file.write((char*)(&height), sizeof(uint32_t));
		m_file.write((char*)(&channels), sizeof(uint32_t));
		m_file.write((char*)(&depth), sizeof(uint32_t));

		break;
	}
	default:
		close();
		return Result::Code::InvalidFormatVersion;
	}

	return Result::Code::Success;
}
#include "Timer.h"
fsi::Result fsi::Writer::write(const uint8_t* data, uint64_t step, bool thumbnail,
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

#if WRITE_THUMB_AS_FILE
	uint8_t* thumbData = new uint8_t[thumbWidth * thumbHeight * thumbChannels * sizeOfDepth(thumbDepth)];
	
	fsi::Timer timer; timer.start();
	proc::generateThumbnail(data, step, originalHeader, thumbData, thumbWidth, thumbHeight);
	std::cout << "Thumbnail generated in " << timer.elapsedMs() << " ms\n";

	// Temp: write thumb as the image
	data = thumbData;
	step = thumbWidth * thumbChannels;
#endif

	switch (m_formatVersion)
	{
	case FormatVersion::V2:
	{
		// Generate thumbnail
		if (thumbnail)
		{
			/*
			// Decide on which operation to do first based on how much memory the dst image will take
			uint64_t sizeAfterCvtColor = m_header.width * m_header.height * 4;
			uint64_t sizeAfterDecimate = 256 * 256 * m_header.channels;
			
			// Convert to RGBA first, then decimate
			if (sizeAfterCvtColor < sizeAfterDecimate)
			{
			}
			// Decimate first, then convert to RGBA
			else
			{
			}
			*/
		}
	}
	case FormatVersion::V1:
	{
		const uint64_t depthSize = sizeOfDepth(m_header.depth);
		const uint64_t imageSize = m_header.width * m_header.height * m_header.channels * depthSize;

		// If buffer is larger than the total data, adjust the buffer size
		const uint64_t bufferSize = defaultBufferSize > imageSize ? imageSize : defaultBufferSize;

		// Write chunks of bytes
		size_t ptr_offset = 0;
		const size_t total = imageSize - bufferSize;
		for (; ptr_offset < total; ptr_offset += bufferSize)
		{
			while (paused)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			if (canceled)
			{
				progressThread.join();
				close();
				return Result::Code::Canceled;
			}

			m_file.write((char*)(data + ptr_offset), bufferSize);

			progress = static_cast<float>(ptr_offset) / static_cast<float>(total);
		}

		// Write remaining bytes (if any)
		size_t remainder_size = imageSize % bufferSize;
		if (remainder_size == 0)
			remainder_size = bufferSize;
		size_t remainder_ptr_offset = imageSize - remainder_size;
		m_file.write((char*)(data + remainder_ptr_offset), remainder_size);

		break;
	}
	default:
		progressThread.join();
		close();
		return Result::Code::InvalidFormatVersion;
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
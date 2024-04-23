// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Reader.h"
#include "consts.h"
#include <iostream>
#include <atomic>

fsi::Reader::Reader() {}

fsi::Reader::~Reader()
{
	close();
}

fsi::Result fsi::Reader::open(const std::filesystem::path& path)
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

	// Read the rest of the header, immediately after the version
	switch (m_formatVersion)
	{
	case FormatVersion::V2:
	{
		uint64_t width;
		uint64_t height;
		uint64_t channels;
		uint8_t depth;

		m_file.read((char*)(&width), sizeof(uint64_t));
		m_file.read((char*)(&height), sizeof(uint64_t));
		m_file.read((char*)(&channels), sizeof(uint64_t));
		m_file.read((char*)(&depth), sizeof(uint8_t));

		if (!(channels >= 1 && channels <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageWidth, "Must be an integer between 1 and 1,048,575" };
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

		m_header.width = width;
		m_header.height = height;
		m_header.channels = channels;
		m_header.depth = static_cast<Depth>(depth);

		break;
	}
	case FormatVersion::V1:
	{
		uint32_t width;
		uint32_t height;
		uint32_t channels;
		uint32_t depth;

		m_file.read((char*)(&width), sizeof(uint32_t));
		m_file.read((char*)(&height), sizeof(uint32_t));
		m_file.read((char*)(&channels), sizeof(uint32_t));
		m_file.read((char*)(&depth), sizeof(uint32_t));

		if (!(channels >= 1 && channels <= 1048575))
		{
			close();
			return { Result::Code::InvalidImageWidth, "Must be an integer between 1 and 1,048,575" };
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

		m_header.width = static_cast<uint64_t>(width);
		m_header.height = static_cast<uint64_t>(height);
		m_header.channels = static_cast<uint64_t>(channels);
		m_header.depth = static_cast<Depth>(depth);

		break;
	}
	default:
		close();
		return Result::Code::InvalidFormatVersion;
	}

	return Result::Code::Success;
}

fsi::Result fsi::Reader::read(uint8_t* data, ProgressThread::ReportProgressCB reportProgressCB,
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

	switch (m_formatVersion)
	{
	case FormatVersion::V2:
	case FormatVersion::V1:
	{
		const uint64_t depthSize = sizeOfDepth(m_header.depth);
		const uint64_t imageSize = m_header.width * m_header.height * m_header.channels * depthSize;


		// TODO: Check remaining size of file equals to "imageSize"


		// If buffer is larger than the total data, adjust the buffer size
		const uint64_t bufferSize = defaultBufferSize > imageSize ? imageSize : defaultBufferSize;

		// Read data
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

			m_file.read((char*)(data + ptr_offset), bufferSize);

			progress = static_cast<float>(ptr_offset) / static_cast<float>(total);
		}

		// Read remaining bytes (if any)
		size_t remainder_size = imageSize % bufferSize;
		if (remainder_size == 0)
			remainder_size = bufferSize;
		size_t remainder_ptr_offset = imageSize - remainder_size;
		m_file.read((char*)(data + remainder_ptr_offset), remainder_size);

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

void fsi::Reader::close()
{
	if (m_file.is_open())
		m_file.close();
}

fsi::Header_V1 fsi::Reader::header()
{
	return m_header;
}
// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Writer_V1.h"
#include "consts.h"
#include "proc.h"
#include <iostream>
#include <atomic>
#include <algorithm>

fsi::Writer_V1::Writer_V1(const Header* header)
	: Writer(header)
{
}

fsi::Result fsi::Writer_V1::openImpl()
{
	fsi::Header_V1* header_v1 = dynamic_cast<fsi::Header_V1*>(m_header);
	assert(header_v1 != nullptr && "Header must point to a Header_V1");

	uint32_t depth = static_cast<uint8_t>(header_v1->depth);

	if (!(header_v1->channels >= 1 && header_v1->channels <= 1048575))
	{
		return { Result::Code::InvalidImageChannels, "Must be an integer between 1 and 1,048,575" };
	}

	if (!(header_v1->width >= 1 && header_v1->width <= 1048575))
	{
		return { Result::Code::InvalidImageWidth, "Must be an integer between 1 and 1,048,575" };
	}

	if (!(header_v1->height >= 1 && header_v1->height <= 1048575))
	{
		return { Result::Code::InvalidImageHeight, "Must be an integer between 1 and 1,048,575" };
	}

	if (!(depth >= 1 && depth <= 10))
	{
		return { Result::Code::InvalidImageDepth, "Must be an integer between 1 and 10" };
	}

	m_file.write((char*)(&header_v1->width), sizeof(uint32_t));
	m_file.write((char*)(&header_v1->height), sizeof(uint32_t));
	m_file.write((char*)(&header_v1->channels), sizeof(uint32_t));
	m_file.write((char*)(&depth), sizeof(uint32_t));

	return Result::Code::Success;
}

fsi::Result fsi::Writer_V1::writeImpl(const uint8_t* data, const std::atomic<bool>& paused,
	const std::atomic<bool>& canceled, std::atomic<float>& progress)
{
	fsi::Header_V1* header_v1 = dynamic_cast<fsi::Header_V1*>(m_header);
	assert(header_v1 != nullptr && "Header must point to a Header_V1");

	const uint64_t depthSize = sizeOfDepth(header_v1->depth);
	const uint64_t imageSize = header_v1->width * header_v1->height * header_v1->channels * depthSize;

	// If buffer is larger than the total data, adjust the buffer size
	const uint64_t bufferSize = defaultBufferSize > imageSize ? imageSize : defaultBufferSize;

	// Write chunks of bytes
	size_t ptr_offset = 0;
	const size_t total = imageSize - bufferSize;
	for (; ptr_offset < total; ptr_offset += bufferSize)
	{
		while (paused)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (canceled)
			return Result::Code::Canceled;

		m_file.write((char*)(data + ptr_offset), bufferSize);

		progress = static_cast<float>(ptr_offset) / static_cast<float>(total);
	}

	// Write remaining bytes (if any)
	size_t remainder_size = imageSize % bufferSize;
	if (remainder_size == 0)
		remainder_size = bufferSize;
	size_t remainder_ptr_offset = imageSize - remainder_size;
	m_file.write((char*)(data + remainder_ptr_offset), remainder_size);

	return Result::Code::Success;
}

uint32_t fsi::Writer_V1::formatVersion() const
{
	return 1;
}

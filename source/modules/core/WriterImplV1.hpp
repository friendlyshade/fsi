// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "WriterImplV1.h"
#include "consts.h"
#include "proc.h"
#include "exceptions.hpp"
#include <iostream>
#include <atomic>
#include <algorithm>

FSI_INLINE_HPP
fsi::FormatVersion fsi::WriterImplV1::formatVersion()
{
	return FormatVersion::V1;
}

FSI_INLINE_HPP
void fsi::WriterImplV1::open(std::ofstream& file, Header& header)
{
	uint32_t depth = static_cast<uint32_t>(header.depth);

	if (!(header.channels >= 1 && header.channels <= 1048575))
	{
		throw ExceptionInvalidImageChannels("Must be an integer between 1 and 1,048,575");
	}

	if (!(header.width >= 1 && header.width <= 1048575))
	{
		throw ExceptionInvalidImageWidth("Must be an integer between 1 and 1,048,575");
	}

	if (!(header.height >= 1 && header.height <= 1048575))
	{
		throw ExceptionInvalidImageHeight("Must be an integer between 1 and 1,048,575");
	}

	if (!(depth >= 1 && depth <= 10))
	{
		throw ExceptionInvalidImageDepth("Must be an integer between 1 and 10");
	}

	file.write((char*)(&header.width), sizeof(uint32_t));
	file.write((char*)(&header.height), sizeof(uint32_t));
	file.write((char*)(&header.channels), sizeof(uint32_t));
	file.write((char*)(&depth), sizeof(uint32_t));
}

FSI_INLINE_HPP
void fsi::WriterImplV1::write(std::ofstream& file, const Header& header, const uint8_t* data,
	const std::atomic<bool>& paused, const std::atomic<bool>& canceled, std::atomic<float>& progress)
{
	const uint64_t depthSize = sizeOfDepth(header.depth);
	const uint64_t imageSize = header.width * header.height * header.channels * depthSize;

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
			return;

		file.write((char*)(data + ptr_offset), bufferSize);

		progress = static_cast<float>(ptr_offset) / static_cast<float>(total);
	}

	// Write remaining bytes (if any)
	size_t remainder_size = imageSize % bufferSize;
	if (remainder_size == 0)
		remainder_size = bufferSize;
	size_t remainder_ptr_offset = imageSize - remainder_size;
	file.write((char*)(data + remainder_ptr_offset), remainder_size);
}
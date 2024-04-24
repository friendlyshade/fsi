// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "WriterV2.h"
#include "consts.h"
#include "proc.h"
#include <iostream>
#include <atomic>
#include <algorithm>
#include <vector>

fsi::Result fsi::WriterV2::openImpl()
{
	// --- Write image header ---
	{
		uint8_t depth = static_cast<uint8_t>(m_header.depth);

		if (!(m_header.channels >= 1 && m_header.channels <= 1048575))
		{
			return { Result::Code::InvalidImageChannels, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(m_header.width >= 1 && m_header.width <= 1048575))
		{
			return { Result::Code::InvalidImageWidth, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(m_header.height >= 1 && m_header.height <= 1048575))
		{
			return { Result::Code::InvalidImageHeight, "Must be an integer between 1 and 1,048,575" };
		}

		if (!(depth >= 1 && depth <= 10))
		{
			return { Result::Code::InvalidImageDepth, "Must be an integer between 1 and 10" };
		}

		m_file.write((char*)(&m_header.width), sizeof(uint64_t));
		m_file.write((char*)(&m_header.height), sizeof(uint64_t));
		m_file.write((char*)(&m_header.channels), sizeof(uint64_t));
		m_file.write((char*)(&depth), sizeof(uint8_t));
	}

	// --- Write thumbnail header ---
	{
		// Write "has thumbnail"
		uint8_t generateThumbnailInt = m_header.hasThumb ? 1 : 0;
		m_file.write((char*)(&generateThumbnailInt), sizeof(uint8_t));

		// Write thumbnail dimensions
		if (m_header.hasThumb)
		{
			calcThumbDimensions(m_header.width, m_header.height, m_header.thumbWidth,
				m_header.thumbHeight);
			m_file.write((char*)(&m_header.thumbWidth), sizeof(uint16_t));
			m_file.write((char*)(&m_header.thumbHeight), sizeof(uint16_t));
		}
	}

	return Result::Code::Success;
}

fsi::Result fsi::WriterV2::writeImpl(const uint8_t* data, const std::atomic<bool>& paused,
	const std::atomic<bool>& canceled, std::atomic<float>& progress)
{
	// --- Write thumbnail data ---
	{
		if (m_header.hasThumb)
		{
			// Thumbnail depth (Uint8)
			const Depth thumbDepth = Depth::Uint8;
			// Thumbnail channels (RGBA)
			const uint64_t thumbChannels = 4;

			const uint64_t thumbSize = m_header.thumbWidth * m_header.thumbHeight * thumbChannels * sizeOfDepth(thumbDepth);
			std::vector<uint8_t> thumb(thumbSize);
			
			const uint64_t step = m_header.width * m_header.channels;

			// fsi::Timer timer; timer.start();
			proc::generateThumbnail(data, step, m_header, thumb.data(), m_header.thumbWidth,
				m_header.thumbHeight);
			// std::cout << "Thumbnail generated in " << timer.elapsedMs() << " ms\n";

#if WRITE_THUMB_AS_FILE
			// Temp: write thumb as the image
			data = thumbData;
			step = thumbWidth * thumbChannels;
#endif
			m_file.write((char*)(thumb.data()), thumbSize);
		}
	}
	
	// --- Write image data ---
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
	}

	return Result::Code::Success;
}

uint32_t fsi::WriterV2::formatVersion() const
{
	return 2;
}

void fsi::WriterV2::calcThumbDimensions(uint32_t imageWidth, uint32_t imageHeight,
	uint16_t& thumbWidth, uint16_t& thumbHeight)
{
	const uint16_t thumbnailSize = 256;

	if (imageWidth > thumbnailSize || imageHeight > thumbnailSize)
	{
		if (imageWidth > imageWidth)
		{
			thumbWidth = thumbnailSize;
			thumbHeight = static_cast<uint16_t>((static_cast<float>(thumbnailSize)
				/ static_cast<float>(imageWidth)) * static_cast<float>(imageHeight));
		}
		else
		{
			thumbHeight = thumbnailSize;
			thumbWidth = static_cast<uint16_t>((static_cast<float>(thumbnailSize)
				/ static_cast<float>(imageHeight)) * static_cast<float>(imageWidth));
		}
	}
	thumbWidth = std::max(uint16_t(1), thumbWidth);
	thumbHeight = std::max(uint16_t(1), thumbHeight);

	std::cout << "Thumb width: " << thumbWidth << "\n";
	std::cout << "Thumb height: " << thumbHeight << "\n";

	assert(thumbWidth <= thumbnailSize && "Thumbnail max width is 256. Should not reach here.");
	assert(thumbHeight <= thumbnailSize && "Thumbnail max height is 256. Should not reach here.");
}

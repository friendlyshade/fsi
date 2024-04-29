// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "WriterImplV2.h"
#include "consts.h"
#include "proc.h"
#include "exceptions.hpp"
#include <iostream>
#include <atomic>
#include <algorithm>
#include <vector>

fsi::FormatVersion fsi::WriterImplV2::formatVersion()
{
	return FormatVersion::V2;
}

void fsi::WriterImplV2::open(std::ofstream& file, Header& header)
{
	// --- Write image header ---
	{
		uint8_t depth = static_cast<uint8_t>(header.depth);

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
		file.write((char*)(&depth), sizeof(uint8_t));

		uint8_t hasThumb = header.hasThumb ? 1 : 0;
		file.write((char*)(&hasThumb), sizeof(uint8_t));
	}

	// --- Write thumbnail header ---
	{
		// Write "has thumbnail"
		uint8_t generateThumbnailInt = header.hasThumb ? 1 : 0;
		file.write((char*)(&generateThumbnailInt), sizeof(uint8_t));

		// Write thumbnail dimensions
		if (header.hasThumb)
		{
			calcThumbDimensions(header.width, header.height, header.thumbWidth,
				header.thumbHeight);
			file.write((char*)(&header.thumbWidth), sizeof(uint16_t));
			file.write((char*)(&header.thumbHeight), sizeof(uint16_t));
		}
	}
}

void fsi::WriterImplV2::write(std::ofstream& file, const Header& header, const uint8_t* data,
	const std::atomic<bool>& paused, const std::atomic<bool>& canceled, std::atomic<float>& progress)
{
	// --- Write thumbnail data ---
	{
		if (header.hasThumb)
		{
			const uint64_t thumbSize = header.thumbWidth * header.thumbHeight * thumbChannels * sizeOfDepth(thumbDepth);
			std::vector<uint8_t> thumb(thumbSize);
			
			const uint64_t step = header.width * header.channels;

			// fsi::Timer timer; timer.start();
			proc::generateThumbnail(data, step, header, thumb.data(), header.thumbWidth,
				header.thumbHeight);
			// std::cout << "Thumbnail generated in " << timer.elapsedMs() << " ms\n";

#if WRITE_THUMB_AS_FILE
			// Temp: write thumb as the image
			data = thumbData;
			step = thumbWidth * thumbChannels;
#endif
			file.write((char*)(thumb.data()), thumbSize);
		}
	}
	
	// --- Write image data ---
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
}

void fsi::WriterImplV2::calcThumbDimensions(uint32_t imageWidth, uint32_t imageHeight,
	uint16_t& thumbWidth, uint16_t& thumbHeight)
{
	const uint16_t thumbnailSize = 256;

	if (imageWidth > thumbnailSize || imageHeight > thumbnailSize)
	{
		if (imageWidth > imageHeight)
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

	// std::cout << "Thumb width: " << thumbWidth << "\n";
	// std::cout << "Thumb height: " << thumbHeight << "\n";

	assert(thumbWidth <= thumbnailSize && "Thumbnail max width is 256. Should not reach here.");
	assert(thumbHeight <= thumbnailSize && "Thumbnail max height is 256. Should not reach here.");
}

// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "ReaderImplV2.h"
#include "Depth.hpp"
#include "consts.h"
#include <iostream>

fsi::ReaderImplV2::ReaderImplV2()
	: ReaderImpl()
{
}

fsi::FormatVersion fsi::ReaderImplV2::formatVersion()
{
	return FormatVersion::V2;
}

void fsi::ReaderImplV2::open(std::ifstream& file, Header& header)
{
	uint32_t width;
	uint32_t height;
	uint32_t channels;
	uint8_t depth;

	file.read((char*)(&width), sizeof(uint32_t));
	file.read((char*)(&height), sizeof(uint32_t));
	file.read((char*)(&channels), sizeof(uint32_t));
	file.read((char*)(&depth), sizeof(uint8_t));

	if (!(channels >= 1 && channels <= 1048575))
	{
		throw ExceptionInvalidImageWidth("Must be an integer between 1 and 1,048,575");
	}

	if (!(width >= 1 && width <= 1048575))
	{
		throw ExceptionInvalidImageWidth("Must be an integer between 1 and 1,048,575");
	}

	if (!(height >= 1 && height <= 1048575))
	{
		throw ExceptionInvalidImageHeight("Must be an integer between 1 and 1,048,575");
	}

	if (!(depth >= 1 && depth <= 10))
	{
		throw ExceptionInvalidImageDepth("Must be an integer between 1 and 10");
	}

	header.width = width;
	header.height = height;
	header.channels = channels;
	header.depth = static_cast<Depth>(depth);

	uint8_t hasThumb;
	file.read((char*)(&hasThumb), sizeof(uint8_t));

	header.hasThumb = hasThumb > 0;
	std::cout << "header.hasThumb: " << header.hasThumb << "\n";

	if (header.hasThumb)
	{
		file.read((char*)(&header.thumbWidth), sizeof(uint16_t));
		file.read((char*)(&header.thumbHeight), sizeof(uint16_t));

		std::cout << "header.thumbWidth: " << header.thumbWidth << "\n";
		std::cout << "header.thumbHeight: " << header.thumbHeight << "\n";

		if (header.thumbWidth == 0 || header.thumbWidth > 256)
			throw ExceptionInvalidThumbnailWidth("Must be an integer between 1 and 256");

		if (header.thumbHeight == 0 || header.thumbHeight > 256)
			throw ExceptionInvalidThumbnailHeight("Must be an integer between 1 and 256");
	}
	else
	{
		file.ignore(sizeof(uint16_t)*2);
	}
}

void fsi::ReaderImplV2::read(std::ifstream& file, const Header& header, uint8_t* data,
	uint8_t* thumbData, const std::atomic<bool>& paused, const std::atomic<bool>& canceled,
	std::atomic<float>& progress)
{
	// TODO: Check if the remaining size of the file equals to "thumbSize + imageSize"
	
	// --- Read thumbnail data ---
	{
		if (header.hasThumb && thumbData)
		{
			uint64_t usedThumbSizeInBytes = header.thumbWidth * header.thumbHeight * thumbChannels
				* thumbSizeOfDepth;
			file.read((char*)(thumbData), usedThumbSizeInBytes);

			// Skip remaining non-read data
			uint64_t remainingBytes = thumbSizeInBytes - usedThumbSizeInBytes;
			if (remainingBytes > 0)
				file.ignore(remainingBytes);
		}
		else
		{
			file.ignore(thumbSizeInBytes);
		}

		if (!header.hasThumb && thumbData)
		{
			// If thumbData is not nullptr it means the user has allocated space and is expecting the data to
			// be filled. Warn them about the absence of a thumbnail in the file.
			std::cout << "Warning: The thumbnail data will be ignored because there is not thumbnail present"
				" in the file.\n";
		}
	}

	// --- Read image data ---
	if (data)
	{
		const uint64_t depthSize = sizeOfDepth(header.depth);
		const uint64_t imageSize = header.width * header.height * header.channels * depthSize;

		// TODO: Check if the remaining size of the file equals to "imageSize"

		// If buffer is larger than the total data, adjust the buffer size
		const uint64_t bufferSize = defaultBufferSize > imageSize ? imageSize : defaultBufferSize;

		// Read data
		size_t ptr_offset = 0;
		const size_t total = imageSize - bufferSize;
		for (; ptr_offset < total; ptr_offset += bufferSize)
		{
			while (paused)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

			if (canceled)
				return;

			file.read((char*)(data + ptr_offset), bufferSize);

			progress = static_cast<float>(ptr_offset) / static_cast<float>(total);
		}

		// Read remaining bytes (if any)
		size_t remainder_size = imageSize % bufferSize;
		if (remainder_size == 0)
			remainder_size = bufferSize;
		size_t remainder_ptr_offset = imageSize - remainder_size;
		file.read((char*)(data + remainder_ptr_offset), remainder_size);
	}
}
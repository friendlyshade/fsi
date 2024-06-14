// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "proc.h"

FSI_INLINE_HPP
void fsi::proc::generateThumbnail(const uint8_t* srcData, uint64_t srcWidth, uint64_t srcHeight,
	uint64_t srcChannels, Depth srcDepth, uint64_t srcStep, uint8_t* dstData, int64_t dstStep,
	uint64_t targetWidth, uint64_t targetHeight)
{
	switch (srcDepth)
	{
	// Signed
	case Depth::Int8:
		generateThumbnail<int8_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;
	case Depth::Int16:
		generateThumbnail<int16_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;
	case Depth::Int32:
		generateThumbnail<int32_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;
	case Depth::Int64:
		generateThumbnail<int64_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;

	// Unsigned
	case Depth::Uint8:
		generateThumbnail<uint8_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;
	case Depth::Uint16:
		generateThumbnail<uint16_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;
	case Depth::Uint32:
		generateThumbnail<uint32_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;
	case Depth::Uint64:
		generateThumbnail<uint64_t>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;

	// Floating-point
	case Depth::Float32:
		generateThumbnail<float>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;
	case Depth::Float64:
		generateThumbnail<double>(srcData, srcWidth, srcHeight, srcChannels, srcStep, dstData, dstStep,
			targetWidth, targetHeight);
		break;

	default:
		assert(false && "Invalid depth");
		break;
	}
}
// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "proc.h"

void fsi::proc::toRGBA(const uint8_t* srcData, const Header& srcHeader, uint8_t* dstData)
{
}

void fsi::proc::decimate(const uint8_t* srcData, const Header& srcHeader, uint8_t* dstData)
{
}

void fsi::proc::extendBorders(const uint8_t* srcData, const Header& srcHeader, uint8_t* dstData)
{
}

void fsi::proc::generateThumbnail(const uint8_t* srcData, uint64_t srcStep, const Header& srcHeader,
	uint8_t* dstData, uint64_t targetWidth, uint64_t targetHeight)
{
	generateThumbnail<uint16_t>(srcData, srcStep, srcHeader, dstData, targetWidth, targetHeight);
}
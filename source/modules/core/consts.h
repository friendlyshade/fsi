// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once
#include <string>

namespace fsi
{

const uint8_t expectedFormatSignature[] = { 'f', 's', 'i', 'f' };

const std::string expectedFileExtension = ".fsi";

const uint64_t defaultBufferSize = 1024*1024; // in bytes

const uint64_t progressCallbackInterval = 100ull; // in ms

// Thumbnail depth (Uint8)
const Depth thumbDepth = Depth::Uint8;
// Thumbnail channels (RGBA)
const uint64_t thumbChannels = 4;

}
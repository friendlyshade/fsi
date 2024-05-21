// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once
#include "Depth.hpp"

namespace fsi { struct Header; }

struct fsi::Header
{
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t channels = 0;
	Depth depth = Depth::Invalid;

	/** @brief Generates a thumbnail when set to true while writing. It will be true when a thumbnails is
	present after reading an image
	*/
	bool hasThumb = false;
	uint16_t thumbWidth = 0;
	uint16_t thumbHeight = 0;
};
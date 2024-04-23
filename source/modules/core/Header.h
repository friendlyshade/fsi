// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once
#include "Depth.hpp"

namespace fsi { struct Header; struct Header_V1; struct Header_V2; class Writer_V2; }

struct fsi::Header
{
	uint32_t width;
	uint32_t height;
	uint32_t channels;
	Depth depth;

	virtual ~Header() {}
};

struct fsi::Header_V1
	: public Header
{
};

struct fsi::Header_V2
	: public Header
{
	bool hasThumb = true;

private:
	uint16_t thumbWidth;
	uint16_t thumbHeight;
	
	friend class Writer_V2;
};
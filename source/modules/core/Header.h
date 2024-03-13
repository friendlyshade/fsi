// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once
#include "Depth.h"

namespace fsi { struct Header; }

struct fsi::Header
{
	uint64_t width;
	uint64_t height;
	uint64_t channels;
	Depth depth;
};
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

enum class FormatVersion : uint32_t
{
	Invalid = 0,
	V1 = 1,
	V2 = 2,
	Latest = V2,
};

}
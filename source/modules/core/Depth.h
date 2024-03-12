// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include <string>
#include <assert.h>

namespace fsi
{

enum class Depth : uint8_t
{
	INVALID = 0,

	/// signed
	INT8    = 1,
	INT16   = 2,
	INT32   = 3,
	INT64   = 4,

	/// unsigned
	UINT8   = 5,
	UINT16  = 6,
	UINT32  = 7,
	UINT64  = 8,

	/// floating-point
	FLOAT32 = 9,
	FLOAT64 = 10,
};

inline uint64_t sizeOfDepth(Depth depth)
{
	switch (depth)
	{
	case Depth::INT8: return sizeof(int8_t);
	case Depth::INT16: return sizeof(int16_t);
	case Depth::INT32: return sizeof(int32_t);
	case Depth::INT64: return sizeof(int64_t);
	case Depth::UINT8: return sizeof(uint8_t);
	case Depth::UINT16: return sizeof(uint16_t);
	case Depth::UINT32: return sizeof(uint32_t);
	case Depth::UINT64: return sizeof(uint64_t);
	case Depth::FLOAT32: return sizeof(float);
	case Depth::FLOAT64: return sizeof(double);
	default:
		assert(false && "Invalid depth\n");
		return 0;
	}
}

}
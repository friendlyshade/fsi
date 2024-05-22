// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include <string>
#include <assert.h>
#include <ostream>

namespace fsi
{

enum class Depth : uint8_t
{
	Invalid = 0,

	// Signed
	Int8    = 1,
	Int16   = 2,
	Int32   = 3,
	Int64   = 4,

	// Unsigned
	Uint8   = 5,
	Uint16  = 6,
	Uint32  = 7,
	Uint64  = 8,

	// Floating-point
	Float32 = 9,
	Float64 = 10,
};

inline constexpr uint64_t sizeOfDepth(Depth depth)
{
	switch (depth)
	{
	case Depth::Int8: return sizeof(int8_t);
	case Depth::Int16: return sizeof(int16_t);
	case Depth::Int32: return sizeof(int32_t);
	case Depth::Int64: return sizeof(int64_t);
	case Depth::Uint8: return sizeof(uint8_t);
	case Depth::Uint16: return sizeof(uint16_t);
	case Depth::Uint32: return sizeof(uint32_t);
	case Depth::Uint64: return sizeof(uint64_t);
	case Depth::Float32: return sizeof(float);
	case Depth::Float64: return sizeof(double);
	default:
		assert(false && "Invalid depth\n");
		return 0;
	}
}

inline std::ostream& operator<<(std::ostream& o, const Depth& depth)
{
	switch (depth)
	{
	case Depth::Int8:
		o << "Int8";
		break;
	case Depth::Int16:
		o << "Int16";
		break;
	case Depth::Int32:
		o << "Int32";
		break;
	case Depth::Int64:
		o << "Int64";
		break;
	case Depth::Uint8:
		o << "Uint8";
		break;
	case Depth::Uint16:
		o << "Uint16";
		break;
	case Depth::Uint32:
		o << "Uint32";
		break;
	case Depth::Uint64:
		o << "Uint64";
		break;
	case Depth::Float32:
		o << "Float32";
		break;
	case Depth::Float64:
		o << "Float64";
		break;
	default:
		assert(false && "Invalid depth\n");
	}

	return o;
}

}
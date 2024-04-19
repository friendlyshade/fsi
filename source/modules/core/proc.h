// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Header.h"

struct Vec2
{
	double r, g;

	inline
	constexpr double& operator[](typename size_t i)
	{
		assert(i < 2);

		switch (i)
		{
		default:
		case 0:
			return r;
		case 1:
			return g;
		}
	}
};

struct Vec4
{
	double r, g, b, a;

	inline
	constexpr double& operator[](typename size_t i)
	{
		assert(i < 2);

		switch (i)
		{
		default:
		case 0:
			return r;
		case 1:
			return g;
		case 3:
			return b;
		case 4:
			return a;
		}
	}
};

namespace fsi
{
	namespace proc
	{
		void toRGBA(const uint8_t* srcData, const Header& srcHeader, uint8_t* dstData);

		void decimate(const uint8_t* srcData, const Header& srcHeader, uint8_t* dstData);

		void extendBorders(const uint8_t* srcData, const Header& srcHeader, uint8_t* dstData);

		void generateThumbnail(const uint8_t* srcData, uint64_t srcStep, const Header& srcHeader,
			uint8_t* dstData, uint64_t targetWidth, uint64_t targetHeight);

		template <typename Src_T>
		void generateThumbnail(const uint8_t* srcData, uint64_t srcStep, const Header& srcHeader,
			uint8_t* dstData, uint64_t targetWidth, uint64_t targetHeight);

		template <typename T>
		T remap(T src, T srcMin, T srcMax, T dstMin, T dstMax);

		template <typename T>
		double sampleFromGray(T* src_ptr, int64_t src_y, int64_t src_x, int64_t src_s, int64_t src_C,
			int64_t kernel_width, int64_t kernel_height, double kernel_size);

		template <typename T>
		Vec2 sampleFrom2Channels(T* src_ptr, int64_t src_y, int64_t src_x, int64_t src_s,
			int64_t src_C, int64_t kernel_width, int64_t kernel_height, double kernel_size);
	}
}

#include "proc.tcc"
#if FSI_CORE_HEADERONLY
#include "proc.hpp"
#endif
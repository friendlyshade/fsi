// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Header.h"
#include "../global.h"

namespace fsi
{
	namespace proc
	{
		struct Vec4
		{
			double r, g, b, a;

			inline
			constexpr double& operator[](typename size_t i)
			{
				assert(i < 4);

				switch (i)
				{
				default:
				case 0:
					return r;
				case 1:
					return g;
				case 2:
					return b;
				case 3:
					return a;
				}
			}
		};

		void generateThumbnail(const uint8_t* srcData, uint64_t srcWidth, uint64_t srcHeight,
			uint64_t srcChannels, Depth srcDepth, uint64_t srcStep, uint8_t* dstData, int64_t dstStep,
			uint64_t targetWidth, uint64_t targetHeight);

		template <typename Src_T, size_t Dst_C = 4>
		void generateThumbnail(const uint8_t* srcData, uint64_t srcWidth, uint64_t srcHeight,
			uint64_t srcChannels, uint64_t srcStep, uint8_t* dstData, int64_t dstStep,
			uint64_t targetWidth, uint64_t targetHeight);

		template <typename T>
		T remap(T src, T srcMin, T srcMax, T dstMin, T dstMax);

		template <typename Src_T, int64_t Dst_C>
		Vec4 sampleChannels(const Src_T* src_ptr, int64_t src_y, int64_t src_x, int64_t src_H,
			int64_t src_W, int64_t src_S, int64_t src_C, int64_t src_end, int64_t kernel_width,
			int64_t kernel_height, double kernel_size);
	}
}

#include "proc.tcc"
#if FSI_HEADERONLY
#include "proc.hpp"
#endif
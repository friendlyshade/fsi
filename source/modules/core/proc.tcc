// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "proc.h"
#include <algorithm>

template <typename Src_T>
inline
void fsi::proc::generateThumbnail(const uint8_t* srcData, uint64_t srcStep, const Header& srcHeader,
	uint8_t* dstData, uint64_t targetWidth, uint64_t targetHeight)
{
	typedef uint8_t Dst_T;
	const size_t Dst_C = 3;

	using std::min;
	using std::floor;
	using std::round;
	using std::clamp;

	// -- Create destination image  --

	// ...

	// -- Asserts --

	assert(targetWidth > 0 && "targetWidth must be greater than 0");
	assert(targetHeight > 0 && "targetHeight must be greater than 0");
	assert(targetWidth <= srcHeader.width && "targetWidth must be less or equal to src width");
	assert(targetHeight <= srcHeader.height && "targetHeight must be less or equal to src height");

	// -- Actual algorithm --

	double src_min = static_cast<double>(std::numeric_limits<Src_T>::lowest());
	double src_max = static_cast<double>(std::numeric_limits<Src_T>::max());
	double dst_min = static_cast<double>(std::numeric_limits<Dst_T>::lowest());
	double dst_max = static_cast<double>(std::numeric_limits<Dst_T>::max());

	const int64_t src_s = static_cast<int64_t>(srcStep);
	const int64_t dst_s = static_cast<int64_t>(targetWidth*Dst_C);
	const int64_t src_w = static_cast<int64_t>(srcHeader.width);
	const int64_t dst_w = static_cast<int64_t>(targetWidth);
	const int64_t src_h = static_cast<int64_t>(srcHeader.height);
	const int64_t dst_h = static_cast<int64_t>(targetHeight);
	const int64_t src_hs = src_h * src_s;
	const int64_t dst_hs = dst_h * dst_s;
	const int64_t src_wc = src_w * static_cast<int64_t>(srcHeader.channels);
	const int64_t dst_wc = dst_w * static_cast<int64_t>(Dst_C);

	const Src_T* src_ptr = reinterpret_cast<const Src_T*>(srcData);
	Dst_T* dst_ptr = reinterpret_cast<Dst_T*>(dstData);

	float width_factor = static_cast<float>(dst_w) / static_cast<float>(src_w);
	float height_factor = static_cast<float>(dst_h) / static_cast<float>(src_h);
	
	assert(width_factor <= 1.0f && "width_factor must be less or equal to 1");
	assert(height_factor <= 1.0f && "height_factor must be less or equal to 1");

	const int64_t kernel_width = round(1.0f / width_factor);
	const int64_t kernel_height = round(1.0f / height_factor);
	const int64_t kernel_size = kernel_width*kernel_height;

#pragma omp parallel for
	for (int64_t dst_ys = 0; dst_ys < dst_hs; dst_ys += dst_s)
	{
		int64_t src_y = static_cast<int64_t>(floor((dst_ys / dst_s) / height_factor));
		src_y = min(src_y, src_h - 1);

		for (int64_t dst_xc = 0; dst_xc < dst_wc; dst_xc += Dst_C)
		{
			int64_t src_x = static_cast<int64_t>(floor((dst_xc / Dst_C) / width_factor));
			src_x = min(src_x, src_w - 1);

			for (int64_t c = 0; c < Dst_C; c++)
			{
				const int64_t dst_idx = dst_ys + dst_xc + c;

				int64_t dst_size = static_cast<int64_t>((targetHeight*targetWidth + targetWidth)*Dst_C);

				assert((dst_idx >= 0) && "dst out of range");
				assert((dst_idx < dst_size) && "dst out of range");

				double accum = 0.0;
				for (int64_t ky = 0; ky < kernel_height; ++ky)
				for (int64_t kx = 0; kx < kernel_width; ++kx)
				{
					const int64_t src_idx = (src_y + ky)*src_s + (src_x + kx)*Dst_C + c;

					int64_t src_size = static_cast<int64_t>((srcHeader.height*srcHeader.width
						+ srcHeader.width) * srcHeader.channels);

					assert((src_idx >= 0) && "src out of range");
					assert((src_idx < src_size) && "src out of range");

					accum += static_cast<double>(src_ptr[src_idx]);
				}

				dst_ptr[dst_idx] = static_cast<Dst_T>(
						clamp(remap(accum / static_cast<double>(kernel_size),
							src_min, src_max, dst_min, dst_max), dst_min, dst_max)
					);
			}
		}
	}
}

template <typename T>
inline
T fsi::proc::remap(T src, T srcMin, T srcMax, T dstMin, T dstMax)
{
	return ((src - srcMin) / (srcMax - srcMin)) * (dstMax - dstMin) + dstMin;
}
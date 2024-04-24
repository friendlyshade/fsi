// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "proc.h"
#include <algorithm>
#include <iostream>

template <typename Src_T, size_t Dst_C>
inline
void fsi::proc::generateThumbnail(const uint8_t* srcData, uint64_t srcStep, const Header& srcHeader,
	uint8_t* dstData, uint64_t targetWidth, uint64_t targetHeight)
{
	typedef uint8_t Dst_T;

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

	const int64_t src_S = static_cast<int64_t>(srcStep);
	const int64_t dst_S = static_cast<int64_t>(targetWidth*Dst_C);
	const int64_t src_W = static_cast<int64_t>(srcHeader.width);
	const int64_t dst_W = static_cast<int64_t>(targetWidth);
	const int64_t src_H = static_cast<int64_t>(srcHeader.height);
	const int64_t dst_H = static_cast<int64_t>(targetHeight);
	const int64_t src_HS = src_H * src_S;
	const int64_t dst_HS = dst_H * dst_S;
	const int64_t src_C = static_cast<int64_t>(srcHeader.channels);
	const int64_t src_WC = src_W * static_cast<int64_t>(srcHeader.channels);
	const int64_t dst_WC = dst_W * static_cast<int64_t>(Dst_C);
	const int64_t src_end = static_cast<int64_t>(src_H*src_S);
	const int64_t dst_end = static_cast<int64_t>(targetHeight*targetWidth*Dst_C);

	const Src_T* src_ptr = reinterpret_cast<const Src_T*>(srcData);
	Dst_T* dst_ptr = reinterpret_cast<Dst_T*>(dstData);

	float width_factor = static_cast<float>(src_W) / static_cast<float>(dst_W);
	float height_factor = static_cast<float>(src_H) / static_cast<float>(dst_H);
	
	assert(width_factor >= 1.0f && "width_factor must be greater or equal to 1");
	assert(height_factor >= 1.0f && "height_factor must be greater or equal to 1");

	const int64_t kernel_width = round(width_factor);
	const int64_t kernel_height = round(height_factor);
	const double kernel_size = static_cast<double>(kernel_width*kernel_height);

#pragma omp parallel for
	for (int64_t dst_y = 0; dst_y < dst_H; dst_y++)
	{
		int64_t src_y = static_cast<int64_t>(floor(dst_y * height_factor));
		src_y = min(src_y, src_H - 1);

		for (int64_t dst_x = 0; dst_x < dst_W; dst_x++)
		{
			int64_t src_x = static_cast<int64_t>(floor(dst_x * width_factor));
			src_x = min(src_x, src_W - 1);

			const int64_t dst_idx = dst_y*dst_S + dst_x*Dst_C;

			assert((dst_idx >= 0) && "dst out of range");
			assert((dst_idx < dst_end) && "dst out of range");

			Vec4 result;
			result = sampleChannels<Src_T, Dst_C>(src_ptr, src_y, src_x, src_H, src_W, src_S, src_C,
				src_end, kernel_width, kernel_height, kernel_size);

			Dst_T result_cvt_r;
			Dst_T result_cvt_g;
			Dst_T result_cvt_b;
			Dst_T result_cvt_a;

			switch (src_C)
			{
				case 1:
				{
					result.r = clamp(remap(result.r, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);

					result_cvt_r = static_cast<Dst_T>(result.r);
					result_cvt_g = static_cast<Dst_T>(result.r);
					result_cvt_b = static_cast<Dst_T>(result.r);
					result_cvt_a = static_cast<Dst_T>(dst_max);

					break;
				}
				case 2:
				{
					result.r = clamp(remap(result.r, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);
					result.g = clamp(remap(result.g, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);

					result_cvt_r = static_cast<Dst_T>(result.r);
					result_cvt_g = static_cast<Dst_T>(result.g);
					result_cvt_b = static_cast<Dst_T>(dst_min);
					result_cvt_a = static_cast<Dst_T>(dst_max);

					break;
				}
				case 3:
				{
					result.r = clamp(remap(result.r, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);
					result.g = clamp(remap(result.g, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);
					result.b = clamp(remap(result.b, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);

					result_cvt_r = static_cast<Dst_T>(result.r);
					result_cvt_g = static_cast<Dst_T>(result.g);
					result_cvt_b = static_cast<Dst_T>(result.b);
					result_cvt_a = static_cast<Dst_T>(dst_max);

					break;
				}
				case 4:
				{
					result.r = clamp(remap(result.r, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);
					result.g = clamp(remap(result.g, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);
					result.b = clamp(remap(result.b, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);
					result.a = clamp(remap(result.a, src_min, src_max, dst_min, dst_max),
						dst_min, dst_max);

					result_cvt_r = static_cast<Dst_T>(result.r);
					result_cvt_g = static_cast<Dst_T>(result.g);
					result_cvt_b = static_cast<Dst_T>(result.b);
					result_cvt_a = static_cast<Dst_T>(result.a);

					break;
				}
			}

			dst_ptr[dst_idx    ] = result_cvt_r;
			dst_ptr[dst_idx + 1] = result_cvt_g;
			dst_ptr[dst_idx + 2] = result_cvt_b;
			dst_ptr[dst_idx + 3] = result_cvt_a;
		}
	}
}

/*inline
void fsi::proc::sampleRGBA(int64_t src_idx, int64_t dst_idx)
{
	for (int64_t c = 0; c < 4; c++)
	{
		// Handle missing channels for images with less than 4 channels
		if (c >= src_C)
		{
			Dst_T val;

			if (c == 4)
				val = dst_max; // Opaque if it's the alpha channel
			else
				val = dst_min; // Black otherwise

			dst_ptr[dst_idx] = val;

			continue;
		}

		assert((dst_idx >= 0) && "dst out of range");
		assert((dst_idx < dst_end) && "dst out of range");

		double accum = 0.0;
		for (int64_t ky = 0; ky < kernel_height; ++ky)
		for (int64_t kx = 0; kx < kernel_width; ++kx)
		{
			const int64_t src_idx = (src_y + ky)*src_s + (src_x + kx)*src_C + c;

			if (src_idx >= src_end)
			{
				std::cout << "c: " << c << "\n";
				std::cout << "src_s: " << src_s << "\n";
				std::cout << "src_y: " << src_y << "\n";
				std::cout << "src_x: " << src_x << "\n";
				std::cout << "ky: " << ky << "\n";
				std::cout << "kx: " << kx << "\n";
				std::cout << "src_idx: " << src_idx << "\n";
				exit(1);
			}

			assert((src_idx >= 0) && "src out of range");
			assert((src_idx < src_end) && "src out of range");

			accum += static_cast<double>(src_ptr[src_idx]);
		}

		dst_ptr[dst_idx] = static_cast<Dst_T>(
				clamp(remap(accum / kernel_size, src_min, src_max, dst_min, dst_max),
					dst_min, dst_max)
			);
	}
}*/

/*template <typename T>
inline
double fsi::proc::sampleFromGray(T* src_ptr, int64_t src_y, int64_t src_x, int64_t src_s, int64_t src_C,
	int64_t kernel_width, int64_t kernel_height, double kernel_size)
{
	using std::clamp;

	double accum = 0.0;
	for (int64_t ky = 0; ky < kernel_height; ++ky)
	for (int64_t kx = 0; kx < kernel_width; ++kx)
	{
		const int64_t src_idx = (src_y + ky)*src_s + (src_x + kx)*src_C;

		// assert((src_idx >= 0) && "src out of range");
		// assert((src_idx < src_end) && "src out of range");

		accum += static_cast<double>(src_ptr[src_idx]);
	}

	return accum / kernel_size;
}*/

template <typename Src_T, int64_t Dst_C>
inline
fsi::proc::Vec4 fsi::proc::sampleChannels(const Src_T* src_ptr, int64_t src_y, int64_t src_x, int64_t src_H,
	int64_t src_W, int64_t src_S, int64_t src_C, int64_t src_end, int64_t kernel_width,
	int64_t kernel_height, double kernel_size)
{
	using std::clamp;
	using std::min;

	assert(Dst_C <= 4 && "More than 4 channels is not supported");

	int64_t C = min(src_C, Dst_C);

	Vec4 result;
	for (int64_t c = 0; c < C; c++)
	{
		double accum = 0.0;
		for (int64_t ky = 0; ky < kernel_height; ++ky)
		for (int64_t kx = 0; kx < kernel_width; ++kx)
		{
			const int64_t src_idx =
				  min(src_y + ky, src_H - 1)*src_S
				+ min(src_x + kx, src_W - 1)*src_C
				+ c;

			assert((src_idx >= 0) && "src out of range");
			/*if (src_idx >= src_end)
			{
				std::cout << "src_idx out of range: " << src_idx << "\n";
				std::cout << "src_x: " << src_x << "\n";
				std::cout << "src_y: " << src_y << "\n";
				std::cout << "c: " << c << "\n";
				std::cout << "kx: " << kx << "\n";
				std::cout << "ky: " << ky << "\n";
				std::cout << "src_W: " << src_W << "\n";
				std::cout << "src_H: " << src_H << "\n";
				std::cout << "src_S: " << src_S << "\n";
				std::cout << "src_C: " << src_C << "\n";
			}*/
			assert((src_idx < src_end) && "src out of range");

			accum += static_cast<double>(src_ptr[src_idx]);
		}

		result[c] = accum / kernel_size;
	}

	return result;
}

template <typename T>
inline
T fsi::proc::remap(T src, T srcMin, T srcMax, T dstMin, T dstMax)
{
	return ((src - srcMin) / (srcMax - srcMin)) * (dstMax - dstMin) + dstMin;
}
// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Header.h"

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
	}
}

#include "proc.tcc"
#if FSI_CORE_HEADERONLY
#include "proc.hpp"
#endif
// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include "Depth.hpp"
#include "Writer.h"
#include "FormatVersion.h"
#include "Header.h"
#include "ProgressThread.h"
#include "Result.h"
#include <filesystem>
#include <fstream>

namespace fsi { class Writer_V2; }

class FSI_CORE_API fsi::Writer_V2 : public Writer
{
public:

	Writer_V2(const Header* header);

private:

	Result openImpl() override;

	Result writeImpl(const uint8_t* data, const std::atomic<bool>& paused,
		const std::atomic<bool>& canceled, std::atomic<float>& progress) override;

	uint32_t formatVersion() const override;

private:

	void calcThumbDimensions(uint32_t imageWidth, uint32_t imageHeight, uint16_t& thumbWidth,
		uint16_t& thumbHeight);
};

#if FSI_CORE_HEADERONLY
#include "Writer_V2.hpp"
#endif
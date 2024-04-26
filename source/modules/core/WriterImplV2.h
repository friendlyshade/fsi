// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include "Depth.hpp"
#include "WriterImpl.h"
#include "FormatVersion.h"
#include "Header.h"
#include "ProgressThread.h"
#include "Result.h"
#include <filesystem>
#include <fstream>

namespace fsi { class WriterImplV2; }

class FSI_CORE_API fsi::WriterImplV2 : public WriterImpl
{
public:

	FormatVersion formatVersion() override;

private:

	Result open(std::ofstream& file, Header& header) override;

	Result write(std::ofstream& file, const Header& header, const uint8_t* data,
		const std::atomic<bool>& paused, const std::atomic<bool>& canceled,
		std::atomic<float>& progress) override;

private:

	void calcThumbDimensions(uint32_t imageWidth, uint32_t imageHeight, uint16_t& thumbWidth,
		uint16_t& thumbHeight);
};

#if FSI_CORE_HEADERONLY
#include "WriterV2.hpp"
#endif
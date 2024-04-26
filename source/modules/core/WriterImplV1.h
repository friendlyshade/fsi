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

namespace fsi { class WriterImplV1; }

class FSI_CORE_API fsi::WriterImplV1 : public WriterImpl
{
public:

	FormatVersion formatVersion() override;

private:

	Result open(std::ofstream& file, Header& header) override;

	Result write(std::ofstream& file, const Header& header, const uint8_t* data,
		const std::atomic<bool>& paused, const std::atomic<bool>& canceled,
		std::atomic<float>& progress) override;
};

#if FSI_CORE_HEADERONLY
#include "WriterV1.hpp"
#endif
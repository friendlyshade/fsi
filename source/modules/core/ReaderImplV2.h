// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include "ReaderImpl.h"
#include "FormatVersion.h"
#include "Header.h"
#include "ProgressThread.h"
#include <filesystem>
#include <fstream>

namespace fsi { class ReaderImplV2; }

class FSI_CORE_API fsi::ReaderImplV2 : public ReaderImpl
{
public:

	ReaderImplV2();

public:

	FormatVersion formatVersion() override;

private:

	void open(std::ifstream& file, Header& header) override;

	void read(std::ifstream& file, const Header& header, uint8_t* data, uint8_t* thumbData,
		const std::atomic<bool>& paused, const std::atomic<bool>& canceled,
		std::atomic<float>& progress) override;

private:

	FSI_DISABLE_COPY_MOVE(ReaderImplV2);

};

#if FSI_CORE_HEADERONLY
#include "ReaderImplV2.hpp"
#endif
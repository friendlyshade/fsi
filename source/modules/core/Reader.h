// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include "Depth.hpp"
#include "FormatVersion.h"
#include "Header.h"
#include "ProgressThread.h"
#include "Result.h"
#include <filesystem>
#include <fstream>
#include <memory>

namespace fsi { class Reader; class ReaderImpl; }

class FSI_CORE_API fsi::Reader
{
public:

	Reader();

	~Reader();

public:

	Header header();

	virtual FormatVersion formatVersion();

public:

	Result open(const std::filesystem::path& path);

	Result read(uint8_t* data, uint8_t* thumbData = nullptr,
		ProgressThread::ReportProgressCB reportProgressCB = nullptr,
		void* reportProgressOpaquePtr = nullptr);

	void close();

private:

	std::unique_ptr<ReaderImpl> m_impl;

	FSI_DISABLE_COPY_MOVE(Reader);
};

#if FSI_CORE_HEADERONLY
#include "Reader.hpp"
#endif
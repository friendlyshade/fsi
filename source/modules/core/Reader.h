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

namespace fsi { class Reader; }

class FSI_CORE_API fsi::Reader
{
public:

	Reader();
	
	~Reader();

	Result open(const std::filesystem::path& path);

	Result read(uint8_t* data, uint8_t* thumbData = nullptr,
		ProgressThread::ReportProgressCB reportProgressCB = nullptr,
		void* reportProgressOpaquePtr = nullptr);

	void close();

public:

	Header header();

private:

	Header m_header;

	FormatVersion m_formatVersion;

	std::ifstream m_file;

	std::filesystem::path m_path;
};

#if FSI_CORE_HEADERONLY
#include "Reader.hpp"
#endif
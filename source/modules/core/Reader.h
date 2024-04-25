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
	
	virtual ~Reader();

public:

	Result open(const std::filesystem::path& path);

	Result read(uint8_t* data, uint8_t* thumbData = nullptr,
		ProgressThread::ReportProgressCB reportProgressCB = nullptr,
		void* reportProgressOpaquePtr = nullptr);

	void close();

public:

	virtual FormatVersion formatVersion() = 0;

private:

	virtual Result open(std::ifstream& file, Header& header) = 0;

	virtual Result read(std::ifstream& file, const Header& header, uint8_t* data, uint8_t* thumbData,
		const std::atomic<bool>& paused, const std::atomic<bool>& canceled,
		std::atomic<float>& progress) = 0;

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
// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include "../global.h"
#include "Depth.hpp"
#include "FormatVersion.h"
#include "Header.h"
#include "ProgressThread.h"
#include <filesystem>
#include <fstream>

namespace fsi { class WriterImpl; }

class FSI_CORE_API fsi::WriterImpl
{
public:

	WriterImpl();

	virtual ~WriterImpl();

public:

	Header header();

	virtual FormatVersion formatVersion() = 0;

public:

	void open(const std::filesystem::path& path, const Header& header);

	bool write(const uint8_t* data, ProgressThread::ReportProgressCB reportProgressCB = nullptr,
		void* reportProgressOpaquePtr = nullptr);

	void close();

protected:

	virtual void open(std::ofstream& file, Header& header) = 0;

	virtual void write(std::ofstream& file, const Header& header, const uint8_t* data,
		const std::atomic<bool>& paused, const std::atomic<bool>& canceled,
		std::atomic<float>& progress) = 0;

private:

	Header m_header;

	std::ofstream m_file;

	std::filesystem::path m_path;

	FSI_DISABLE_COPY_MOVE(WriterImpl);
};

#if FSI_HEADERONLY
#include "WriterImpl.hpp"
#endif
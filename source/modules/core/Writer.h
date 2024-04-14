// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include "Depth.h"
#include "FormatVersion.h"
#include "Header.h"
#include "ProgressThread.h"
#include "Result.h"
#include <filesystem>
#include <fstream>

namespace fsi { class Writer; }

class FSI_CORE_API fsi::Writer
{
public:

	Writer();

	~Writer();

	Result open(const std::filesystem::path& path, Header header,
		FormatVersion useFormatVersion = FormatVersion::Latest);

	/** @brief Write image data to FSI file.

	The function writes the image bytes and optionally a thumbnail to the file.

	@param data The image data.
	@param reportProgressCB The function is called when the progress of the operation is updated. It
	which can additionally be used for pausing, resuming and canceling the operation.
	@param reportProgressOpaquePtr Opaque pointer passed to reportProgressCB in case access to a member
	of an instance of opaquePointer is required.
	 */
	Result write(const uint8_t* data,
		ProgressThread::ReportProgressCB reportProgressCB = nullptr,
		void* reportProgressOpaquePtr = nullptr);

	void close();

private:

	Header m_header;

	FormatVersion m_formatVersion;

	std::ofstream m_file;

	std::filesystem::path m_path;
};

#if FSI_CORE_HEADERONLY
#include "Writer.hpp"
#endif
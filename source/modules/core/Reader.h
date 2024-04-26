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

	/** @brief Returns the header containing the image properties like dimensions, number of channels and
	bit-depth.
	*/
	Header header();

	/** @brief Returns the version of the FSI specification
	*/
	FormatVersion formatVersion();

public:
	/** @brief Opens an FSI file and reads the header information.

	@param path The path to the image file.
	*/
	Result open(const std::filesystem::path& path);

	/** @brief Reads image data from a FSI file.

	The function reads the image bytes and optionally a thumbnail from the file. If a thumbnail is
	present Header::hasThumb will be true.

	@param data The image data.
	@param reportProgressCB The function is called when the progress of the operation is updated. It can
	additionally be used for pausing, resuming and canceling the operation.
	@param reportProgressOpaquePtr Opaque pointer passed to reportProgressCB in case access to a member
	of an instance of opaquePointer is required.
	*/
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
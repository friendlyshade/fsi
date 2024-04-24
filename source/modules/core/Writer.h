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

namespace fsi { class Writer; class Writer; }

/*class FSI_CORE_API fsi::WriterCreator
{
public:

	Writer();

	~Writer();

public:

	fsi::Result open();

	void close();

private:

	std::unique_ptr<WriterImpl> m_impl;
};*/

class FSI_CORE_API fsi::Writer
{
public:
	/** @brief Creates a Writer
 
	@param useFormatVersion The version of the FSI specification to be used when writing the file. See
	#FormatVersion.
	*/
	static std::unique_ptr<fsi::Writer> createWriter(FormatVersion formatVersion);

public:

	Writer();

	virtual ~Writer();

public:

	/** @brief Creates an empty FSI file and writes the header information.

	@param path The path to the image file.
	@param header The header containing the image properties like dimensions, number of channels and
	bit-depth.
	*/
	Result open(const std::filesystem::path& path, const Header& header);

	/** @brief Writes image data to FSI file.

	The function write writes the image bytes and optionally a thumbnail to the file. The thumbnail is
	generated automatically from the image data.

	The step in bytes is calculated as follow:
	@code
		uint64_t step = width*channels + padding;
	@endcode
	If the image has no padding, then padding = 0. The size of the depth type is calculated with the
	sizeof operator as eg., sizeof(uint16_t) for Depth::Uint16, or sizeof(float) for Depth::Float32.

	@param data The image data.
	@param reportProgressCB The function is called when the progress of the operation is updated. It
	which can additionally be used for pausing, resuming and canceling the operation.
	@param reportProgressOpaquePtr Opaque pointer passed to reportProgressCB in case access to a member
	of an instance of opaquePointer is required.
	*/
	Result write(const uint8_t* data, ProgressThread::ReportProgressCB reportProgressCB = nullptr,
		void* reportProgressOpaquePtr = nullptr);

	void close();

protected:

	virtual Result openImpl() = 0;

	virtual Result writeImpl(const uint8_t* data, const std::atomic<bool>& paused,
		const std::atomic<bool>& canceled, std::atomic<float>& progress) = 0;

	virtual uint32_t formatVersion() const = 0;

protected:

	Header m_header;

	std::ofstream m_file;

	std::filesystem::path m_path;
};

#if FSI_CORE_HEADERONLY
#include "Writer.hpp"
#endif
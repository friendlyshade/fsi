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

namespace fsi { class Writer; }

class FSI_CORE_API fsi::Writer
{
public:

	Writer();

	~Writer();

	/** @brief Creates an empty FSI file and writes the header information.
	
	@param path The path to the image file.
	@param header The header containing the image properties like dimensions, number of channels and
	bit-depth.
	@param useFormatVersion The version of the FSI specification to be used when writing the file. See #FormatVersion.
	 */
	Result open(const std::filesystem::path& path, Header header,
		FormatVersion useFormatVersion = FormatVersion::Latest);

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
	@param step The image step/stride is the number of channels per pixel between the start of one row
	and the start of the next row in memory. Useful when the image is a shallow copy of another or the
	image has padding. A shallow copy is usually called "sub-image" or "sub-matrix" by image processing
	libraries. The step is usually calculated as: width*channels + padding. If 0 is passed as the step,
	it will be calculated from the header information as: width*channels without padding.
	@param thumbnail Whether to generate a thumbnail from the image data (if set to true, the write
	process will take some extra due to the generation of the thumbnail).
	@param reportProgressCB The function is called when the progress of the operation is updated. It
	which can additionally be used for pausing, resuming and canceling the operation.
	@param reportProgressOpaquePtr Opaque pointer passed to reportProgressCB in case access to a member
	of an instance of opaquePointer is required.
	 */
	Result write(const uint8_t* data, uint64_t step, bool thumbnail = false,
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
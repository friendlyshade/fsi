// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Reader.h"
#include "consts.h"
#include "ReaderImpl.h"
#include "ReaderImplV1.h"
#include "ReaderImplV2.h"
#include "exceptions.hpp"
#include <iostream>
#include <atomic>
#include <string>

FSI_INLINE_HPP
fsi::Reader::Reader()
{
}

FSI_INLINE_HPP
fsi::Reader::~Reader()
{
}

FSI_INLINE_HPP
fsi::FormatVersion fsi::Reader::formatVersionFromFile(const std::filesystem::path& path)
{
	// Check file extension
	if (path.extension() != expectedFileExtension)
		throw ExceptionInvalidFileExtension();

	// Open file
	std::ifstream file = std::ifstream(path, std::ios::binary);
	if (file.fail())
		throw ExceptionFailedToOpenFile();

	// Read and check signature
	const uint8_t formatSignature[sizeof(expectedFormatSignature)] = {};
	{
		file.read((char*)formatSignature, sizeof(expectedFormatSignature));

		for (size_t c = 0; c < sizeof(expectedFormatSignature); c++)
		{
			if (formatSignature[c] != expectedFormatSignature[c])
			{
				file.close();
				throw ExceptionInvalidSignature();
			}
		}
	}

	// Read the version of the file specification
	FormatVersion formatVersion;
	file.read((char*)(&formatVersion), sizeof(uint32_t));

	file.close();

	return formatVersion;
}

FSI_INLINE_HPP
fsi::Header fsi::Reader::header()
{
	assert(m_impl && "The file must be opened before accessing the Header");
	return m_impl->header();
}

FSI_INLINE_HPP
fsi::FormatVersion fsi::Reader::formatVersion()
{
	assert(m_impl && "The file must be opened before accessing the Format Version");
	return m_impl->formatVersion();
}

FSI_INLINE_HPP
void fsi::Reader::open(const std::filesystem::path& path)
{
	FormatVersion formatVersion = formatVersionFromFile(path);

	switch (formatVersion)
	{
	case fsi::FormatVersion::V1:
		m_impl = std::make_unique<ReaderImplV1>();
		break;
	case fsi::FormatVersion::V2:
		m_impl = std::make_unique<ReaderImplV2>();
		break;
	default:
		throw ExceptionInvalidFormatVersion("Version "
			+ std::to_string(static_cast<uint32_t>(formatVersion))
			+ " is not a valid FSI format version");
	}

	m_impl->open(path);
}

FSI_INLINE_HPP
bool fsi::Reader::read(uint8_t* data, uint8_t* thumbData,
	ProgressThread::ReportProgressCB reportProgressCB, void* reportProgressOpaquePtr)
{
	if (!m_impl)
		throw ExceptionFileIsNotOpen("The file must be opened before reading can be attempted");
	return m_impl->read(data, thumbData, reportProgressCB, reportProgressOpaquePtr);
}

FSI_INLINE_HPP
void fsi::Reader::close()
{
	if (!m_impl)
		return;
	return m_impl->close();
}
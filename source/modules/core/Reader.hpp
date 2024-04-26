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
#include <iostream>
#include <atomic>
#include <string>

fsi::Reader::Reader()
{
}

fsi::Reader::~Reader()
{
}

fsi::Header fsi::Reader::header()
{
	return m_impl->header();
}

fsi::FormatVersion fsi::Reader::formatVersion()
{
	return m_impl->formatVersion();
}

fsi::Result fsi::Reader::open(const std::filesystem::path& path)
{
	// Check file extension
	if (path.extension() != expectedFileExtension)
		return Result::Code::InvalidFileExtension;

	// Open file
	std::ifstream file = std::ifstream(path, std::ios::binary);
	if (file.fail())
		return Result::Code::FailedToOpenFile;

	// Read and check signature
	const uint8_t formatSignature[sizeof(expectedFormatSignature)] = {};
	{
		file.read((char*)formatSignature, sizeof(expectedFormatSignature));

		for (size_t c = 0; c < sizeof(expectedFormatSignature); c++)
		{
			if (formatSignature[c] != expectedFormatSignature[c])
			{
				close();
				return Result::Code::InvalidSignature;
			}
		}
	}

	// Read the version of the file specification
	FormatVersion formatVersion;
	file.read((char*)(&formatVersion), sizeof(uint32_t));

	file.close();

	switch (formatVersion)
	{
	case fsi::FormatVersion::V1:
		m_impl = std::make_unique<ReaderImplV1>();
		break;
	case fsi::FormatVersion::V2:
		m_impl = std::make_unique<ReaderImplV2>();
		break;
	default:
		return { Result::Code::InvalidFormatVersion, "Version "
			+ std::to_string(static_cast<uint32_t>(formatVersion))
			+ " is not a valid FSI format version" };
		break;
	}

	return m_impl->open(path);
}

fsi::Result fsi::Reader::read(uint8_t* data, uint8_t* thumbData,
	ProgressThread::ReportProgressCB reportProgressCB, void* reportProgressOpaquePtr)
{
	return m_impl->read(data, thumbData, reportProgressCB, reportProgressOpaquePtr);
}

void fsi::Reader::close()
{
	return m_impl->close();
}
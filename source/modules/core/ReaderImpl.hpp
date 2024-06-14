// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "ReaderImpl.h"
#include "ReaderImplV1.h"
#include "ReaderImplV2.h"
#include "consts.h"
#include <iostream>
#include <atomic>
#include <string>

FSI_INLINE_HPP
fsi::ReaderImpl::ReaderImpl()
{
}

FSI_INLINE_HPP
fsi::ReaderImpl::~ReaderImpl()
{
	close();
}

FSI_INLINE_HPP
fsi::Header fsi::ReaderImpl::header()
{
	if (!m_file.is_open())
		throw ExceptionFileIsNotOpen("The file must be opened before accessing the Header");
	return m_header;
}

FSI_INLINE_HPP
void fsi::ReaderImpl::open(const std::filesystem::path& path)
{
	// Check file extension
	if (path.extension() != expectedFileExtension)
		throw ExceptionInvalidFileExtension();

	// Store path
	m_path = path;

	// Open file
	m_file = std::ifstream(path, std::ios::binary);
	if (m_file.fail())
		throw ExceptionFailedToOpenFile();

	// Read and check signature
	const uint8_t formatSignature[sizeof(expectedFormatSignature)] = {};
	{
		m_file.read((char*)formatSignature, sizeof(expectedFormatSignature));

		for (size_t c = 0; c < sizeof(expectedFormatSignature); c++)
		{
			if (formatSignature[c] != expectedFormatSignature[c])
			{
				close();
				throw ExceptionInvalidSignature();
			}
		}
	}

	// Read the version of the file specification
	FormatVersion formatVersionFromFile;
	m_file.read((char*)(&formatVersionFromFile), sizeof(uint32_t));
	
	switch (formatVersionFromFile)
	{
	case fsi::FormatVersion::V1:
		if (formatVersion() != formatVersionFromFile)
			ExceptionUnexpectedFormatVersion("Concrete instance of ReaderImpl must be ReaderImplV1");
		break;
	case fsi::FormatVersion::V2:
		if (formatVersion() != formatVersionFromFile)
			ExceptionUnexpectedFormatVersion("Concrete instance of ReaderImpl must be ReaderImplV2");
		break;
	default:
		throw ExceptionInvalidFormatVersion("Version "
			+ std::to_string(static_cast<uint32_t>(formatVersionFromFile))
			+ " is not a valid FSI format version");
	}

	// Read the rest of the header specific to the file version
	try
	{
		open(m_file, m_header);
	}
	catch (...)
	{
		// Close file and rethrow the exception
		close();
		throw;
	}
}

FSI_INLINE_HPP
bool fsi::ReaderImpl::read(uint8_t* data, uint8_t* thumbData,
	ProgressThread::ReportProgressCB reportProgressCB, void* reportProgressOpaquePtr)
{
	if (!m_file.is_open())
		throw ExceptionFileIsNotOpen("The file must be opened before reading can be attempted");

	std::atomic<bool> canceled = false;
	std::atomic<bool> paused = false;
	std::atomic<float> progress = 0.0f;
	ProgressThread progressThread(reportProgressOpaquePtr, reportProgressCB,
		[&progress]() { return progress.load(std::memory_order_relaxed); },
		[&canceled]() { canceled = true; },
		[&paused]() { paused = true; },
		[&paused]() { paused = false; },
		progressCallbackInterval);

	// Read the data specific to the file version
	try
	{
		read(m_file, m_header, data, thumbData, paused, canceled, progress);
	}
	catch (...)
	{
		// Join the progress thread, close file and rethrow the exception
		progressThread.join(false);
		close();
		throw;
	}

	progressThread.join(!canceled);
	close();
	return canceled;
}

FSI_INLINE_HPP
void fsi::ReaderImpl::close()
{
	if (m_file.is_open())
		m_file.close();
}
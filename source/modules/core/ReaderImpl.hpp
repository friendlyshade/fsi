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
#include <stdexcept>

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

namespace
{
    inline uint64_t fsiImageDataOffset(const fsi::FormatVersion formatVersion)
    {
        switch (formatVersion)
        {
        case fsi::FormatVersion::V1:
            return
                sizeof(fsi::expectedFormatSignature) +
                sizeof(uint32_t) + // version
                sizeof(uint32_t) + // width
                sizeof(uint32_t) + // height
                sizeof(uint32_t) + // channels
                sizeof(uint32_t);  // depth

        case fsi::FormatVersion::V2:
            return
                sizeof(fsi::expectedFormatSignature) +
                sizeof(uint32_t) + // version
                sizeof(uint32_t) + // width
                sizeof(uint32_t) + // height
                sizeof(uint32_t) + // channels
                sizeof(uint8_t)  + // depth
                sizeof(uint8_t)  + // hasThumb
                sizeof(uint16_t) + // thumbWidth
                sizeof(uint16_t) + // thumbHeight
                fsi::thumbSizeInBytes;

        default:
            throw fsi::ExceptionInvalidFormatVersion(
                "Invalid FSI format version while reading rectangle."
            );
        }
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
bool fsi::ReaderImpl::readRect(
    uint8_t* data,
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height
)
{
    if (!m_file.is_open())
        throw ExceptionFileIsNotOpen("The file must be opened before reading can be attempted");

    const uint64_t bytesPerPixel =
        static_cast<uint64_t>(m_header.channels) * sizeOfDepth(m_header.depth);

    return readRect(
        data,
        x,
        y,
        width,
        height,
        static_cast<uint64_t>(width) * bytesPerPixel
    );
}

FSI_INLINE_HPP
bool fsi::ReaderImpl::readRect(
    uint8_t* data,
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint64_t dstStrideBytes
)
{
    if (!m_file.is_open())
        throw ExceptionFileIsNotOpen("The file must be opened before reading can be attempted");

    if (!data)
        throw std::runtime_error("Cannot read rectangle into a null data pointer.");

    if (width == 0 || height == 0)
        throw std::runtime_error("Rectangle width and height must be greater than zero.");

    if (m_header.width == 0 || m_header.height == 0 || m_header.channels == 0)
        throw std::runtime_error("Invalid FSI header while reading rectangle.");

    const uint64_t rectEndX =
        static_cast<uint64_t>(x) + static_cast<uint64_t>(width);

    const uint64_t rectEndY =
        static_cast<uint64_t>(y) + static_cast<uint64_t>(height);

    if (x >= m_header.width ||
        y >= m_header.height ||
        rectEndX > m_header.width ||
        rectEndY > m_header.height)
    {
        throw std::runtime_error("Requested rectangle is outside the image bounds.");
    }

    const uint64_t bytesPerChannel =
        sizeOfDepth(m_header.depth);

    if (bytesPerChannel == 0)
        throw std::runtime_error("Invalid image depth while reading rectangle.");

    const uint64_t bytesPerPixel =
        static_cast<uint64_t>(m_header.channels) * bytesPerChannel;

    const uint64_t sourceRowSize =
        static_cast<uint64_t>(m_header.width) * bytesPerPixel;

    const uint64_t targetRowSize =
        static_cast<uint64_t>(width) * bytesPerPixel;

    if (dstStrideBytes < targetRowSize)
        throw std::runtime_error("Destination stride is smaller than the rectangle row size.");

    const uint64_t imageDataOffset =
        fsiImageDataOffset(formatVersion());

    for (uint32_t row = 0; row < height; ++row)
    {
        const uint64_t sourceOffset =
            imageDataOffset +
            static_cast<uint64_t>(y + row) * sourceRowSize +
            static_cast<uint64_t>(x) * bytesPerPixel;

        uint8_t* targetRow =
            data + static_cast<uint64_t>(row) * dstStrideBytes;

        m_file.clear();

        m_file.seekg(
            static_cast<std::streamoff>(sourceOffset),
            std::ios::beg
        );

        if (!m_file)
            throw std::runtime_error("Failed to seek while reading FSI rectangle.");

        m_file.read(
            reinterpret_cast<char*>(targetRow),
            static_cast<std::streamsize>(targetRowSize)
        );

        if (!m_file)
            throw std::runtime_error("Failed to read row while reading FSI rectangle.");
    }

    return true;
}

FSI_INLINE_HPP
void fsi::ReaderImpl::close()
{
	if (m_file.is_open())
		m_file.close();
}
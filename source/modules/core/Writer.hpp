// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Writer.h"
#include "WriterImpl.h"
#include "WriterImplV1.h"
#include "WriterImplV2.h"
#include "consts.h"
#include "proc.h"
#include <iostream>
#include <atomic>
#include <algorithm>
#include <exception>

#define WRITE_THUMB_AS_FILE 0

#if WRITE_THUMB_AS_FILE
	size_t thumbWidth = 256;
	size_t thumbHeight = 256;
	size_t thumbChannels = 4;
	fsi::Depth thumbDepth = fsi::Depth::Uint8;

	fsi::Header originalHeader;
#endif

fsi::Writer::Writer(FormatVersion formatVersion)
{
	switch (formatVersion)
	{
	case fsi::FormatVersion::V1:
		m_impl = std::make_unique<WriterImplV1>();
		return;
	case fsi::FormatVersion::V2:
		m_impl = std::make_unique<WriterImplV2>();
		return;
	}

	throw std::exception("Header must be castable to Header_V1 or Header_V2");
}

fsi::Writer::~Writer()
{
}

fsi::Result fsi::Writer::open(const std::filesystem::path& path, const Header& header)
{
	return m_impl->open(path, header);
}

fsi::Result fsi::Writer::write(const uint8_t* data, ProgressThread::ReportProgressCB reportProgressCB,
	void* reportProgressOpaquePtr)
{
	return m_impl->write(data, reportProgressCB, reportProgressOpaquePtr);
}

void fsi::Writer::close()
{
	m_impl->close();
}
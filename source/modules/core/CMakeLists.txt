# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FSI.
# FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
# file, you can obtain one at https://opensource.org/license/mit.

helper_add_library(core
	OUTPUT_NAME Core
	BUILD_TYPE "${FSI_BUILD_TYPE}"
	FOLDER "modules"
	LINK_SCOPE "${FSI_LINK_SCOPE}"
	PUBLIC_HEADERS
		"consts.h"
		"Depth.hpp"
		"Exception.h"
		"Exception.inl"
		"exceptions.hpp"
		"FormatVersion.h"
		"Header.h"
		"Reader.h"
		"Reader.hpp"
		"Writer.h"
		"Writer.hpp"
		"ProgressThread.h"
		"ProgressThread.hpp"
	PRIVATE_HEADERS
		"ReaderImpl.h"
		"ReaderImpl.hpp"
		"ReaderImplV1.h"
		"ReaderImplV1.hpp"
		"ReaderImplV2.h"
		"ReaderImplV2.hpp"
		"WriterImpl.h"
		"WriterImpl.hpp"
		"WriterImplV1.h"
		"WriterImplV1.hpp"
		"WriterImplV2.h"
		"WriterImplV2.hpp"
		"Timer.h"
		"Timer.hpp"
		"proc.h"
		"proc.hpp"
		"proc.tcc"
	SOURCES
		"src/ProgressThread.cpp"
		"src/Reader.cpp"
		"src/ReaderImpl.cpp"
		"src/ReaderImplV1.cpp"
		"src/ReaderImplV2.cpp"
		"src/Writer.cpp"
		"src/WriterImpl.cpp"
		"src/WriterImplV1.cpp"
		"src/WriterImplV2.cpp"
		"src/proc.cpp"
		"src/Timer.cpp"
)
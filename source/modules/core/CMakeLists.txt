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
		"FormatVersion.h"
		"Header.h"
		"ProgressThread.h"
		"ProgressThread.hpp"
		"Reader.h"
		"Reader.hpp"
		"Result.h"
		"Result.hpp"
		"Writer.h"
		"Writer.hpp"
		"Writer_V1.h"
		"Writer_V1.hpp"
		"Writer_V2.h"
		"Writer_V2.hpp"
		"proc.h"
		"proc.hpp"
		"proc.tcc"
		"Timer.h"
		"Timer.hpp"
	SOURCES
		"src/ProgressThread.cpp"
		"src/Reader.cpp"
		"src/Result.cpp"
		"src/Writer.cpp"
		"src/Writer_V1.cpp"
		"src/Writer_V2.cpp"
		"src/proc.cpp"
		"src/Timer.cpp"
)
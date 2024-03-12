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
		"Depth.h"
		"FormatVersion.h"
		"Header.h"
		"Reader.h"
		"Reader.hpp"
		"Result.h"
		"Result.hpp"
		"Writer.h"
		"Writer.hpp"
	SOURCES
		"src/Reader.cpp"
		"src/Result.cpp"
		"src/Writer.cpp"
)
// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#define FSI_DISABLE_COPY(X)      \
    X(const X &) = delete;       \
    X &operator=(const X &) = delete;
#define FSI_DISABLE_MOVE(X)      \
    X(X &&) = delete;            \
    X &operator=(X &&) = delete;
#define FSI_DISABLE_COPY_MOVE(X) \
	FSI_DISABLE_COPY(X)          \
	FSI_DISABLE_MOVE(X)

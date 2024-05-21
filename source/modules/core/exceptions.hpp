// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Exception.h"

namespace fsi
{
	class ExceptionUnexpectedFormatVersion;
	class ExceptionInvalidFormatVersion;
	class ExceptionInvalidFileExtension;
	class ExceptionInvalidSignature;
	class ExceptionInvalidThumbnailWidth;
	class ExceptionInvalidThumbnailHeight;
	class ExceptionInvalidImageWidth;
	class ExceptionInvalidImageHeight;
	class ExceptionInvalidImageChannels;
	class ExceptionInvalidImageDepth;
	class ExceptionFailedToOpenFile;
	class ExceptionFailedToCreateFile;
	class ExceptionFileIsNotOpen;
}

class fsi::ExceptionUnexpectedFormatVersion : public Exception
{
public:
	ExceptionUnexpectedFormatVersion() : Exception() {}

	ExceptionUnexpectedFormatVersion(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Concrete instance of implementation class and FormatVersion don't match";
	}
};

class fsi::ExceptionInvalidFormatVersion : public Exception
{
public:
	ExceptionInvalidFormatVersion() : Exception() {}

	ExceptionInvalidFormatVersion(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Format version not supported or file is malformed/corrupted";
	}
};

class fsi::ExceptionInvalidFileExtension : public Exception
{
public:
	ExceptionInvalidFileExtension() : Exception() {}

	ExceptionInvalidFileExtension(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "\".fsi\" file extension expected";
	}
};

class fsi::ExceptionInvalidSignature : public Exception
{
public:
	ExceptionInvalidSignature() : Exception() {}

	ExceptionInvalidSignature(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Wrong signature or file is malformed/corrupted";
	}
};

class fsi::ExceptionInvalidThumbnailWidth : public Exception
{
public:
	ExceptionInvalidThumbnailWidth() : Exception() {}

	ExceptionInvalidThumbnailWidth(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Wrong thumbnail width";
	}
};

class fsi::ExceptionInvalidThumbnailHeight : public Exception
{
public:
	ExceptionInvalidThumbnailHeight() : Exception() {}

	ExceptionInvalidThumbnailHeight(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Wrong thumbnail height";
	}
};

class fsi::ExceptionInvalidImageWidth : public Exception
{
public:
	ExceptionInvalidImageWidth() : Exception() {}

	ExceptionInvalidImageWidth(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Wrong width";
	}
};

class fsi::ExceptionInvalidImageHeight : public Exception
{
public:
	ExceptionInvalidImageHeight() : Exception() {}

	ExceptionInvalidImageHeight(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Wrong height";
	}
};

class fsi::ExceptionInvalidImageChannels : public Exception
{
public:
	ExceptionInvalidImageChannels() : Exception() {}

	ExceptionInvalidImageChannels(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Wrong channels";
	}
};

class fsi::ExceptionInvalidImageDepth : public Exception
{
public:
	ExceptionInvalidImageDepth() : Exception() {}

	ExceptionInvalidImageDepth(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "Wrong depth";
	}
};

class fsi::ExceptionFailedToOpenFile : public Exception
{
public:
	ExceptionFailedToOpenFile() : Exception() {}

	ExceptionFailedToOpenFile(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "File doesn't exist or could not be opened";
	}
};

class fsi::ExceptionFailedToCreateFile : public Exception
{
public:
	ExceptionFailedToCreateFile() : Exception() {}

	ExceptionFailedToCreateFile(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "File could not be created";
	}
};

class fsi::ExceptionFileIsNotOpen : public Exception
{
public:
	ExceptionFileIsNotOpen() : Exception() {}

	ExceptionFileIsNotOpen(const std::string& whatDetails) : Exception(whatDetails) {}

	const char* what() const noexcept override
	{
		return "The file must be opened";
	}
};
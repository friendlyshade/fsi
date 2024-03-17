// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include <string>


namespace fsi { class Result; }

class FSI_CORE_API fsi::Result
{

public:

	enum class Code : uint32_t
	{
		Success,
		Canceled,
		WrongFormatVersion,
		WrongFileExtension,
		WrongSignature,
		WrongDepth,
		WrongChannels,
		WrongWidth,
		WrongHeight,
		FailedToOpen,
		FailedToCreate,
		FileIsNotOpen,
	};

public:

	Result();

	Result(Code code);

	Result(Code code, const std::string& messageDetails);

	~Result();

public:

	Code code() const;

	std::string message() const;

	void setMessageDetails(const std::string& messageDetails);

public:

	// -- operators --

	operator std::string() const
	{
		return message();
	}

	bool operator==(const Result& rhs) const
	{
		return m_code == rhs.m_code;
	}

	bool operator!=(const Result& rhs) const
	{
		return m_code != rhs.m_code;
	}

private:

	Code m_code;

	std::string m_messageDetails;

};

#if FSI_CORE_HEADERONLY
#include "Result.hpp"
#endif
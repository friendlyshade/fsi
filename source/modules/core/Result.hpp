// � 2023 Friendly Shade, Inc.
// � 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#include "Result.h"
#include <assert.h>


fsi::Result::Result()
	: m_code(Code::Success)
	, m_messageDetails("")
{
}


fsi::Result::Result(Code code)
	: m_code(code)
	, m_messageDetails("")
{
}


fsi::Result::Result(Code code, const std::string& messageDetails)
	: m_code(code)
	, m_messageDetails(messageDetails)
{
}


fsi::Result::~Result()
{
}


fsi::Result::Code fsi::Result::code() const
{
	return m_code;
}


std::string fsi::Result::message() const
{
	std::string message;

	switch (m_code)
	{
	case Code::Success:
		message = "Success";
		break;
	case Code::Canceled:
		message = "Canceled";
		break;
	case Code::WrongFormatVersion:
		message = "Format version not supported or file is malformed/corrupted";
		break;
	case Code::WrongFileExtension:
		message = "\".fsi\" file extension expected";
		break;
	case Code::WrongSignature:
		message = "Wrong signature or file is malformed/corrupted";
		break;
	case Code::WrongDepth:
		message = "Wrong depth";
		break;
	case Code::WrongChannels:
		message = "Wrong channels";
		break;
	case Code::WrongWidth:
		message = "Wrong width";
		break;
	case Code::WrongHeight:
		message = "Wrong height";
		break;
	case Code::FailedToOpen:
		message = "File doesn't exist or could not be opened";
		break;
	case Code::FailedToCreate:
		message = "File could not be created";
		break;
	case Code::FileIsNotOpen:
		message = "File must be opened before attempting to read";
		break;
	default:
		assert(false && "Internal error: Result code not supported.");
	}

	return message + (m_messageDetails.empty() ? "" : ": ") + m_messageDetails;
}


void fsi::Result::setMessageDetails(const std::string& messageDetails)
{
	m_messageDetails = messageDetails;
}
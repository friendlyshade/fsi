// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "Exception.h"
#include <assert.h>

inline
fsi::Exception::Exception()
	: Exception("")
{
}

inline
fsi::Exception::Exception(const std::string& whatDetails)
	: m_whatDetails(whatDetails)
{
}

inline
void fsi::Exception::setWhatDetails(const std::string& whatDetails)
{
	m_whatDetails = whatDetails;
}

inline
std::string fsi::Exception::whatDetails() const
{
	return m_whatDetails;
}

inline
std::ostream& operator<<(std::ostream& os, const fsi::Exception& exception)
{
	os << "Exception: " << exception.what();
	
	if (!exception.whatDetails().empty())
		os << "\nException details: " << exception.whatDetails();

	return os;
}
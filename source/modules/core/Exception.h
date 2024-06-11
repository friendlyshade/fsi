// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include "fsi_core_exports.h"
#include <string>
#include <stdexcept>

namespace fsi { class Exception; }

class FSI_CORE_API fsi::Exception : public std::exception
{
public:

	Exception();

	Exception(const std::string& whatDetails);

public:

	void setWhatDetails(const std::string& whatDetails);

	std::string whatDetails() const;

private:

	std::string m_whatDetails;

};

std::ostream& operator<<(std::ostream& os, const fsi::Exception& exception);

#include "Exception.inl"
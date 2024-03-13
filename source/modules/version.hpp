// © 2023 Friendly Shade, Inc.
// © 2023 Sebastian Zapata
//
// This file is part of FSI.
// FSI is licensed under The MIT License. If a copy of The MIT License was not distributed with this
// file, you can obtain one at https://opensource.org/license/mit.

#pragma once

#include <ostream>

#define FSI_VERSION_MAJOR 3
#define FSI_VERSION_MINOR 1
#define FSI_VERSION_PATCH 0

// Preprocessor for string conversion
#define FSI_TO_STRING_HELPER(x) #x
#define FSI_TO_STRING(x) FSI_TO_STRING_HELPER(x)

// Get version as string. For example "5.2.0"
#define FSI_VERSION_STRING FSI_TO_STRING(FSI_VERSION_MAJOR) "." \
                           FSI_TO_STRING(FSI_VERSION_MINOR) "." \
                           FSI_TO_STRING(FSI_VERSION_PATCH)

namespace fsi {

class Version
{
public:

	Version()
		: m_major(FSI_VERSION_MAJOR)
		, m_minor(FSI_VERSION_MINOR)
		, m_patch(FSI_VERSION_PATCH)
	{
	}

    Version(int major, int minor, int patch)
        : m_major(major)
        , m_minor(minor)
        , m_patch(patch)
    {
    }

    bool operator<(const Version& other) const
    {
		if (*this == other)
			return false;

		if (m_major < other.m_major)
			return true;

		if (m_major > other.m_major)
			return false;

		if (m_minor < other.m_minor)
			return true;

		if (m_minor > other.m_minor)
			return false;

		if (m_patch < other.m_patch)
			return true;

		if (m_patch > other.m_patch)
			return false;

		return false;
    }

	bool operator>(const Version& other) const
	{
		if (*this == other)
			return false;

		if (m_major > other.m_major)
			return true;

		if (m_major < other.m_major)
			return false;

		if (m_minor > other.m_minor)
			return true;

		if (m_minor < other.m_minor)
			return false;

		if (m_patch > other.m_patch)
			return true;

		if (m_patch < other.m_patch)
			return false;

		return false;
	}

	bool operator==(const Version& other) const
	{
		if (m_major != other.m_major
		 || m_minor != other.m_minor
		 || m_patch != other.m_patch)
		{
			return false;
		}

		return true;
	}

	bool operator!=(const Version& other) const
	{
		return !(*this == other);
	}

	friend std::ostream& operator<<(std::ostream& os, const Version& version)
	{
		os << version.m_major << "." << version.m_minor << "." << version.m_patch;
		return os;
	}

private:

    int m_major;

    int m_minor;

    int m_patch;

};

}

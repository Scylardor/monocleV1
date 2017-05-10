#include "LogPolicies/FileWritePolicy.h"

namespace moe
{
    FileWritePolicy::FileWritePolicy() :
		m_fileHandle(nullptr)
    {
    }

	FileWritePolicy::FileWritePolicy(const char* filename)
	{
		OpenFile(filename);
	}

	FileWritePolicy::FileWritePolicy(FileWritePolicy&& rPolicy)
	{
		// steal the file handle
		m_fileHandle = rPolicy.m_fileHandle;
		rPolicy.m_fileHandle = nullptr;
	}

    FileWritePolicy::~FileWritePolicy()
    {
        Close();
    }

    void    FileWritePolicy::OpenFile(const char* filename)
    {
        // TODO: assert filename != nullptr
        m_fileHandle = fopen(filename, "w");
        // TODO: assert fileHandle != nullptr
    }

    void    FileWritePolicy::Close()
    {
        if (HasOpenFile())
        {
            fclose(m_fileHandle);
			m_fileHandle = nullptr;
        }
    }

    void    FileWritePolicy::Write(const std::string& message)
    {
        if (HasOpenFile())
        {
            fwrite(message.c_str(), sizeof(char), message.length(), m_fileHandle);
        }
    }

    bool    FileWritePolicy::HasOpenFile() const
    {
        return (m_fileHandle != nullptr);
    }
}

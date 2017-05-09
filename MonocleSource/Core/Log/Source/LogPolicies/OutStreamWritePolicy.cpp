#include "LogPolicies/OutStreamWritePolicy.h"

namespace moe
{
    OutStreamWritePolicy::OutStreamWritePolicy(std::ostream& outStream) :
        m_ostream(outStream)
    {
    }

    void    OutStreamWritePolicy::Write(const std::string& message)
    {
        m_ostream << message.c_str();
    }

}
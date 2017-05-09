#ifndef MOE_DEFAULT_LOGGER_H_
#define MOE_DEFAULT_LOGGER_H_

#include <memory>
#include "moeDLLVisibility.h"

class LoggerBase;

namespace moe
{
    // The default logger is a kind of "dynamic" singleton: a wrapper around a static LoggerBase unique ptr.
    // It will be the logger used "by default" by all logging commands.
    // But it comes uninitialized: you need to initialize it with Set() before it does anything.
    class DefaultLogger
    {
    public:
        MOE_DLL_API LoggerBase*  Get()
        {
            return m_defaultLogger.get();
        }

        // NewLoggerType must be a child of LoggerBase.
        // You can pass policies parameters in args.
        template <typename NewLoggerType, typename... Args>
        MOE_DLL_API LoggerBase*  Set(Args&&... args)
        {
            static_assert(std::is_base_of<LoggerBase, NewLoggerType>::value, "DefaultLogger only works with base/derived related classes");

            // Relink properly if part of a list, because when setting the default logger, the current one will be destroyeds
            IntrusiveListNode<LoggerBase>* nextLink = nullptr;
            if (m_defaultLogger != nullptr && m_defaultLogger->Next() != m_defaultLogger.get())
            {
                nextLink = m_defaultLogger->Next();
            }

            m_defaultLogger = std::make_unique<NewLoggerType>(std::forward<Args>(args)...);

            if (nextLink != nullptr)
            {
                m_defaultLogger->LinkTo(nextLink);
            }

            return m_defaultLogger.get();
        }

    private:
        std::unique_ptr<LoggerBase> m_defaultLogger; //TODO: maybe replace by a custom smart ptr
    };

    static DefaultLogger defaultLogger;
}

#endif // MOE_DEFAULT_LOGGER_H_
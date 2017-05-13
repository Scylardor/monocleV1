#ifndef MOE_LOG_H_
#define MOE_LOG_H_

// This is the "include all things" header for Monocle logging.
// It includes commonly used logger interfaces, logging utilities, log policies...
// And the usually useful macros to log more easily.

// The logger basically includes everything else
#include "moeLogger.h"
#include "moeLogMacros.h"
#include "DefaultLogger.h"

// Logger Policies
#include "LogPolicies/NoFilterPolicy.h"
#include "LogPolicies/SeverityFilterPolicy.h"

#include "LogPolicies/NoFormatPolicy.h"
#include "LogPolicies/DebuggerFormatPolicy.h"

#include "LogPolicies/CaptureWritePolicy.h"
#include "LogPolicies/FileWritePolicy.h"
#include "LogPolicies/OutStreamWritePolicy.h"
#include "LogPolicies/IdeWritePolicy.h"

#endif // MOE_LOG_H_
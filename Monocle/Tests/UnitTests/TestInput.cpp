#include "catch.hpp"

// At the moment, tell Monocle we use std::string for our tests
#ifndef MOE_STD_SUPPORT
#define MOE_STD_SUPPORT
#endif
#include "Input/InputHandler.h"


// All tests should verify both char and wchar_t support.
TEST_CASE("Input", "[Input]")
{
    // A normal string format
    SECTION("Keyboard mappings")
    {
		moe::IInputHandler handler;
    }

}

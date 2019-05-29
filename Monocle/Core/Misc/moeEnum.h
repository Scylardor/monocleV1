// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "ThirdParty/BetterEnums/enum.h"

/*
	Monocle better enums
	Based on BetterEnums header-only library.
	The goal is to provide a friendly and improved enum interface that allows easy enum iteration, stringization, etc.
	I also added basic operator overloading to allow enums to be used as bitflags.

	Note : if you read the http://blog.bitwigglers.org post you'll realize I didn't implement the SFINAE for bitflags enums.
	It's because I thought it was too much added complexity for not a lot of gained value.
	Instead, I use DECLARE_MOE_ENUM_OPERATORS and DEFINE_MOE_ENUM_OPERATORS macros. When creating a new MOE_ENUM, you need to use both
	to use bitwise operations (DECLARE_MOE_ENUM_OPERATORS in the header, DEFINE_MOE_ENUM_OPERATORS in a translation unit, .cpp etc...).

	Better Enums is distributed under the terms of the 2-clause BSD license (see LICENSE in ThirdParty/BetterEnums).

	See also:
	- https://aantron.github.io/better-enums/
	- http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
*/

#define MOE_ENUM(Enum, UnderlyingType, ...) \
BETTER_ENUM(Enum, UnderlyingType, ##__VA_ARGS__);

#define DECLARE_MOE_ENUM_OPERATORS(Enum)\
Enum operator |(Enum lhs, Enum rhs);\
Enum operator &(Enum lhs, Enum rhs);\
Enum operator ^(Enum lhs, Enum rhs);\
Enum operator ~(Enum rhs);\
Enum& operator |=(Enum &lhs, Enum rhs);\
Enum& operator &=(Enum &lhs, Enum rhs);\
Enum& operator ^=(Enum &lhs, Enum rhs);


#define DEFINE_MOE_ENUM_OPERATORS(Enum) \
Enum operator |(Enum lhs, Enum rhs) \
{ \
	return Enum::_from_integral(\
		lhs._to_integral() | rhs._to_integral() \
	); \
} \
Enum operator &(Enum lhs, Enum rhs) \
{ \
	return Enum::_from_integral(\
		lhs._to_integral() & rhs._to_integral() \
	); \
} \
Enum operator ^(Enum lhs, Enum rhs) \
{ \
	return Enum::_from_integral(\
		lhs._to_integral() ^ rhs._to_integral() \
	); \
} \
Enum operator ~(Enum rhs) \
{ \
	return Enum::_from_integral(\
		~rhs._to_integral() \
	); \
} \
Enum& operator |=(Enum &lhs, Enum rhs) \
{ \
	lhs = lhs | rhs; \
	return lhs; \
} \
Enum& operator &=(Enum &lhs, Enum rhs) \
{ \
	lhs = lhs & rhs; \
	return lhs; \
} \
Enum& operator ^=(Enum &lhs, Enum rhs) \
{ \
	lhs = lhs ^ rhs; \
	return lhs; \
}

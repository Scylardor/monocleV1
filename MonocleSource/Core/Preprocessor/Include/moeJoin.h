#ifndef MOE_JOIN_H_
#define MOE_JOIN_H_

// This kind of strange idiom allows us to even join macro values together (e.g. __FILE__ ...)
#define MOE_JOIN_INTERNAL(a, b) a ## b
#define MOE_JOIN(a, b) MOE_JOIN_INTERNAL(a, b)

#endif // MOE_JOIN_H_

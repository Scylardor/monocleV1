Code convention
===============

Here are some guidelines I usually follow for the code convention.

##### No tabs
- I replace tabs by spaces in Monocle code for the code to display the same amount of space whatever your IDE is. Tab length can vary among text editors, so tabs should be avoided. Make sure you activate the "Replace tabs by spaces" feature of your IDE.
Default indentation is four (4) spaces.

##### One instruction per line (except for ternary operator)
- One instruction per line, including in initializer lists. Limit use of ternary (?:) operator. I usually use it to chain null pointer checks.

```cpp
// Check ptr1 and ptr2 are non-null without having to write a full-blown if (ptr == nullptr) {...} check
Ptr* ptr1 = getPtr();
Ptr2* ptr2 = ptr1 ? ptr1->getPtr2() : nullptr;
Ptr3* ptr3 = ptr2 ? ptr2->getPtr3() : nullptr;

if (ptr3 == nullptr)
{
	...
}
```

##### Brackets style
I always use the "bracket on its own line" style.

```
class Test
{

};

void test()
{

}

if (true == bTest)
{

}
else if (false == bTest)
{

}
```


##### Yoda conditions
"Yoda conditions" (e.g. `2 == var` instead of `var == 2`) are encouraged but not enforced.


##### Variable naming
Class member variables must have a "m_" prefix.

Static const member variables should be named IN ALL CAPITALS.

You can "name encode" your variables (e.g. `bBool` for a bool variable or `pVar` for a pointer variable) if you want, but I usually don't do it.


##### Headers

"Public" header files must be named with the "moe" (MOE stands for MOnocle Engine) prefix in order to avoid potential name clashes with user files that could have the same name and to make it explicit they are files coming from Monocle.

Headers must use good old include guards.
```
// moeClass.h
#ifndef MOE_CLASS_H_
#define MOE_CLASS_H_

...

#endif // MOE_CLASS_H_
```

As much as I love `#pragma once`'s handiness, sadly it's not standard and some compilers and build systems don't like it very much.
You can put one inside the guards if you want, though.

Always use `.h` extension for header files, with an exception for complex template classes.

Templates are declared and defined only in header files.

But, in order to keep a readable interface for complex cases (templates can quickly become very hard to read), I sometimes mimic the `.h/.cpp` declaration/definition paradigm with `.h/.hpp`.

A `.hpp` file means it's the "implementation" part of a template class declared in the matching `.h` file. An `.hpp` is always included at the end of its matching `.h` file and shouldn't be included elsewhere.
That's why there aren't include guards on `.hpp` files.






# CompileTimeTypeName
Experimental MSVC `__FUNCSIG__` exploit to get full type name in compile time

I have used a pretty dumb algorithm to extract the type in `__FUNCSIG__`. Sadly because `__FUNCSIG__` isn't working right with pointers and templates.

## Usage
```
namespace CompileTimeTypeName {
    template<class T>
    constexpr auto make_type_name_array() -> std::array<char, <extracted type name size>>;
}

template<class T>
void print_type_name_array(); // prints the type name byte by byte, then puts newline
```
All spaces, `class`, `struct`, `enum`, `union`, `const`, `__cdecl`, `volatile`, and `T(void)` parameterless `void` will be removed. This is done to better simulate `__PRETTY_FUNCTION__` in other C++ platforms.

## Bug
Consider this class
```
class Foo {
};
```
`make_type_name_array<Foo *>()` will give you `classFoo *`, not `class Foo *` for some reason. 
Combining with `const` and `volatile` this is even trickier. 
This bug has been discovered since 2013, but still, nothing is done by the VS team.

Template is also not supported, as the default template argument was also dumped in in compile time, but you could dangerously turn template type resolve on by `#define COMPILE_TIME_TYPE_NAME_DISABLE_TEMPLATE 1` before including the header

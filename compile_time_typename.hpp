#pragma once

#include <array>
#include <type_traits>

#ifndef COMPILE_TIME_TYPE_NAME_DISABLE_TEMPLATE
#define COMPILE_TIME_TYPE_NAME_DISABLE_TEMPLATE 1
#endif

namespace CompileTimeTypeName::Detail {

    template<size_t N>
    constexpr size_t const_strlen(const char(&)[N]) {
        return N - 1;
    }

    template<std::size_t n, std::size_t N>
    constexpr bool const_strneql(const char(&target)[n], const char(&src)[N], const std::size_t begin) {
        for (std::size_t i = begin, j = 0; i < begin + n - 1; ++j, ++i) {
            if (target[j] != src[i]) {
                return false;
            }
        }
        return true;
    }


    template <std::size_t N >
    constexpr bool validate_sig(const char(&sig)[N], std::size_t i, const std::size_t j) {
#if COMPILE_TIME_TYPE_NAME_DISABLE_TEMPLATE
        for (; i <= j; i++) {
            if (sig[i] == '<') {
                return false;
            }
        }
#endif
        return true;
    }


    // layout: { begin, end, total_size }
    template<std::size_t N>
    constexpr std::array<std::size_t, 3> find_name_info(const char(&sig)[N]) {
#ifdef _MSC_VER
        std::size_t i = const_strlen(sig);

        // bottom up scanning
        for (; i > 0 && sig[i] != '>'; --i);

        // i is the very last right bracket '>', decrement one to get last character index of the type name
        const std::size_t end = i - 1;

        std::size_t balance = 0;

        for (; i > 0; --i) {
            if (sig[i] == '>' || sig[i] == '<') {
                switch (sig[i]) {
                    case '>': balance++; break;
                    case '<': balance--; break;
                    default: continue;
                }

                if (balance == 0) {
                    break;
                }
            }
        }

        // i is the foremost left bracket '<', increment one to get first character index of the type name
        std::size_t begin = i + 1;
        std::size_t total_size = (end - begin) + 1;
        bool found = false;

        for (std::size_t j = begin; j <= end; ) {
#define TYPE_FIND_NAME_INFO_IGNORE(s, b) \
        { \
            if (const_strneql(s, sig, j)) { \
                total_size -= const_strlen(s) - 1; \
                j += const_strlen(s) - 1; \
                if (!found && (b)) { \
                    begin += const_strlen(s); \
                } \
                continue; \
            }\
        } 

            if (sig[j] == ' ') {
                --total_size;
                ++j;
                continue;
            }

            if (sig[j] == '<' && !found) {
                found = true;
            }

            TYPE_FIND_NAME_INFO_IGNORE("void)", false);
            TYPE_FIND_NAME_INFO_IGNORE("enum ", true);
            TYPE_FIND_NAME_INFO_IGNORE("class ", true);
            TYPE_FIND_NAME_INFO_IGNORE("union ", true);
            TYPE_FIND_NAME_INFO_IGNORE("const ", true);
            TYPE_FIND_NAME_INFO_IGNORE("struct ", true);
            TYPE_FIND_NAME_INFO_IGNORE("__cdecl", false);
            TYPE_FIND_NAME_INFO_IGNORE("volatile ", true);

            ++j;
        }
#undef TYPE_FIND_NAME_INFO_IGNORE
#define TYPE_FIND_NAME_INFO_IGNORE(s) \
    do { \
        if (const_strneql(s, sig, begin)) { \
            begin += const_strlen(s);\
        } \
    } while(0)

        TYPE_FIND_NAME_INFO_IGNORE("enum");
        TYPE_FIND_NAME_INFO_IGNORE("class");
        TYPE_FIND_NAME_INFO_IGNORE("union");
        TYPE_FIND_NAME_INFO_IGNORE("const");
        TYPE_FIND_NAME_INFO_IGNORE("struct");

#undef TYPE_FIND_NAME_INFO_IGNORE
        return { begin, end, total_size };
#else
        printf("%s\n", __PRETTY_FUNCTION__);
        return { 0, 0, 0 };
#endif
    }

    template<size_t n, size_t N>
    constexpr std::array<char, n> extract_name(const char(&sig)[N], std::size_t i, const std::size_t j) {
        std::array<char, n> arr{};

        for (std::size_t k = 0; i <= j; ) {
#define TYPE_FIND_NAME_INFO_IGNORE(s) \
    do { \
        if (const_strneql(s, sig, i)) { \
            i += const_strlen(s); \
            continue; \
        } \
    } while(0)


            if (sig[i] == ' ') {
                ++i;
                continue;
            }

            if (const_strneql("void)", sig, i)) {
                i += 4;
                continue;
            }

            TYPE_FIND_NAME_INFO_IGNORE("enum ");
            TYPE_FIND_NAME_INFO_IGNORE("class ");
            TYPE_FIND_NAME_INFO_IGNORE("union ");
            TYPE_FIND_NAME_INFO_IGNORE("const ");
            TYPE_FIND_NAME_INFO_IGNORE("struct ");
            TYPE_FIND_NAME_INFO_IGNORE("volatile ");

            if (const_strneql("__cdecl", sig, i)) {
                i += 7;
                continue;
            }

            arr[k] = std::move(sig[i]);
            ++k;
            ++i;
        }
#undef TYPE_FIND_NAME_INFO_IGNORE

        return arr;
    }
}

namespace CompileTimeTypeName {
    template<class T>
    constexpr auto make_type_name_array() {
        using namespace Detail;

        if constexpr (std::is_const<T>() || std::is_volatile<T>()) {
            return make_type_name_array<std::remove_cv<T>>();
        }

#ifdef _MSC_VER
        constexpr auto info = find_name_info(__FUNCSIG__);
        static_assert(validate_sig(__FUNCSIG__, info[0], info[1]), "target type contains template");
        constexpr auto arr = extract_name<info[2]>(__FUNCSIG__, info[0], info[1]);

        return arr;
#else
        printf("%s\n", __PRETTY_FUNCTION__);
#endif
    }
}

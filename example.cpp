#include <vector>

#define COMPILE_TIME_TYPE_NAME_DISABLE_TEMPLATE 0
#include "compile_time_typename.hpp"

namespace fooo {
    template<class T, class U>
    class Foo {

    };

    class Bar {

    };

    template<class T = void>
    class Baz {

    };

    enum Quux {

    };

    struct Quz;
}


class classX {
    
};

class volatileX {
    
};

int main() {
    using namespace CompileTimeTypeName;

    print_type_name_array<
        std::vector<void *>
    >();

    print_type_name_array<
        fooo::Quz
    >();

    print_type_name_array<
        int
    >();

    print_type_name_array<
        int const volatile *
    >();
    
    print_type_name_array<
        fooo::Quux
    >();
    
    print_type_name_array<
        fooo::Bar
    >();

    print_type_name_array<
        fooo::Quux&
    >();

    print_type_name_array<
        class classX*
    >();

    print_type_name_array<
        class volatileX*
    >();

    getchar();
    return 0;
}

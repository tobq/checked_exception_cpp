#include <variant>
#include <stdexcept>
#include <iostream>

template<class ret_t, class... exception_ts>
struct checked {
    using vexception_t = std::variant<exception_ts...>;

    const std::variant<ret_t, vexception_t> result;

    checked(ret_t value) : result(value) { // NOLINT(google-explicit-constructor)
    }

    template<class exception_t>
    requires (std::same_as<exception_t, exception_ts> || ...)
    checked(exception_t exception) : result(vexception_t(exception)) { // NOLINT(google-explicit-constructor)
    }

    [[nodiscard]] bool threw() const {
        return result.index() == 1;
    };

    [[nodiscard]] ret_t get() const {
        return std::get<ret_t>(result);
    };

    [[nodiscard]] vexception_t get_exception() const {
        return std::get<vexception_t>(result);
    }
};

using test_checked = checked<int, std::invalid_argument, std::runtime_error>;

void log(const test_checked &result) {
    if (result.threw()) {
        auto e = result.get_exception();
        switch (e.index()) {
            case 0:
                std::cout << "Threw: invalid_argument: " << std::get<std::invalid_argument>(e).what() << std::endl;
                break;
            case 1:
                std::cout << "Threw: runtime_error: " << std::get<std::runtime_error>(e).what() << std::endl;
                break;
        }
    } else {
        std::cout << "Value: " << result.get() << std::endl;
    }
}

test_checked test() {
    return 23;
}

test_checked test_throw() {
    return std::invalid_argument("test exception");
}

test_checked test_throw_2() {
    return std::runtime_error("test a different exception");
}

int main() {
    {
        auto result = test();
        log(result);
    }
    {
        auto result = test_throw();
        log(result);
    }
    {
        auto result = test_throw_2();
        log(result);
    }
    return 0;
}

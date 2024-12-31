namespace zzwlib {

class color {
public:
    enum class type {
        red,
        green,
        blue,
        black,
        white,
        gray,
        yellow,
        orange
    };
    static std::string_view to_string(type color_) noexcept {
        switch (color_) {
        case type::red:
            return "red";
        case type::green:
            return "green";
        case type::blue:
            return "blue";
        case type::black:
            return "black";
        case type::white:
            return "white";
        case type::gray:
            return "gray";
        case type::yellow:
            return "yellow";
        case type::orange:
            return "orange";
        }
        return "unknown";
    }
};

void test_color() {
    color::type c = color::type::red;
    std::cout << color::to_string(c) << std::endl;
};

}

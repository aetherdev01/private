// Create by sonurajbhar242@gmail.com
#pragma once

#include <array>
#include <string>
#include <iterator>
#include <algorithm>

namespace SPra {

    inline std::string xor_(std::string s) {
        std::string out{};
        for (char& c : s) {
            out += c ^ (s.length() + 1) ^ 'f';
        }
        return out;
    }

    template<std::size_t S>
    struct Xor_String {
        std::array<char, S> charArr;

        inline auto operator()() {
            std::string str{};

            std::transform(charArr.begin(), charArr.end() - 1, std::back_inserter(str), [](auto const& c) {
                return c ^ S ^ 'f';
                });
            return str;
        }

        constexpr Xor_String(const char(&string)[S]) : charArr{} {
            auto it = charArr.begin();
            for (auto const& c : string) {
                *it = c ^ S ^ 'f';
                it++;
            }
        }
    };
}

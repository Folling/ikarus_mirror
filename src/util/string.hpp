#pragma once

#include <string_view>

#include <unicode/schriter.h>
#include <unicode/uchar.h>
#include <unicode/uiter.h>

namespace util::str {

inline bool is_empty_or_blank(std::string_view str) {
    for (int32_t i = 0; i < str.size();) {
        UChar32 c;
        U8_NEXT(str, i, str.size(), c);
        if (!u_isUWhiteSpace(c)) {
            return false;
        }
    }

    return true;
}

}

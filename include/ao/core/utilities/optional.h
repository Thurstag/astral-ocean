// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#ifdef __has_include
#    if __has_include(<optional>)
#        include <optional>
#    elif __has_include(<experimental/optional>)
#        include <experimental/optional>

namespace std {
    template<class T>
    using optional = experimental::optional<T>;

    template<class T>
    constexpr std::optional<std::decay_t<T> > make_optional(T&& value) {
        return experimental::make_optional<T>(std::forward<T>(value));
    }

    template<class T, class... Args>
    constexpr experimental::optional<T> make_optional(Args&&... args) {
        return experimental::make_optional<T>(std::forward<Args>(args)...);
    }

    constexpr experimental::nullopt_t nullopt = experimental::nullopt_t{0};
    constexpr experimental::nullopt_t nullopt_t = experimental::nullopt_t{0};
}  // namespace std
#    else
#        error "Missing <optional> header"
#    endif
#else
#    include <optional>
#endif

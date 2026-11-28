#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <optional>

namespace qqfo::input {

using Frame = std::array<std::uint8_t, 56>;
using SessionId = std::array<std::uint8_t, 16>;

struct Target {
    SessionId session;
    std::uint64_t hwnd;
    std::uint32_t pid;
};

enum class Operation : std::uint16_t { Bind = 1, Click = 2 };

struct Request {
    Operation operation;
    std::uint32_t x;
    std::uint32_t y;
};

inline std::uint64_t read_le(const Frame& frame, std::size_t offset, unsigned size) noexcept {
    std::uint64_t value = 0;
    for (unsigned i = 0; i < size; ++i)
        value |= static_cast<std::uint64_t>(frame[offset + i]) << (i * 8);
    return value;
}

// One instance per accepted pipe connection, bound to the host's independently identified target.
class RequestSequence {
public:
    explicit RequestSequence(Target target) noexcept : target_(target) {}

    std::optional<Request> accept(const Frame& frame) noexcept {
        if (read_le(frame, 0, 4) != 0x31464F51 || read_le(frame, 4, 2) != 1 ||
            !std::equal(target_.session.begin(), target_.session.end(), frame.begin() + 16) ||
            read_le(frame, 32, 8) != target_.hwnd || read_le(frame, 40, 4) != target_.pid ||
            read_le(frame, 52, 4) != 0)
            return std::nullopt;

        const auto operation = static_cast<Operation>(read_le(frame, 6, 2));
        const auto id = read_le(frame, 8, 8);
        const auto x = read_le(frame, 44, 4);
        const auto y = read_le(frame, 48, 4);
        constexpr auto max_coordinate = (std::numeric_limits<std::int32_t>::max)();
        if (id <= last_id_ || x > max_coordinate || y > max_coordinate)
            return std::nullopt;
        if (last_id_ == 0 ? operation != Operation::Bind || x != 0 || y != 0 : operation != Operation::Click)
            return std::nullopt;

        last_id_ = id;
        return Request{operation, static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)};
    }

private:
    Target target_;
    std::uint64_t last_id_ = 0;
};

inline Frame acknowledgement(Frame request, bool accepted) noexcept {
    request[52] = accepted ? 1 : 0;
    return request;
}

} // namespace qqfo::input

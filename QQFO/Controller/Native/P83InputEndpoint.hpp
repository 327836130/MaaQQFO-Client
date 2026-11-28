#pragma once

#include "InputHostSession.hpp"
#include <limits>

namespace qqfo::input {

// The backend owns the provisioned P8.3 session, including activation state and detach.
class P83Bridge {
public:
    virtual ~P83Bridge() = default;
    virtual bool open(const Target& target) noexcept = 0;
    virtual std::optional<ClientExtent> client(const Target& target) noexcept = 0;
    virtual bool begin_desktop_check() noexcept = 0;
    virtual bool desktop_unchanged() noexcept = 0;
    virtual bool position(std::uint16_t x, std::uint16_t y) noexcept = 0;
    virtual bool move(std::uint16_t x, std::uint16_t y) noexcept = 0;
    virtual bool virtual_left(bool pressed) noexcept = 0;
    virtual bool deliver_left(bool pressed, std::uint16_t x, std::uint16_t y) noexcept = 0;
    virtual bool hold_left() noexcept = 0;
    virtual void close() noexcept = 0;
};

class P83InputEndpoint final : public InputEndpoint {
public:
    explicit P83InputEndpoint(P83Bridge& bridge) noexcept : bridge_(bridge) {}
    bool bind(const Target& target) noexcept override { return bridge_.open(target); }
    std::optional<ClientExtent> current_client(const Target& target) noexcept override { return bridge_.client(target); }

    bool click(const Target& target, std::uint32_t x, std::uint32_t y) noexcept override {
        const auto extent = bridge_.client(target);
        constexpr auto max_point = static_cast<std::uint32_t>((std::numeric_limits<std::int16_t>::max)());
        if (!extent || x >= extent->width || y >= extent->height || x > max_point || y > max_point ||
            !bridge_.begin_desktop_check())
            return false;
        const auto px = static_cast<std::uint16_t>(x);
        const auto py = static_cast<std::uint16_t>(y);
        if (!bridge_.position(px, py) || !bridge_.move(px, py) || !bridge_.desktop_unchanged())
            return false;

        // A failed acknowledgement can still mean the request reached the backend.
        // Once either press is attempted, attempt the corresponding release.
        const bool virtual_down = bridge_.virtual_left(true);
        const bool attempted_delivery = virtual_down && bridge_.desktop_unchanged();
        const bool delivered = attempted_delivery && bridge_.deliver_left(true, px, py);
        const bool held = delivered && bridge_.hold_left() && bridge_.desktop_unchanged();
        const bool virtual_up = bridge_.virtual_left(false);
        const bool released = attempted_delivery && bridge_.deliver_left(false, px, py);
        return held && virtual_up && released && bridge_.desktop_unchanged();
    }

    void release() noexcept override { bridge_.close(); }

private:
    P83Bridge& bridge_;
};

} // namespace qqfo::input

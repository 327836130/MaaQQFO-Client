#pragma once

#include "InputProtocol.hpp"

namespace qqfo::input {

struct ClientExtent {
    std::uint32_t width;
    std::uint32_t height;
};

class InputEndpoint {
public:
    virtual ~InputEndpoint() = default;
    virtual bool bind(const Target& target) noexcept = 0;
    virtual std::optional<ClientExtent> current_client(const Target& target) noexcept = 0;
    virtual bool click(const Target& target, std::uint32_t x, std::uint32_t y) noexcept = 0;
    virtual void release() noexcept = 0;
};

// The transport owns one connection. Disconnect also covers partial Bind failure.
class HostSession {
public:
    HostSession(Target target, InputEndpoint& endpoint) noexcept
        : target_(target), endpoint_(endpoint), sequence_(target) {}
    HostSession(const HostSession&) = delete;
    HostSession& operator=(const HostSession&) = delete;
    ~HostSession() { close(); }

    std::optional<Frame> handle(const Frame& frame, std::optional<ClientExtent> window_extent = std::nullopt) noexcept {
        if (closed_)
            return std::nullopt;
        const auto request = sequence_.accept(frame);
        if (!request) {
            close();
            return std::nullopt;
        }

        if (request->operation == Operation::Bind) {
            release_needed_ = true;
            const bool bound = endpoint_.bind(target_);
            if (!bound)
                close();
            return acknowledgement(frame, bound);
        }

        const auto extent = endpoint_.current_client(target_);
        if (!extent) {
            close();
            return acknowledgement(frame, false);
        }
        if (request->x >= extent->width || request->y >= extent->height ||
            (window_extent && (request->x >= window_extent->width || request->y >= window_extent->height)))
            return acknowledgement(frame, false);

        const bool delivered = endpoint_.click(target_, request->x, request->y);
        if (!delivered)
            close();
        return acknowledgement(frame, delivered);
    }

    bool closed() const noexcept { return closed_; }
    bool can_wait_for_request() noexcept {
        return release_needed_ && !closed_ && endpoint_.current_client(target_).has_value();
    }

    void close() noexcept {
        closed_ = true;
        if (release_needed_) {
            release_needed_ = false;
            endpoint_.release();
        }
    }

private:
    Target target_;
    InputEndpoint& endpoint_;
    RequestSequence sequence_;
    bool closed_ = false;
    bool release_needed_ = false;
};

} // namespace qqfo::input

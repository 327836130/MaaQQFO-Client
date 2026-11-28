#pragma once

#include <windows.h>
#include "InputHostSession.hpp"

namespace qqfo::input {

// The HWND and PID come from the provisioned endpoint, never from an incoming frame.
class WindowsInputTarget {
public:
    explicit WindowsInputTarget(const Target& target) noexcept {
        if (!target.hwnd || !target.pid ||
            target.hwnd > (std::numeric_limits<std::uintptr_t>::max)())
            return;
        window_ = reinterpret_cast<HWND>(static_cast<std::uintptr_t>(target.hwnd));
        thread_ = GetWindowThreadProcessId(window_, &pid_);
        if (!thread_ || pid_ != target.pid)
            return;
        process_ = OpenProcess(SYNCHRONIZE, FALSE, pid_);
    }

    WindowsInputTarget(const WindowsInputTarget&) = delete;
    WindowsInputTarget& operator=(const WindowsInputTarget&) = delete;

    ~WindowsInputTarget() {
        if (process_)
            CloseHandle(process_);
    }

    bool current() const noexcept {
        DWORD pid = 0;
        return process_ && WaitForSingleObject(process_, 0) == WAIT_TIMEOUT &&
            GetWindowThreadProcessId(window_, &pid) == thread_ && pid == pid_;
    }

    std::optional<ClientExtent> client() const noexcept {
        if (!current())
            return std::nullopt;
        const auto previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        if (!previous)
            return std::nullopt;
        RECT bounds{};
        const bool found = GetClientRect(window_, &bounds) != FALSE;
        SetThreadDpiAwarenessContext(previous);
        if (!found || !current() || bounds.right < bounds.left || bounds.bottom < bounds.top)
            return std::nullopt;
        return ClientExtent{static_cast<std::uint32_t>(bounds.right - bounds.left),
                            static_cast<std::uint32_t>(bounds.bottom - bounds.top)};
    }

private:
    HWND window_ = nullptr;
    HANDLE process_ = nullptr;
    DWORD pid_ = 0;
    DWORD thread_ = 0;
};

} // namespace qqfo::input

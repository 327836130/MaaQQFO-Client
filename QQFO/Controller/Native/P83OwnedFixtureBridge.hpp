#pragma once

#include <cstring>
#include <cwchar>
#include "P83InputEndpoint.hpp"
#include "WindowsInputTarget.hpp"

namespace qqfo::input {

// Adapter for the archived x86 receiver fixture. It only opens the owned receiver
// supplied by the fixture launcher and validates that receiver's button-state replies.
class P83OwnedFixtureBridge final : public P83Bridge {
public:
    P83OwnedFixtureBridge(Target owned, HMODULE fixture_module) noexcept
        : owned_(owned), window_(owned), module_(fixture_module) {}

    ~P83OwnedFixtureBridge() override { close(); }

    bool open(const Target& target) noexcept override {
        if (!matches(target) || !window_.current() || hook_ || !module_)
            return false;
        hwnd_ = reinterpret_cast<HWND>(static_cast<std::uintptr_t>(owned_.hwnd));
        wchar_t class_name[64]{};
        if (!GetClassNameW(hwnd_, class_name, 64) || wcscmp(class_name, L"P82.Receiver") != 0)
            return false;
        const auto thread = GetWindowThreadProcessId(hwnd_, nullptr);
        const auto message = RegisterWindowMessageW(L"ProtoInput.P83.20261117");
        if (!thread || !message)
            return false;
        message_ = message;

        wchar_t name[64]{};
        if (swprintf_s(name, L"Local\\ProtoInput.P83.%lu", static_cast<unsigned long>(owned_.pid)) < 0)
            return false;
        mapping_ = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, evidence_size, name);
        if (!mapping_ || GetLastError() == ERROR_ALREADY_EXISTS)
            return false;
        evidence_ = static_cast<volatile Evidence*>(MapViewOfFile(mapping_, FILE_MAP_WRITE, 0, 0, evidence_size));
        if (!evidence_)
            return false;
        evidence_->owner = GetCurrentProcessId();

        const auto monitor = GetProcAddress(module_, "_Monitor@12");
        if (!monitor || !window_.current())
            return false;
        static_assert(sizeof(HOOKPROC) == sizeof(monitor));
        HOOKPROC procedure{};
        std::memcpy(&procedure, &monitor, sizeof(procedure));
        hook_ = SetWindowsHookExW(WH_CALLWNDPROC, procedure, module_, thread);
        if (!hook_)
            return false;
        install_attempted_ = true;
        return command(Install);
    }

    std::optional<ClientExtent> client(const Target& target) noexcept override {
        return matches(target) ? window_.client() : std::nullopt;
    }

    bool begin_desktop_check() noexcept override {
        foreground_ = GetForegroundWindow();
        return GetPhysicalCursorPos(&cursor_) != FALSE;
    }

    bool desktop_unchanged() noexcept override {
        POINT cursor{};
        return GetPhysicalCursorPos(&cursor) && cursor.x == cursor_.x && cursor.y == cursor_.y &&
            GetForegroundWindow() == foreground_;
    }

    bool position(std::uint16_t x, std::uint16_t y) noexcept override {
        POINT screen{static_cast<LONG>(x), static_cast<LONG>(y)};
        DWORD_PTR observed = 0;
        return ClientToScreen(hwnd_, &screen) && command(Position, MAKELPARAM(x, y)) &&
            send(WM_APP + 72, 0, 0, &observed) &&
            static_cast<DWORD>(observed) == static_cast<DWORD>(MAKELONG(screen.x, screen.y));
    }
    bool move(std::uint16_t x, std::uint16_t y) noexcept override { return send(WM_MOUSEMOVE, 0, MAKELPARAM(x, y)); }
    bool virtual_left(bool pressed) noexcept override { return command(pressed ? Down : Up); }
    bool deliver_left(bool pressed, std::uint16_t x, std::uint16_t y) noexcept override {
        DWORD_PTR reply = 0;
        return send(pressed ? WM_LBUTTONDOWN : WM_LBUTTONUP, pressed ? MK_LBUTTON : 0,
            MAKELPARAM(x, y), &reply) && reply == (pressed ? 3u : 0u);
    }
    bool hold_left() noexcept override { Sleep(90); return true; }

    void close() noexcept override {
        if (install_attempted_) {
            install_attempted_ = false;
            detached_ = command(Remove);
        }
        if (hook_) {
            detached_ = UnhookWindowsHookEx(hook_) && detached_;
            hook_ = nullptr;
        }
        if (evidence_) {
            UnmapViewOfFile(const_cast<Evidence*>(evidence_));
            evidence_ = nullptr;
        }
        if (mapping_) {
            CloseHandle(mapping_);
            mapping_ = nullptr;
        }
    }

    bool detached() const noexcept { return detached_; }

private:
    enum Action { Install = 1, Position = 2, Down = 3, Up = 4, Remove = 6 };
    struct Evidence {
        LONG ack, status, hits, key_hits, x, y;
        DWORD owner;
        DWORD diagnostics[9];
    };
    static_assert(sizeof(Evidence) == 64);
    static constexpr DWORD evidence_size = sizeof(Evidence);

    bool matches(const Target& target) const noexcept {
        return target.hwnd == owned_.hwnd && target.pid == owned_.pid && target.session == owned_.session;
    }
    bool send(UINT message, WPARAM wp, LPARAM lp, DWORD_PTR* reply = nullptr) const noexcept {
        DWORD_PTR ignored = 0;
        return window_.current() && SendMessageTimeoutW(hwnd_, message, wp, lp,
            SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT, 400, reply ? reply : &ignored) != 0;
    }
    bool command(Action action, LPARAM argument = 0) const noexcept {
        const auto previous = evidence_->ack;
        if (!send(message_, action, argument))
            return false;
        if (action == Remove) {
            const auto deadline = GetTickCount64() + 7000;
            while (evidence_->ack == previous && GetTickCount64() < deadline && window_.current())
                Sleep(10);
        }
        return evidence_->ack == previous + 1 && evidence_->status == 0;
    }

    Target owned_;
    WindowsInputTarget window_;
    HMODULE module_;
    HWND hwnd_ = nullptr;
    HANDLE mapping_ = nullptr;
    HHOOK hook_ = nullptr;
    volatile Evidence* evidence_ = nullptr;
    UINT message_ = 0;
    HWND foreground_ = nullptr;
    POINT cursor_{};
    bool install_attempted_ = false;
    bool detached_ = false;
};

} // namespace qqfo::input

#include "InputProtocol.hpp"
#include "InputHostSession.hpp"
#include "LocalInputPipe.hpp"
#include "LocalInputSession.hpp"
#include "P83InputEndpoint.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <windows.h>

using namespace qqfo::input;

class FixtureEndpoint final : public InputEndpoint {
public:
    bool bind_ok = true;
    bool click_ok = true;
    unsigned releases = 0;
    unsigned clicks = 0;
    std::optional<ClientExtent> extent = ClientExtent{1600, 900};

    bool bind(const Target&) noexcept override { return bind_ok; }
    std::optional<ClientExtent> current_client(const Target&) noexcept override { return extent; }
    bool click(const Target&, std::uint32_t x, std::uint32_t y) noexcept override {
        ++clicks;
        return click_ok && x == 800 && y == 450;
    }
    void release() noexcept override { ++releases; }
};

class P83FixtureBridge final : public P83Bridge {
public:
    enum Step { Open, Position, Move, VirtualDown, DeliverDown, Hold, VirtualUp, DeliverUp, Close };
    std::array<Step, 16> steps{};
    std::size_t count = 0;
    bool fail_down_ack = false;
    bool move_desktop_during_hold = false;
    unsigned clicks = 0;

    bool open(const Target&) noexcept override { return record(Open); }
    std::optional<ClientExtent> client(const Target&) noexcept override { return ClientExtent{65536, 900}; }
    bool begin_desktop_check() noexcept override { return true; }
    bool desktop_unchanged() noexcept override { return desktop_unchanged_; }
    bool position(std::uint16_t x, std::uint16_t y) noexcept override {
        return x == 800 && y == 450 && record(Position);
    }
    bool move(std::uint16_t, std::uint16_t) noexcept override { return record(Move); }
    bool virtual_left(bool pressed) noexcept override { return record(pressed ? VirtualDown : VirtualUp); }
    bool deliver_left(bool pressed, std::uint16_t x, std::uint16_t y) noexcept override {
        if (x != 800 || y != 450 || !record(pressed ? DeliverDown : DeliverUp))
            return false;
        if (pressed)
            ++clicks;
        return !pressed || !fail_down_ack;
    }
    bool hold_left() noexcept override {
        desktop_unchanged_ = !move_desktop_during_hold;
        return record(Hold);
    }
    void close() noexcept override { record(Close); }

private:
    bool desktop_unchanged_ = true;
    bool record(Step step) noexcept {
        if (count == steps.size())
            return false;
        steps[count++] = step;
        return true;
    }
};

void p83_sequence_smoke(const Target& target, const Frame& bind, const Frame& click) {
    using Step = P83FixtureBridge::Step;
    for (unsigned scenario = 0; scenario < 4; ++scenario) {
        P83FixtureBridge bridge;
        bridge.fail_down_ack = scenario == 1;
        bridge.move_desktop_during_hold = scenario == 2;
        P83InputEndpoint endpoint(bridge);
        {
            HostSession session(target, endpoint);
            assert(session.handle(bind)->at(52) == 1);
            auto request = click;
            if (scenario == 3) {
                request[44] = 0;
                request[45] = 0x80;
            }
            assert(session.handle(request)->at(52) == (scenario == 0 ? 1 : 0));
            assert(session.closed() == (scenario != 0));
            if (scenario != 0)
                assert(!session.handle(request));
        }
        const std::array<Step, 9> full{Step::Open, Step::Position, Step::Move, Step::VirtualDown,
            Step::DeliverDown, Step::Hold, Step::VirtualUp, Step::DeliverUp, Step::Close};
        const std::array<Step, 8> unacknowledged{Step::Open, Step::Position, Step::Move, Step::VirtualDown,
            Step::DeliverDown, Step::VirtualUp, Step::DeliverUp, Step::Close};
        if (scenario == 3) {
            assert(bridge.count == 2 && bridge.steps[0] == Step::Open && bridge.steps[1] == Step::Close);
            assert(bridge.clicks == 0);
        } else if (scenario == 1) {
            assert(bridge.count == unacknowledged.size() &&
                std::equal(unacknowledged.begin(), unacknowledged.end(), bridge.steps.begin()));
        } else {
            assert(bridge.count == full.size() && std::equal(full.begin(), full.end(), bridge.steps.begin()));
        }
    }
}

static HWND fixture_window() {
    return CreateWindowExW(WS_EX_NOACTIVATE, L"STATIC", L"MaaQQFO host fixture", WS_POPUP,
        0, 0, 1200, 700, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
}

static void windows_target_smoke() {
    const auto old = fixture_window();
    assert(old);
    const auto handle = reinterpret_cast<std::uintptr_t>(old);
    const auto pid = GetCurrentProcessId();
    const WindowsInputTarget wrong_pid(Target{{}, handle, pid + 1});
    assert(!wrong_pid.current());
    const WindowsInputTarget target(Target{{}, handle, pid});
    assert(target.current());
    assert(target.client()->width == 1200 && target.client()->height == 700);
    assert(SetWindowPos(old, nullptr, 0, 0, 900, 600, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER));
    assert(target.client()->width == 900 && target.client()->height == 600);
    assert(DestroyWindow(old));
    const auto replacement = fixture_window();
    assert(replacement && replacement != old && !target.current() && !target.client());
    const WindowsInputTarget fresh(Target{{}, reinterpret_cast<std::uintptr_t>(replacement), pid});
    assert(fresh.current());
    assert(DestroyWindow(replacement));
}

int wire_smoke() {
    windows_target_smoke();
    // Wire bytes emitted by the managed client for a fixed target and session.
    Frame bind{0x51,0x4f,0x46,0x31,1,0,1,0,1,0,0,0,0,0,0,0,
        0x33,0x22,0x11,0x00,0x55,0x44,0x77,0x66,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,
        0xc8,0x01,0,0,0,0,0,0,0x7b,0,0,0};
    Target target{{0x33,0x22,0x11,0x00,0x55,0x44,0x77,0x66,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff}, 456, 123};
    auto click = bind;
    click[6] = 2;
    click[8] = 2;
    click[44] = 0x20;
    click[45] = 0x03;
    click[48] = 0xc2;
    click[49] = 0x01;
    RequestSequence connection(target);
    assert(!connection.accept(click));
    auto wrong = bind;
    wrong[16] ^= 1;
    assert(!connection.accept(wrong));
    wrong = bind;
    wrong[40]++;
    assert(!connection.accept(wrong));
    assert(connection.accept(bind)->operation == Operation::Bind);
    wrong = click;
    wrong[47] = 0xff;
    assert(!connection.accept(wrong));
    const auto accepted = connection.accept(click);
    assert(accepted && accepted->operation == Operation::Click && accepted->x == 800 && accepted->y == 450);
    assert(!connection.accept(click));
    assert(acknowledgement(click, true)[52] == 1);
    assert(acknowledgement(click, false) == click);

    FixtureEndpoint endpoint;
    {
        HostSession host(target, endpoint);
        assert(!host.can_wait_for_request());
        assert(host.handle(bind)->at(52) == 1);
        assert(host.can_wait_for_request());
        endpoint.extent = ClientExtent{720, 480};
        assert(host.handle(click)->at(52) == 0 && endpoint.clicks == 0 && !host.closed());
        endpoint.extent = ClientExtent{1600, 900};
        click[8] = 3;
        assert(host.handle(click)->at(52) == 1 && endpoint.clicks == 1);
        endpoint.extent = std::nullopt;
        assert(!host.can_wait_for_request());
        click[8] = 4;
        assert(host.handle(click)->at(52) == 0 && host.closed() && endpoint.clicks == 1);
    }
    assert(endpoint.releases == 1);
    endpoint.bind_ok = false;
    {
        HostSession host(target, endpoint);
        assert(host.handle(bind)->at(52) == 0 && host.closed());
    }
    assert(endpoint.releases == 2);
    endpoint.bind_ok = true;
    endpoint.click_ok = false;
    endpoint.extent = ClientExtent{1600, 900};
    {
        HostSession host(target, endpoint);
        assert(host.handle(bind)->at(52) == 1);
        assert(host.handle(click)->at(52) == 0 && host.closed() && endpoint.clicks == 2);
        assert(!host.handle(click));
    }
    assert(endpoint.releases == 3);
    p83_sequence_smoke(target, bind, click);
    const auto timeout_name = L"\\\\.\\pipe\\MaaQQFO.Input.Timeout." + std::to_wstring(GetCurrentProcessId());
    const auto timeout_pipe = create_local_input_pipe(timeout_name.c_str());
    assert(timeout_pipe != INVALID_HANDLE_VALUE);
    const auto time_started = GetTickCount64();
    assert(!connect_local_pipe(timeout_pipe, 50));
    assert(GetTickCount64() - time_started < 5000);
    const auto client = CreateFileW(timeout_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
        nullptr, OPEN_EXISTING, 0, nullptr);
    assert(client != INVALID_HANDLE_VALUE);
    assert(connect_local_pipe(timeout_pipe, 500));
    Frame partial{};
    assert(read_input_frame(timeout_pipe, partial, 50) == FrameRead::Idle);
    DWORD written = 0;
    assert(WriteFile(client, bind.data(), 1, &written, nullptr) && written == 1);
    assert(read_input_frame(timeout_pipe, partial, 50) == FrameRead::Failed);
    CloseHandle(client);
    CloseHandle(timeout_pipe);
    std::cout << "OFFLINE_PASS " << sizeof(void*) * 8 << "-bit wire, host, P8.3 sequence, cleanup and timeout checks\n";
    return 0;
}

int pipe_fixture(const std::wstring& name, const std::wstring& wire_session) {
    if (wire_session.size() != 32 || wire_session.find_first_not_of(L"0123456789abcdef") != std::wstring::npos ||
        name.size() != 32 || name.find_first_not_of(L"0123456789abcdef") != std::wstring::npos)
        return 2;
    const auto window = fixture_window();
    if (!window)
        return 3;
    Target target{{}, reinterpret_cast<std::uintptr_t>(window), GetCurrentProcessId()};
    for (std::size_t i = 0; i < target.session.size(); ++i)
        target.session[i] = static_cast<std::uint8_t>(std::stoul(wire_session.substr(i * 2, 2), nullptr, 16));
    const auto path = L"\\\\.\\pipe\\MaaQQFO.Input." + name;
    const HANDLE pipe = create_local_input_pipe(path.c_str());
    if (pipe == INVALID_HANDLE_VALUE) {
        DestroyWindow(window);
        return 3;
    }
    std::cout << target.pid << ' ' << target.hwnd << std::endl;
    P83FixtureBridge bridge;
    P83InputEndpoint endpoint(bridge);
    const auto result = serve_local_session(pipe, target, endpoint, {5000, 500, 5000});
    CloseHandle(pipe);
    DestroyWindow(window);
    return result == TransportResult::EndOfSession && bridge.clicks == 1 && bridge.count == 9 &&
        bridge.steps[8] == P83FixtureBridge::Close ? 0 : 4;
}

int wmain(int argc, wchar_t** argv) {
    if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
        return 5;
    if (argc == 1)
        return wire_smoke();
    if (argc == 4 && std::wstring(argv[1]) == L"--pipe-fixture")
        return pipe_fixture(argv[2], argv[3]);
    return 2;
}

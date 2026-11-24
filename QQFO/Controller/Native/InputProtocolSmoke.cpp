#include "InputProtocol.hpp"
#include "InputHostSession.hpp"
#include "LocalInputPipe.hpp"
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

int wire_smoke() {
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
        assert(host.handle(bind)->at(52) == 1);
        endpoint.extent = ClientExtent{720, 480};
        assert(host.handle(click)->at(52) == 0 && endpoint.clicks == 0 && !host.closed());
        endpoint.extent = ClientExtent{1600, 900};
        click[8] = 3;
        assert(host.handle(click)->at(52) == 1 && endpoint.clicks == 1);
        endpoint.extent = std::nullopt;
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
    const auto timeout_name = L"\\\\.\\pipe\\MaaQQFO.Input.Timeout." + std::to_wstring(GetCurrentProcessId());
    const auto timeout_pipe = create_local_input_pipe(timeout_name.c_str());
    assert(timeout_pipe != INVALID_HANDLE_VALUE);
    const auto time_started = GetTickCount64();
    assert(!connect_local_pipe(timeout_pipe, 50));
    assert(GetTickCount64() - time_started < 5000);
    CloseHandle(timeout_pipe);
    std::cout << "OFFLINE_PASS " << sizeof(void*) * 8 << "-bit wire, host bounds, failure, cleanup and timeout checks\n";
    return 0;
}

int pipe_fixture(const std::wstring& name, const std::wstring& wire_session) {
    if (wire_session.size() != 32 || wire_session.find_first_not_of(L"0123456789abcdef") != std::wstring::npos ||
        name.size() != 32 || name.find_first_not_of(L"0123456789abcdef") != std::wstring::npos)
        return 2;
    Target target{{}, 456, 123};
    for (std::size_t i = 0; i < target.session.size(); ++i)
        target.session[i] = static_cast<std::uint8_t>(std::stoul(wire_session.substr(i * 2, 2), nullptr, 16));
    const auto path = L"\\\\.\\pipe\\MaaQQFO.Input." + name;
    const HANDLE pipe = create_local_input_pipe(path.c_str());
    if (pipe == INVALID_HANDLE_VALUE)
        return 3;
    const auto transact = [&] {
        if (!connect_local_pipe(pipe, 5000))
            return 4;
        FixtureEndpoint endpoint;
        HostSession session(target, endpoint);
        for (unsigned request_index = 0; request_index < 2; ++request_index) {
            Frame frame{};
            if (!read_input_frame(pipe, frame, 5000))
                return 5;
            const auto reply = session.handle(frame);
            if (!reply || reply->at(52) != 1 || endpoint.clicks != request_index)
                return 6;
            if (!write_input_frame(pipe, *reply, 5000))
                return 7;
        }
        Frame unexpected{};
        return read_input_frame(pipe, unexpected, 5000) ? 8 : 0;
    };
    const auto result = transact();
    CloseHandle(pipe);
    return result;
}

int wmain(int argc, wchar_t** argv) {
    if (argc == 1)
        return wire_smoke();
    if (argc == 4 && std::wstring(argv[1]) == L"--pipe-fixture")
        return pipe_fixture(argv[2], argv[3]);
    return 2;
}

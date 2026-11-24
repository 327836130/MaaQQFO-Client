#include "InputProtocol.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <windows.h>

using namespace qqfo::input;

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
    std::cout << "OFFLINE_PASS " << sizeof(void*) * 8 << "-bit native wire validation and replay rejection\n";
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
    const HANDLE pipe = CreateNamedPipeW(path.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE | PIPE_REJECT_REMOTE_CLIENTS, 1, 56, 56, 2000, nullptr);
    if (pipe == INVALID_HANDLE_VALUE)
        return 3;
    const auto transact = [&] {
        if (!ConnectNamedPipe(pipe, nullptr) && GetLastError() != ERROR_PIPE_CONNECTED)
            return 4;
        RequestSequence sequence(target);
        for (auto operation : {Operation::Bind, Operation::Click}) {
            Frame frame{};
            DWORD offset = 0;
            const auto size = static_cast<DWORD>(frame.size());
            while (offset < size) {
                DWORD bytes = 0;
                if (!ReadFile(pipe, frame.data() + offset, size - offset, &bytes, nullptr) || bytes == 0)
                    return 5;
                offset += bytes;
            }
            const auto request = sequence.accept(frame);
            if (!request || request->operation != operation ||
                (operation == Operation::Click && (request->x != 800 || request->y != 450)))
                return 6;
            const auto reply = acknowledgement(frame, true);
            DWORD written = 0;
            if (!WriteFile(pipe, reply.data(), size, &written, nullptr) || written != size)
                return 7;
        }
        return FlushFileBuffers(pipe) ? 0 : 8;
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

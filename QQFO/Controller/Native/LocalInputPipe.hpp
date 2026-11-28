#pragma once

#include <windows.h>
#include <sddl.h>
#include <memory>
#include <string>
#include <vector>
#include "InputProtocol.hpp"

namespace qqfo::input {

inline std::vector<unsigned char> token_information(HANDLE token, TOKEN_INFORMATION_CLASS kind) {
    DWORD needed = 0;
    GetTokenInformation(token, kind, nullptr, 0, &needed);
    std::vector<unsigned char> storage(needed);
    if (needed == 0 || !GetTokenInformation(token, kind, storage.data(), needed, &needed))
        return {};
    return storage;
}

inline std::wstring sid_text(PSID value) {
    LPWSTR text = nullptr;
    if (!ConvertSidToStringSidW(value, &text))
        return {};
    const std::unique_ptr<void, decltype(&LocalFree)> owner(text, &LocalFree);
    return text;
}

// The caller provides a provisioned, local endpoint name. Restrict it to the host account.
inline HANDLE create_local_input_pipe(const wchar_t* name) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
        return INVALID_HANDLE_VALUE;
    const std::unique_ptr<void, decltype(&CloseHandle)> token_owner(token, &CloseHandle);

    const auto users = token_information(token, TokenUser);
    const auto owners = token_information(token, TokenOwner);
    if (users.empty() || owners.empty())
        return INVALID_HANDLE_VALUE;

    // .NET's CurrentUserOnly checks TokenOwner, which can differ from TokenUser.
    // Only the user SID is granted read/write access by the DACL.
    const auto user = sid_text(reinterpret_cast<const TOKEN_USER*>(users.data())->User.Sid);
    const auto owner = sid_text(reinterpret_cast<const TOKEN_OWNER*>(owners.data())->Owner);
    if (user.empty() || owner.empty())
        return INVALID_HANDLE_VALUE;
    const auto sddl = L"O:" + owner + L"D:P(A;;GA;;;" + user + L")";
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(), SDDL_REVISION_1, &descriptor, nullptr))
        return INVALID_HANDLE_VALUE;
    const std::unique_ptr<void, decltype(&LocalFree)> descriptor_owner(descriptor, &LocalFree);

    SECURITY_ATTRIBUTES attributes{sizeof(SECURITY_ATTRIBUTES), descriptor, FALSE};
    return CreateNamedPipeW(name, PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_BYTE | PIPE_REJECT_REMOTE_CLIENTS, 1, 56, 56, 2000, &attributes);
}

// The event and buffer remain alive until the requested I/O (including cancellation) finishes.
template <typename Begin>
inline bool bounded_pipe_io(HANDLE pipe, DWORD timeout, Begin begin, DWORD& transferred,
                            bool* timed_out = nullptr) noexcept {
    const HANDLE event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (event == nullptr)
        return false;
    const std::unique_ptr<void, decltype(&CloseHandle)> owner(event, &CloseHandle);
    OVERLAPPED operation{};
    operation.hEvent = event;
    if (begin(operation))
        return true;
    if (GetLastError() != ERROR_IO_PENDING)
        return false;
    const auto wait = WaitForSingleObject(event, timeout);
    if (wait == WAIT_OBJECT_0)
        return GetOverlappedResult(pipe, &operation, &transferred, FALSE) != FALSE;
    CancelIoEx(pipe, &operation);
    if (GetOverlappedResult(pipe, &operation, &transferred, TRUE))
        return true; // Completion can race the deadline and consume bytes.
    if (timed_out)
        *timed_out = wait == WAIT_TIMEOUT && GetLastError() == ERROR_OPERATION_ABORTED;
    return false;
}

inline bool connect_local_pipe(HANDLE pipe, DWORD timeout) noexcept {
    DWORD unused = 0;
    return bounded_pipe_io(pipe, timeout, [&](OVERLAPPED& operation) {
        return ConnectNamedPipe(pipe, &operation) || GetLastError() == ERROR_PIPE_CONNECTED;
    }, unused);
}

enum class FrameRead { Complete, Idle, Failed };

inline FrameRead read_input_frame(HANDLE pipe, Frame& frame, DWORD timeout) noexcept {
    // An idle client has not sent a request. Once a byte arrives, bound the remaining frame assembly.
    DWORD first_byte = 0;
    bool idle = false;
    if (!bounded_pipe_io(pipe, timeout, [&](OVERLAPPED& operation) {
        return ReadFile(pipe, frame.data(), 1, &first_byte, &operation) != FALSE;
    }, first_byte, &idle) || first_byte != 1)
        return idle ? FrameRead::Idle : FrameRead::Failed;

    const auto deadline = GetTickCount64() + timeout;
    const auto size = static_cast<DWORD>(frame.size());
    DWORD offset = first_byte;
    while (offset < size) {
        const auto now = GetTickCount64();
        if (now >= deadline)
            return FrameRead::Failed;
        DWORD transferred = 0;
        if (!bounded_pipe_io(pipe, static_cast<DWORD>(deadline - now), [&](OVERLAPPED& operation) {
            return ReadFile(pipe, frame.data() + offset, size - offset, &transferred, &operation) != FALSE;
        }, transferred) || transferred == 0)
            return FrameRead::Failed;
        offset += transferred;
    }
    return FrameRead::Complete;
}

inline bool write_input_frame(HANDLE pipe, const Frame& frame, DWORD timeout) noexcept {
    DWORD transferred = 0;
    const auto size = static_cast<DWORD>(frame.size());
    return bounded_pipe_io(pipe, timeout, [&](OVERLAPPED& operation) {
        return WriteFile(pipe, frame.data(), size, &transferred, &operation) != FALSE;
    }, transferred) && transferred == size;
}

} // namespace qqfo::input

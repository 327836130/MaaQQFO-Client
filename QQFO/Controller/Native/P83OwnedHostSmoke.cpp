#include "LocalInputSession.hpp"
#include "P83OwnedFixtureBridge.hpp"
#include <iostream>

using namespace qqfo::input;

static HWND receiver_window(DWORD pid) {
    for (unsigned attempt = 0; attempt < 100; ++attempt) {
        HWND window = nullptr;
        while ((window = FindWindowExW(nullptr, window, L"P82.Receiver", nullptr)) != nullptr) {
            DWORD owner = 0;
            if (GetWindowThreadProcessId(window, &owner) && owner == pid)
                return window;
        }
        Sleep(20);
    }
    return nullptr;
}

static bool restored_cursor(HWND window) {
    POINT point{};
    DWORD_PTR result = 0;
    return GetCursorPos(&point) &&
        SendMessageTimeoutW(window, WM_APP + 72, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 400, &result) &&
        static_cast<DWORD>(result) == static_cast<DWORD>(MAKELONG(point.x, point.y));
}

int wmain(int argc, wchar_t** argv) {
    if (argc != 4 || std::wstring(argv[1]) != L"--pipe-fixture" ||
        !SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
        return 2;
    const std::wstring name(argv[2]), wire_session(argv[3]);
    if (name.size() != 32 || wire_session.size() != 32 ||
        name.find_first_not_of(L"0123456789abcdef") != std::wstring::npos ||
        wire_session.find_first_not_of(L"0123456789abcdef") != std::wstring::npos)
        return 2;

    wchar_t module_path[MAX_PATH]{};
    const auto size = GetModuleFileNameW(nullptr, module_path, MAX_PATH);
    if (!size || size >= MAX_PATH)
        return 2;
    const std::wstring path(module_path);
    const auto folder = path.substr(0, path.find_last_of(L'\\') + 1);
    const auto module = LoadLibraryW((folder + L"P82Hook.dll").c_str());
    if (!module)
        return 3;
    const std::unique_ptr<std::remove_pointer_t<HMODULE>, decltype(&FreeLibrary)> loaded(module, &FreeLibrary);
    const auto receiver = folder + L"p82.exe";
    wchar_t command[] = L"p82.exe receiver";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION child{};
    if (!CreateProcessW(receiver.c_str(), command, nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                        nullptr, folder.c_str(), &startup, &child))
        return 4;
    CloseHandle(child.hThread);
    const auto cleanup = [](void* process) {
        if (WaitForSingleObject(process, 0) == WAIT_TIMEOUT && TerminateProcess(process, 0))
            WaitForSingleObject(process, 2000);
        CloseHandle(process);
    };
    const std::unique_ptr<void, decltype(cleanup)> owned(child.hProcess, cleanup);
    const auto window = receiver_window(child.dwProcessId);
    if (!window)
        return 5;
    Target target{{}, reinterpret_cast<std::uintptr_t>(window), child.dwProcessId};
    for (std::size_t i = 0; i < target.session.size(); ++i)
        target.session[i] = static_cast<std::uint8_t>(std::stoul(wire_session.substr(i * 2, 2), nullptr, 16));
    const auto pipe_name = L"\\\\.\\pipe\\MaaQQFO.Input." + name;

    for (int attempt = 0; attempt < 2; ++attempt) {
        const HANDLE pipe = create_local_input_pipe(pipe_name.c_str());
        if (pipe == INVALID_HANDLE_VALUE)
            return 6;
        P83OwnedFixtureBridge bridge(target, module);
        P83InputEndpoint endpoint(bridge);
        std::cout << target.pid << ' ' << target.hwnd << std::endl;
        const auto result = serve_local_session(pipe, target, endpoint, {5000, 500, 2000});
        CloseHandle(pipe);
        if (result != TransportResult::EndOfSession || !bridge.detached() || !restored_cursor(window))
            return 7;
    }
    return 0;
}

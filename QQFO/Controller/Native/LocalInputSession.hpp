#pragma once

#include "InputHostSession.hpp"
#include "LocalInputPipe.hpp"
#include "WindowsInputTarget.hpp"

namespace qqfo::input {

enum class TransportResult { EndOfSession, TargetUnavailable, ConnectFailed, InvalidRequest, ReplyFailed };

struct PipeTimeouts {
    DWORD connect_ms;
    DWORD read_ms;
    DWORD write_ms;
};

// The caller owns the pipe handle. The session owns backend release on every exit path.
inline TransportResult serve_local_session(HANDLE pipe, Target target, InputEndpoint& endpoint,
                                           PipeTimeouts timeouts) noexcept {
    const WindowsInputTarget window(target);
    if (!window.current())
        return TransportResult::TargetUnavailable;
    if (!connect_local_pipe(pipe, timeouts.connect_ms))
        return TransportResult::ConnectFailed;

    HostSession session(target, endpoint);
    while (!session.closed()) {
        Frame frame{};
        const auto read = read_input_frame(pipe, frame, timeouts.read_ms);
        if (!window.current())
            return TransportResult::TargetUnavailable;
        if (read == FrameRead::Idle && session.can_wait_for_request())
            continue;
        if (read != FrameRead::Complete)
            return TransportResult::EndOfSession;
        const auto extent = window.client();
        if (!extent)
            return TransportResult::TargetUnavailable;
        const auto reply = session.handle(frame, extent);
        if (!reply)
            return TransportResult::InvalidRequest;
        if (!write_input_frame(pipe, *reply, timeouts.write_ms))
            return TransportResult::ReplyFailed;
    }
    return TransportResult::EndOfSession;
}

} // namespace qqfo::input

#pragma once

#include "InputHostSession.hpp"
#include "LocalInputPipe.hpp"

namespace qqfo::input {

enum class TransportResult { EndOfSession, ConnectFailed, InvalidRequest, ReplyFailed };

struct PipeTimeouts {
    DWORD connect_ms;
    DWORD read_ms;
    DWORD write_ms;
};

// The caller owns the pipe handle. The session owns backend release on every exit path.
inline TransportResult serve_local_session(HANDLE pipe, Target target, InputEndpoint& endpoint,
                                           PipeTimeouts timeouts) noexcept {
    if (!connect_local_pipe(pipe, timeouts.connect_ms))
        return TransportResult::ConnectFailed;

    HostSession session(target, endpoint);
    while (!session.closed()) {
        Frame frame{};
        if (!read_input_frame(pipe, frame, timeouts.read_ms))
            return TransportResult::EndOfSession;
        const auto reply = session.handle(frame);
        if (!reply)
            return TransportResult::InvalidRequest;
        if (!write_input_frame(pipe, *reply, timeouts.write_ms))
            return TransportResult::ReplyFailed;
    }
    return TransportResult::EndOfSession;
}

} // namespace qqfo::input

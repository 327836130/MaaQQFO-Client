# A3 native input protocol verification

`QQFO/Controller/Native/InputProtocol.hpp` validates the input frames used by the managed QQFO Controller. Following the P3 design, the protocol uses fixed-width little-endian fields. The 56-byte frame includes signature, version, operation, request ID, session UUID bytes in the .NET wire order, HWND, PID, two physical client coordinates, and a reply status. A sequence starts with Bind. Click frames require that binding and a strictly increasing ID. The decoder matches a target identity supplied by the host and rejects negative coordinates. The input host must still validate current client bounds before dispatch.

`tools/verify_native_input_protocol.cmd x86` and `tools/verify_native_input_protocol.cmd x64` passed with MSVC and `/W4 /WX`. The checked wire payload maps to client (800,450). The smoke rejected a Click before Bind, wrong session and PID, a negative coordinate and a replay. Both checks run in Windows CI.

Native pipe host and Proto Input dispatch remain open. This evidence covers decoding and connection sequencing.

An x64 managed client subsequently opened a real Windows named pipe to each native fixture process and completed Bind plus one (800,450) Click acknowledgement. The fixture validates frames and exits without dispatching input. The cross-process check runs in the same CI step as the native decoder checks.

## 2026-11-24 host session and transport

`InputHostSession.hpp` supplies the lifecycle around the backend interface. It verifies requests against the provisioned target, rechecks client bounds before each click and releases the endpoint once on disconnect, partial bind failure, stale target or uncertain dispatch. A rejected out-of-bounds point does not invoke the backend. `LocalInputPipe.hpp` creates a local first-instance named pipe with a protected DACL granting access to the host account. It uses bounded overlapped connect/read/write and cancels outstanding I/O on timeout.

MSVC `/W4 /WX` x86 and x64 smoke runs passed bound, resized and stale-client checks, partial bind cleanup, failed click cleanup and idle-connect cancellation. The x64 managed client completed Bind and Click acknowledgements with each native fixture over the hardened transport. Fixture endpoints receive and validate requests without dispatching clicks. The P8.3 adapter and authorized game validation remain pending.

The archived P8.3 owned-window selftest was rerun from `artifacts/a3-p83-fixture-8dae84fd82484803b38af035554ecaf3`. The three copied binaries matched `docs/migration/p83-assets.json`. Both attempts exited 1 at the desktop stability guard. The logs show a change in physical cursor coordinates with an unchanged foreground HWND. Expected virtual cursor coordinates were observed before abort. The first attempt sampled fixtures at 144 and 96 DPI awareness settings. This is an incomplete historical-backend retest, with no complete P8.3 PASS or cross-monitor DPI result. A third attempt was deferred by the desktop-activity precheck. Raw fixture JSONL is preserved in the ignored local evidence directory.

Windows CI 45412178095 compiled both architectures but rejected the managed handshake. .NET 10 CurrentUserOnly compares the pipe owner to the client's TokenOwner. The host now assigns TokenOwner as object owner and grants pipe I/O only to TokenUser in the protected DACL. This handles accounts whose default owner is a group. Local x86/x64 smoke and managed round trips passed again.

## P8.3 sequence adapter and reusable connection loop

The managed client now exercises `serve_local_session` and `P83InputEndpoint` together through the x86 and x64 native pipe fixtures. The endpoint preserves the archived position, move, virtual down, window down, hold, virtual up and window up order. The bridge interface owns the approved window/session, activation and detach. The pipe host remains independent of the concrete backend.

The fixture tests passed the normal sequence, missing down acknowledgement cleanup, desktop change during hold cleanup, signed message-coordinate overflow rejection and disconnect release. The P8.3 native bridge implementation that owns the historical hook and activation state remains pending. Current tests establish the host-to-endpoint contract using a fixture bridge.

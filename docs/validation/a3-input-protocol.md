# A3 native input protocol verification

`QQFO/Controller/Native/InputProtocol.hpp` validates the input frames used by the managed QQFO Controller. Following the P3 design, the protocol uses fixed-width little-endian fields. The 56-byte frame includes signature, version, operation, request ID, session UUID bytes in the .NET wire order, HWND, PID, two physical client coordinates, and a reply status. A sequence starts with Bind. Click frames require that binding and a strictly increasing ID. The decoder matches a target identity supplied by the host and rejects negative coordinates. The input host must still validate current client bounds before dispatch.

`tools/verify_native_input_protocol.cmd x86` and `tools/verify_native_input_protocol.cmd x64` passed with MSVC and `/W4 /WX`. The checked wire payload maps to client (800,450). The smoke rejected a Click before Bind, wrong session and PID, a negative coordinate and a replay. Both checks run in Windows CI.

Native pipe host and Proto Input dispatch remain open. This evidence covers decoding and connection sequencing.

An x64 managed client subsequently opened a real Windows named pipe to each native fixture process and completed Bind plus one (800,450) Click acknowledgement. The fixture validates frames and exits without dispatching input. The cross-process check runs in the same CI step as the native decoder checks.

## 2026-11-24 host session and transport

`InputHostSession.hpp` supplies the lifecycle around the backend interface. It verifies requests against the provisioned target, rechecks client bounds before each click and releases the endpoint once on disconnect, partial bind failure, stale target or uncertain dispatch. A rejected out-of-bounds point does not invoke the backend. `LocalInputPipe.hpp` creates a local first-instance named pipe with a protected DACL granting access to the host account. It uses bounded overlapped connect/read/write and cancels outstanding I/O on timeout.

MSVC `/W4 /WX` x86 and x64 smoke runs passed bound, resized and stale-client checks, partial bind cleanup, failed click cleanup and idle-connect cancellation. The x64 managed client completed Bind and Click acknowledgements with each native fixture over the hardened transport. Fixture endpoints receive and validate requests without dispatching clicks. The P8.3 adapter and authorized game validation remain pending.

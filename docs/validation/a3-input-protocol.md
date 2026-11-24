# A3 native input protocol verification

`QQFO/Controller/Native/InputProtocol.hpp` validates the input frames used by the managed QQFO Controller. Following the P3 design, the protocol uses fixed-width little-endian fields. The 56-byte frame includes signature, version, operation, request ID, session UUID bytes in the .NET wire order, HWND, PID, two physical client coordinates, and a reply status. A sequence starts with Bind. Click frames require that binding and a strictly increasing ID. The decoder matches a target identity supplied by the host and rejects negative coordinates. The input host must still validate current client bounds before dispatch.

`tools/verify_native_input_protocol.cmd x86` and `tools/verify_native_input_protocol.cmd x64` passed with MSVC and `/W4 /WX`. The checked wire payload maps to client (800,450). The smoke rejected a Click before Bind, wrong session and PID, a negative coordinate and a replay. Both checks run in Windows CI.

Native pipe host and Proto Input dispatch remain open. This evidence covers decoding and connection sequencing.

An x64 managed client subsequently opened a real Windows named pipe to each native fixture process and completed Bind plus one (800,450) Click acknowledgement. The fixture validates frames and exits without dispatching input. The cross-process check runs in the same CI step as the native decoder checks.

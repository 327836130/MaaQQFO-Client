# A2 production Controller path

A Win32 resource controller can opt in with `qqfo_input_session` set to the provisioned session UUID in 32-digit hex format. `MaaProcessor.InitializeController` creates the QQFO Custom Controller for that explicitly selected profile. The Windows backend takes raw client screenshots via Maa's existing Win32 capture controller. It sends client-space clicks exclusively to the same-user named pipe opened for the session. The pipe requires a matching reply for the exact request and closes on timeout or framing mismatch. No uncertain click is replayed.

The owned-window smoke drove the production backend with a test peer. With an 800×600 client at 120 DPI and a 640×480 Maa screenshot, a click at image (320,240) decoded to (400,300) at the pipe. The peer acknowledged the request without delivering input. The constructor uses deferred linking to avoid duplicate connections. Client bounds are read under Per-Monitor V2 with restoration of the previous thread context. Core tests cover split reads, wrong request IDs and missing server timeout.

The app entry and production client backend are compiled. A live input host and cross-monitor DPI coverage remain open. The existing synthetic and actual fixture window evidence remains in STAGE_A2_REVIEW.md. The agreed game execution permissions remain in P9 §7.

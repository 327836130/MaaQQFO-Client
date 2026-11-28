# A6 GameStateReader checkpoint

Status: IN_PROGRESS.

The C++ reader consumes exact reads from a source tied to a specific PID, process creation time and session generation. It validates the C1 player layout and grid header, and checks the scene links again after sampling the world coordinates. Consumers can share a short-lived snapshot from this reader; changes to scene, role, map resource or process identity invalidate its cache. Failed samples carry a status and no cached position. Resource number refers to the client map resource.

Run `tools/verify_game_state.cmd` to build and run the Windows synthetic isolation regression. It covers two process identities, process reuse, stale reads, world coordinate signs, map change during sampling, and cache invalidation. Hosted CI runs the same synthetic test.

For the recorded-byte replay, run `python tools/extract_c1_state_replay.py <original-C1-json> artifacts/a6-state/c1.txt`, then `artifacts/a6-state/state.exe artifacts/a6-state/c1.txt`. The extractor verifies the archive SHA256 against the migration inventory. The local replay recovered world point `(8087, 1572)` and map resource `37` from the archived bytes.

The Q registry sampler reads the evidenced half-open pointer range in two passes and rejects a changed scene, range, member or query field. Results include slot, optional module-relative vtable, raw query key, raw filter and local-role match. The 4096-slot ceiling is a read budget. No object address is exported in the snapshot, and query keys do not serve as persistent identities. Synthetic regression covers a member removed between passes, malformed bounds and separate sessions with different keys.

The hash-verified C1 replay also reproduced all three Q members and query keys. C1 vtable and query fields establish membership and lookup behavior. Entity classification, live Windows memory backing, client version qualification, native map identity and permanent character ID remain open. Replay read archived bytes only.

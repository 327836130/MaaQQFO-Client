# A12 NuGet security gate (PR #1, B3)

Status: open. Owner: A12 release validation. Source: PR #1 Windows build, `45186466211`, and GitHub advisory package ranges. Keep NuGet auditing enabled and retain warnings. These advisories do not block independent development.

| Resolved package | Advisory and warning | Exposure to verify | First patched in affected line |
|---|---|---|---|
| System.Security.Cryptography.Pkcs 8.0.1 | [GHSA-846f-f764-76mx](https://github.com/advisories/GHSA-846f-f764-76mx) NU1903, [GHSA-2fmv-rv2c-59x7](https://github.com/advisories/GHSA-2fmv-rv2c-59x7) NU1902 | PKCS processing and denial of service. Trace Windows dependency and input paths. | 8.0.2 for both |
| Tmds.DBus.Protocol 0.21.2 | [GHSA-xrw6-gwf8-vvr9](https://github.com/advisories/GHSA-xrw6-gwf8-vvr9) NU1903 | D-Bus peers. Check Linux and Windows package reachability separately. | 0.21.3 |
| MailKit 4.14.1 | [GHSA-9j88-vvj5-vhgr](https://github.com/advisories/GHSA-9j88-vvj5-vhgr) NU1902 | STARTTLS in mail notifications. Check configured transports. | 4.16.0 |
| MimeKit 4.14.0 | [GHSA-g7hc-96xr-gvvx](https://github.com/advisories/GHSA-g7hc-96xr-gvvx) NU1902 | SMTP command injection via mailbox parsing. Trace message/address inputs. | 4.15.1 |
| SharpCompress 0.42.1 | [GHSA-6c8g-7p36-r338](https://github.com/advisories/GHSA-6c8g-7p36-r338) NU1902 | Directory traversal during archive extraction. Check update and import paths. | 0.48.0 |

Before A12 release, rerun NuGet audit including transitive packages, trace every affected package to its source dependency, and prefer compatible patched versions or an upstream fix. Update locks and rerun affected build, runtime and packaging tests. Any remaining advisory requires evidence-backed risk acceptance and an explicit release decision. Patch availability alone does not establish compatibility or actual reachability.

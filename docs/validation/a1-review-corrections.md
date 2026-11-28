# A1 review corrections verification

Reviewed commit `b2402a7` remains historical. Correction `31096f6bf025df2fc43c090f6cb0eb5b56b18644` passed [Windows CI](https://github.com/327836130/MaaQQFO-Client/actions/runs/45190167862) on PR #1.

- Clean local clone: SDK 10.0.402, locked desktop build (0 errors, 35 warnings), 15/15 configuration tests, both native smokes and archive generation passed. See local ignored directory `artifacts/a1-review-ca9b340e0d6648f0adc936532b28585b`.
- B1: CI's provenance assertion and downloaded test artifact report SDK 10.0.402. `global.json` prohibits roll-forward. Dependency locks were retained.
- B2: downloaded developer artifact `14825337794` and TRX artifact `14825497456`. The PR head is `31096f6bf025df2fc43c090f6cb0eb5b56b18644`, base is `edfb87ac7a049a50b7921658cc206265c8b9e4e5`, and tested checkout is `cfa146e68d85767f648c54c5d408685d7b7fbec0`. The ZIP name uses `pr-merge` and its checkout prefix. Embedded and stand-alone provenance parsed to identical JSON. SHA256, ZIP CRCs and TRX totals (15 passed, 0 failed) were checked. A local follow-up verified that the packager now preserves exact metadata bytes, including newlines.
- B3: the [A12 security gate](../engineering/A12_NUGET_SECURITY_GATE.md) tracks audit findings. NuGet warnings remain enabled.

A1 remains VERIFY_PENDING for independent review of the correction. Draft PR #1 is the review entry point.

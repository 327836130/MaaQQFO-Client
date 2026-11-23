# A1 Windows package architecture check

Tested the outputs of the clean Fork clone at `8fcf63068c9115f954ab217d296d3629803ff5ad`. The desktop apphost and MaaFramework, MaaAgentClient and MaaAgentServer DLLs all reported PE machine `0x8664` (x64). ZIP CRC verification passed. The resulting ZIP SHA256 was `494780463fbf1318b183495d66d12d93b8c6c0d52ece88c42c68d6794e630d18`.

A fixture with a copied apphost whose PE machine was changed to `0x014c` was rejected with exit code 2 before any archive was written.

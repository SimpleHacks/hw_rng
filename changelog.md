# Changelist for HW_RNG arduino library

## v1.1.0

<details><summary>Fix: #2 by supporting both `nrfx v1.7.2` and `nrfx v2.1.0`</summary><P/>

Nordic made significant breaking API changes to the RNG API
somewhere between `nrfx v1.7.2` (which v1.0.0 of this library
was written for) and `nrfx v2.1.0`. These breaking API changes
were **not** listed in the nrfx changelog.

By sheer luck, a new macro was also defined with this version
bump, allowing this library to support either version of
the `nrfx` headers, with zero code change by library users.
</details><HR/>

Thanks to user @BitLox for reporting this issue, and his help testing the fix.

## v1.0.0

First release.  Works with nRF52 series chips when using Adafruit Arduino BSP.

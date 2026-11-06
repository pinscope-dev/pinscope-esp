# Security

## Reporting a vulnerability

Write to **security@pinscope.dev**, or use **Report a vulnerability** under this
repository's Security tab (a private report, only the maintainers see it).
Please do not open a public issue for a vulnerability.

What helps: the version, the platform and board, what an attacker can do, and how
to reproduce it. English or Serbian.

## What happens then

- We answer within 3 working days.
- We confirm the issue, agree with you on a date, and fix it in a new release.
- The fix is published as a GitHub security advisory with a CVE where one applies,
  and we credit you unless you would rather not be named.
- Releases with a security fix say so in their notes, and the Pinscope dashboard
  shows which devices still run an affected version.

## What is covered

The device component in this repository, every supported platform. Security fixes
go into the newest release; updates within the same major version keep the API, so
moving to the newest release is the fix.

## What is in it

`sbom.spdx.json` lists what the component carries (SPDX 2.3): Pinscope's own code,
and the public-domain ref10 and poly1305-donna code its crypto is built on. The
platform it is built against (ESP-IDF, Zephyr, the Pico SDK) is not part of it;
your product's own SBOM lists that with its version.

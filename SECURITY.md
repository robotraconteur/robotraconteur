# Vulnerability Disclosure Policy

Wason Technology, LLC is dedicated to software security across the Robot Raconteur ecosystem. Please notify us immediately if you discover a security vulnerability in one of our products.

## 1. Reporting a Vulnerability
Please submit vulnerability reports by emailing **softwaresecurity@wasontech.com**, preferably in English. Alternatively, reports may be submitted privately via the **Private Vulnerability Reporting** feature on our main GitHub repository.

The more information provided about the bug, the easier it will be to fix. If you already have a fix, please include it with your report. We may ask to exchange PGP keys to discuss sensitive details about the vulnerability—including proof-of-concept code, impact assessments, or recommended remediation steps.

## 2. Coordinated Disclosure & Timeline
In order to protect our users, unless otherwise agreed by both parties, we ask that you not publicly discuss the vulnerability until a fix is published or at least **90 days** have passed since the initial report submission. Wason Technology will adhere to this same timeline.

If you do not wish to be acknowledged in the release communications, please indicate so when you submit your report.

## 3. Remediation & Release Process
* Vulnerabilities are investigated using internal code review and static analysis tools (such as Coverity) to isolate and patch the flaw.
* Fixes will be integrated into the core libraries and distributed via new software releases accompanied by updated release notes and an automated Software Bill of Materials (SBOM).
* Special releases may be issued to fix specific vulnerabilities outside of normal release schedules.

## 4. Regulatory Compliance (EU Cyber Resilience Act Alignment)
For products distributed within the European Union, Wason Technology complies with applicable provisions of the EU Cyber Resilience Act (CRA). In the event of a severe security incident or an actively exploited vulnerability affecting commercial offerings, we execute internal tracking and fulfill mandatory reporting obligations to relevant authorities (such as ENISA and national CSIRTs) via the official EU Single Reporting Platform within statutory timeframes.

# ROS Quality Declaration

Robot Raconteur Core is a ROS Quality Category 2 package according to [REP 2004](https://www.ros.org/reps/rep-2004.html).

Category Requirements:

* 1.i: **Must have a version policy**: semver is used for versioning
* 1.ii: **Must be at a stable version**: *Version 1.0 release pending*
* 1.iii: **Must have a strictly declared public API**: Public API is documented in Doxygen, Sphinx, and other documentation
* 1.iv: **Must have a policy for API stability**: API will remain stable between major releases
* 1.v: **Must have a policy for ABI stability**: ABI will remain stable between minor releases
* 1.vi: **Must have a policy that keeps API and ABI stability within a released ROS distribution**: ABI Check is performed on each release
* 2.i: **Must have all code changes occur through a change request:** Master branch is protected, requiring pull requests
* 2.ii: **Must have a confirmation of contributor origin**: Harmony CLA is used for contributors. See https://www.wasontech.com/contributors for more information
* 2.iii: N/A
* 2.iv: **Must have Continuous Integration (CI) for all change requests**: CI must pass before pull request can be merged
* 2.v: N/A
* 3.i: **Must have documentation for each "feature"**: Full documentation is available at https://github.com/robotraconteur/robotraconteur/wiki/Documentation
* 3.ii: N/A
* 3.iii: **Must have a declared license or set of licenses**: Apache 2.0 License is used for entire package
* 3.iv: **Must state copyrights within the project and attribute all authors**: All files have appropriate headers for Apache 2.0
* 3.v: **Must have a "quality declaration" document**: This document is the quality declaration
* 4.i: **Must have system tests which cover all items in the "feature" documentation**: GitHub Actions is used for testing
* 4.ii: N/A
* 4.iiia: **Code Coverage**: Code coverage is included in GitHub Actions workflow
* 4.iiib: N/A
* 4.iv: N/A
* 4.va: **Must have a code style and enforce it**: Code style is enforced by clang-format
* 4.vb: **Must use static analysis tools where applicable**: clang-tidy, cppcheck, and Coverity Scan are used for static analysis
* 5.i: **Must not have direct runtime "ROS" dependencies which are not at the same level as the package in question**: The package has no ROS dependencies
* 5.ii: N/A
* 5.iii: **Must have justification for why each direct runtime "non-ROS" dependency is equivalent to a 'Level N' package in terms of quality**: Only external dependencies are Boost, OpenSSL, SWIG, and system packages. These are all equivalent ROS Quality Category 1
* 6.i: **Must support all target platforms for the package's ecosystem**: Windows, Linux, and MacOS are fully supported on all architectures
* 7.i: **Must have a declared Vulnerability Disclosure Policy**: See [VULNERABILITY_POLICY](VULNERABILITY_POLICY.md)


Experimental Features:

* HandwareTransport for USB, PCIe, and Bluetooth
* Java wrappers
* StringTable message compression
* Taps for message interception

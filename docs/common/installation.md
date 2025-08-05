## Installation

  - [Installation](#installation)
    - [Windows](#windows)
    - [Ubuntu Xenial, Bionic, Focal, Jammy, and Noble](#ubuntu-xenial-bionic-focal-jammy-and-noble)
    - [Debian 10, 11, 12 (buster, bullseye, bookworm)](#debian-10-11-12-buster-bullseye-bookworm)
    - [Raspbian 10, 11, 12](#raspbian-10-11-12)
    - [Mac OSX](#mac-osx)
    - [iOS](#ios)
    - [Android](#android)
    - [MATLAB](#matlab-add-on)
    - [LabVIEW](#labview-add-on)

### Windows

#### C++

`vcpkg` is used to install the Robot Raconteur C++ library. See https://github.com/microsoft/vcpkg for installation instructions.
The `robotraconteur` package is available in the vcpkg registry.

```
vcpkg install robotraconteur
```

To build x64, use:

```
vcpkg --triplet=x64-windows install robotraconteur
```

#### Python

The Python wrappers are distributed using PyPi.

```
pip install robotraconteur
```

#### C\#

The C\# library is available on NuGet. Search for "RobotRaconteurNET".

#### Java

A zip file is available on the releases page on github. Download the zip file and extract it to a directory in your classpath.

### Ubuntu Xenial, Bionic, Focal, Jammy, and Noble

A PPA is available for Robot Raconteur. [https://launchpad.net/~robotraconteur/+archive/ubuntu/ppa](https://launchpad.net/~robotraconteur/+archive/ubuntu/ppa)

```
sudo add-apt-repository ppa:robotraconteur/ppa
sudo apt-get update
```

#### C++

```
sudo apt-get install robotraconteur-dev
```

#### Python
```
sudo apt-get install python-robotraconteur
sudo apt-get install python3-robotraconteur

```

#### C\#

The C\# library is available on NuGet. Search for "RobotRaconteurNET".

The Linux native library must be installed using apt:

```
sudo apt-get install librobotraconteur-net-native
```

#### Java

Java must be built from source on Linux.

### Debian 10, 11, 12 (buster, bullseye, bookworm)

An apt repository is available for Debian. Packages are available for amd64, armhf, and arm64. See below for raspbian setup. To use, run:

```
sudo apt install apt-transport-https dirmngr gnupg ca-certificates
wget -O - https://robotraconteur.github.io/robotraconteur-apt/wasontech-apt.gpg.key | sudo apt-key add -
echo "deb https://robotraconteur.github.io/robotraconteur-apt/debian buster main" | sudo tee /etc/apt/sources.list.d/robotraconteur.list
sudo apt update
```

#### C++

```
sudo apt-get install robotraconteur-dev
```

#### Python
```
sudo apt-get install python-robotraconteur
sudo apt-get install python3-robotraconteur

```

#### C\#

The C\# library is available on NuGet. Search for "RobotRaconteurNET".

The Linux native library must be installed using apt:

```
sudo apt-get install librobotraconteur-net-native
```

#### Java

Java must be built from source on Linux.

### Raspbian 10, 11, 12

The Raspberry Pi OS (raspbian) for armhf 32-bit processors is slightly different than the main debian armhf distributions. It uses ARMv6 instructions, instead of the ARMv7 instructions used by the main debian installation. Use the following to set up the raspbian repository, and see the debian section for the rest of the instructions.

**Raspberry Pi OS (raspbian) for 64-bit uses the standard debian repository. This is only for 32-bit ARMv6 installation.**

```
sudo apt install apt-transport-https dirmngr gnupg ca-certificates
wget -O - https://robotraconteur.github.io/robotraconteur-apt/wasontech-apt.gpg.key | sudo apt-key add -
echo "deb https://robotraconteur.github.io/robotraconteur-apt/raspbian buster main" | sudo tee /etc/apt/sources.list.d/robotraconteur.list
sudo apt update
```

### Mac OSX

#### C++

Use `brew` to install the Robot Raconteur C++ library.

```
brew install robotraconteur/robotraconteur/robotraconteur
```

#### Python

The Python wrappers are distributed using PyPi.

```
pip install robotraconteur
```

#### C\#

The C\# library must be built from source.

#### Java

The Java library is available to download from the github release.

### iOS

See https://github.com/robotraconteur/robotraconteur/wiki/iOS

### Android

See https://github.com/robotraconteur/robotraconteur/wiki/Android

### MATLAB Add-on

[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/176028-robot-raconteur-matlab)

The Robot Raconteur Matlab add-on can be installed using
the [Add-On Explorer in Matlab](https://www.mathworks.com/products/matlab/add-on-explorer.html).
Search for "Robot Raconteur" and install the add-on by clicking the "Add" button. The homepage for the add-on
can be found at [https://www.mathworks.com/matlabcentral/fileexchange/176028-robot-raconteur-matlab](https://www.mathworks.com/matlabcentral/fileexchange/176028-robot-raconteur-matlab). Alternatively Click "Download from GitHub," save the file, and open the file with MATLAB to install the toolbox.

### LabVIEW Add-on

A Robot Raconteur for LabVIEW Add-on is available from Wason Technology, LLC as a commercial product.
[Click Here](https://github.com/robotraconteur/robotraconteur/wiki/LabView) for information on installation and purchasing a subscription license.

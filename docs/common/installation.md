## Installation

  - [Installation](#installation)
    - [Windows](#windows)
    - [Ubuntu Xenial, Bionic, Focal, and Jammy](#ubuntu-xenial-bionic-focal-and-jammy)
    - [Debian 10 (buster)](#debian-10-buster)
    - [Raspbian 10 (buster)](#raspbian-10-buster)
    - [Mac OSX](#mac-osx)
    - [iOS](#ios)
    - [Android](#android)

### Windows

#### C++

`vcpkg` is used to install the Robot Raconteur C++ library. See https://github.com/microsoft/vcpkg for installation instructions.

To build Robot Raconteur, clone the `vcpkg-robotraconteur` overlay repo in the vcpkg directory:

```
git clone https://github.com/robotraconteur/vcpkg-robotraconteur.git
```

and build the library:

```
vcpkg --overlay-ports=vcpkg-robotraconteur\ports install robotraconteur
```

To build x64, use:

```
vcpkg --overlay-ports=vcpkg-robotraconteur\ports install robotraconteur:x64-windows
```

#### Python

The Python wrappers are distributed using PyPi.

```
pip install robotraconteur
```

#### C\# 

The C\# library is available on NuGet. Search for "RobotRaconteurNET".

#### Java

The Java library is available on the releases page on github.

#### MATLAB

[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)

The MATLAB toolbox can be downloaded from the Matlab File Exchange. [https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur) Click "Download from GitHub" and save the file. Open the file with MATLAB to install the toolbox.

### Ubuntu Xenial, Bionic, Focal, and Jammy

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

Java must be built from source

#### MATLAB

[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)

The MATLAB toolbox can be downloaded from the Matlab File Exchange. [https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur) Click "Download from GitHub" and save the file. Open the file with MATLAB to install the toolbox.

### Debian 10 (buster)

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

Java must be built from source

### Raspbian 10 (buster)

The Raspberry Pi OS (raspbian) for amhf 32-bit processors is slightly different than the main debian armhf distributions. It uses ARMv6 instructions, instead of the ARMv7 instructions used by the main debian installation. Use the following to set up the raspbian repository, and see the debian section for the rest of the instructions.

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

#### MATLAB

[![View robotraconteur on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur)

The MATLAB toolbox can be downloaded from the Matlab File Exchange. [https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur](https://www.mathworks.com/matlabcentral/fileexchange/80509-robotraconteur) Click "Download from GitHub" and save the file. Open the file with MATLAB to install the toolbox.

### iOS

See https://github.com/robotraconteur/robotraconteur/wiki/iOS

### Android

See https://github.com/robotraconteur/robotraconteur/wiki/Android

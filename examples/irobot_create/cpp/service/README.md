# iRobot Create C++ Robot Raconteur Service Example

## Raspberry Pi arm64 Setup

### Start at boot

Ubuntu 22.04 arm64 is used to compile the C++ examples. Compiling for arm64 can be difficult so
binaries are provided on the wiki at
[Examples Binaries](https://github.com/robotraconteur/robotraconteur/wiki/Examples-Binaries).

First, copy the `irobot_create_service` file to `$HOME`.

Add your user to the `dialout` group to access the serial port:

```bash
sudo usermod -a -G dialout $USER
```

It may be necessary to log out and log back in for the group change to take effect.

Create the systemd unit file in `~/.config/systemd/user/irobot-create-service.service`:

```ini
[Unit]
Description=iRobot Create Robot Raconteur Service
After=network.target

[Service]
ExecStart=%h/irobot_create_service

[Install]
WantedBy=default.target
```

It may be necessary to create the `~/.config/systemd/user` directory.

This assumes that the default `/dev/ttyUSB0` serial port is used. If a different port is used, modify the
`ExecStart` line accordingly with the correct service device.

Enable and start the service:

```bash
systemctl --user enable irobot-create-service
systemctl --user start irobot-create-service
```

Allow the user to start services at boot:

```bash
sudo loginctl enable-linger $USER
```

Use the following command to see the status of the service:

```bash
systemctl --user status irobot-create-service.service
```


It is recommended to make the SD card read only to prevent corruption. This can be done by modifying the
`/etc/fstab` and adding the `ro` (read only) option to the root partition.

An example `/etc/fstab` before modification:

```bash
LABEL=writable  /       ext4    discard,errors=remount-ro       0 1
LABEL=system-boot       /boot/firmware  vfat    defaults        0       1
```

and after modification:

```bash
LABEL=writable  /       ext4    discard,errors=remount-ro,ro    0 1
LABEL=system-boot       /boot/firmware  vfat    defaults        0       1
```

See
[Ubuntu Fstab](https://help.ubuntu.com/community/Fstab) for more information.

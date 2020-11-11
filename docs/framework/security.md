# Security {#security}

Robot Raconteur provides security using security policies, client authentication, and TLS encrypted transports.

## Security Policy {#security_policy}

Services can optionally have a "security policy". This security policy includes an authentication mechanism and the privileges for anonymous and authenticated users. Typically the authentication is password based, using username/password pairs. Authentication may also use TLS certificates for public-key authentication. Privileges specify if authentication is required and fine-grained privileges for each user.

## TLS Certificates {#tls_certificates}

Robot Raconteur provides security through a combination of Transport Layer Security (TLS) and X.509 certificates issued by Wason Technology, LLC in partnership with Digicert.  TLS provides strong security using RSA encryption combined with a symmetric encryption algorithm, most commonly AES.  AES provides military grade encryption, and because most modern processors contain hardware acceleration, performance is very high. Certificates are used to validate the identity of nodes communicating using 4096-bit RSA keys. The use of TLS over the Robot Raconteur TCP Transport is transparent to the user requiring only a change in the URL. Using TLS requires installation of an identity X.509 certificate on the service node and optionally the client node if certificates are used for authentication. TLS 1.2 is defined in [IETF RFC 5246](https://tools.ietf.org/html/rfc5246). X.509 v3 certificates are defined in [IETF RFC 5280](https://tools.ietf.org/html/rfc5280).

### Node Identity Certificates

Identity certificates are an important part of TLS and provide a way to guarantee that the connected peer is not an imposter.  Certificates use what is called "Asymmetric Encryption".  "Asymmetric Encryption" keys have two parts: a public key that is globally available, and a private key that is kept secret and known only to the assigned node.  The public key is encased in a "Certificate" that is "signed" by an "authority" that guarantees the identity of the certificate.  The private key is installed on the server/device that corresponds to the identity. When a client connects to the server/device, it compares the certificate of the server/device to the "root certificate" distributed by the authority. This root certificate is published and is known by all devices that need to authenticate certificates. The root is automatically included in all versions of Robot Raconteur.

Certificates are issued by Wason Technology, LLC to guarantee the Node Id identity of the assigned node.  A NodeID is a 128 bit UUID that can be represented as a string of hexadecimal numbers.  An example UUID is:

    {8afda3b2-f70b-4b41-8086-71b0b3415e86}

The assigned certificate guarantees that the Node ID is unique and that only one device will have a certificate assigned to this NodeID. The common name has the form "Robot Raconteur Node {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}", where the "x" are replaced with the NodeID in hex form. For example,

    Robot Raconteur Node {8afda3b2-f70b-4b41-8086-71b0b3415e86}

### Certificate Extensions

Robot Raconteur certificates contain custom X.501 v3 extensions marked "critical". See [IETF RFC 5280](https://tools.ietf.org/html/rfc5280) for more information about certificates and extensions. Extensions that are marked critical require that the client reject the certificate if it doesn't understand the extension. Extensions not marked critical may be ignored if not understood. Because the certificates used by Robot Raconteur include these custom extensions, they cannot be used for standand DNS HTTP certificates, and standard DNS HTTP certificates cannot be used for Robot Raconteur.

See the [Robot Raconteur Public Key Certificates](https://github.com/johnwason/robotraconteur_standards/blob/master/draft/tls_certificates.md) standards document for more information.

### Certificate Distribution

Certificates for Robot Raconteur nodes are sold by Wason Technology, LLC. Please visit http://robotraconteur.com for more information on how to acquire certificates.

### Certificate Installation Locations

Certificates are loaded by Robot Raconteur from the user certificate store (Windows), or from the user's home directory (Linux, Mac OSX). The location searched for certificates are in the following table:

| OS | Location |
| --- | ---     |
| Windows | User Certificate Store |
| Linux | ~/.config/RobotRaconteur/certificates/ |
| Mac OS | ~/.config/RobotRaconteur/certificates/ |

Certificates are expected to be in PKCS #12 format with no password to be used by Robot Raconteur nodes. The file must have a .p12 extension. The file must contain the private key, the node certificate, and any intermediate certificates. For Linux and OSX, place the file in the directory specified in the table above. For Windows, certificates must be imported into the Windows user certificate store. Type "certmgr" in a command prompt, click "Import", and follow the instructions to import the file.

## Transport Security {#transport_security}

Robot Raconteur currently supports four transport types:

* LocalTransport - Provides communication between processes on the same computer
* HardwareTransport - Provides communication with USB, Bluetooth, and PCIe devices
* IntraTransport - Provides communication between nodes within the same process
* TcpTransport - Provides communication between nodes over a network using TCP protocol

Each transport has its own security concerns.

### LocalTransport

By default, LocalTransport only allows incoming connections from the same user. The `public` flag when starting the LocalTransport server will allow any user within the `robotraconteur` group to connect. The `public` flag should only be used when the system does not have untrusted users logged in to the system. An example of an innapropriate place to use the LocalTransport `public` flag is on a shared supercomputer or mainframe system with many users logged in at the same time.

With the exception of the use of the `public` flag, the LocalTransport has no special security concerns and should be considered secure by default.

### HardwareTransport

As with any type of hardware interface, only trusted devices should be connected to a computer using the USB, Bluetooth, or PCIe connections. These connections provide the hardware with a high degree of access to the system, and can easily compromise the security of the computer, regardless of the client and drivers in use. For this reason, there are no additional specific security concerns when using the Robot Raconteur HardwareTransport and the tranport can be considered secure by default.

### IntraTransport

Since the IntraTransport never leaves the executing process, it can be considered secure by default.

### TcpTransport

The TcpTransport has a number of security concerns that must be carefully managed. **Read this section carefully before deploying any Robot Raconteur devices on a network in a production environment.**

The TcpTransport is capable of a number of operating modes, with varying levels of security. By default, the TcpTransport operates in `rr+tcp` mode, which is a raw unencrypted TCP connection. `rr+ws` is a slightly modified version that wraps the data in WebSocket messages to allow for communication with web browsers and web servers. These modes are suitable for research or hobbyist use, but **are not suitable for a production environment.** Internet of Things (IoT) devices are frequently targeted due to their lax security and access to valuable targets like factories, infrastructure, and personal data. The TcpTransport provides the ability to use Transport Layer Security (TLS) to protect communication between devices. TLS encrypts the communication between devices using advanced encryption algorithms, and is an international standard used for nearly all modern network communication.

For TLS to be activated, a certificate must be acquired installed for the server node as discussed in \ref tls_certificates. The client may also optionally have a certificate installed for client certificate authentication, but it is not requires for TLS communication. The certificate verifies the identity of the server, and prevents impersonation attacks that can cause the client to give up authentication credentials to a malicious device.

Once the TLS certificate has been installed, the client may use the following secure transport options:

* `rrs+tcp` - Secure TLS/TCP connection 
* `rrs+ws` - HTTP WebSocket connection *with message data encrypted using TLS*
* `rr+wss` - HTTPS WebSocket connection *without message data encryption*
* `rrs+wss` - HTTPS WebSocket connection *with message data encrypted*

`rrs+tcp` should be used when communicating directly with a device without an HTTP elements involved, such as a Web Browser or Web Server. When a Web Browser or Web Server is involved, one of the three WebSocket options must be used: `rrs+ws`, `rr+wss`, or `rrs+wss`. The selection of which WebSocket option to use depends on the configuration of the system. The `rr+wss` and `rrs+wss` options depend on the use of HTTPS, which is the standard security method used for web pages and web services such as REST or SOAP. HTTPS depends a certificate issude by a major certificate authority that ties it to a domain name (DNS). These domain names are used to identify websites. For example `robotraconteur.com`, `wasontech.com`, `github.com`, etc are all domain names. Since IoT devices are not tied to DNS and do not have static IP addresses, this method of authentication is not available. The rule therefore is, use `rr+wss` or `rrs+wss` when an HTTPS certificate is available, such as when connecting to a web server running Robot Raconteur, and use `rrs+ws` when connecting to an IoT device that does not have HTTPS available.

`rrs+ws` must be used carefully. The initial HTTP handshake is not encrypted. Encryption is only activated to protect the contents of messages once the connection has been established. This means that the entire connection URL and other metadata used during the handshake will be exposed. **Do not include any session keys as part of the Robot Raconteur URL query when using `rr+ws`.

`rrs+wss` is "double encrypted" meaning at the entire connection is protected with HTTPS, and then the message data is again encrpted using TLS. This is only necessary when the identity of the client or server needs to be authenticated using Robot Raconteur certificates.

**Using RobotRaconteur::SecureServerNodeSetup with enforce that clients connect with secure TcpTransport settings.**

### Transport Security Summary

The following rules of thumb should be followed to use Robot Raconteur transports securely:

* **Never use `rr+tcp` or `rr+ws` in a production environment
* Use RobotRaconteur::SecureServerNodeSetup to enforce the use of secure transports
* Do not include secrets should as tokens in URLs when using `rrs+ws`
* Use `rr+wss` or `rrs+wss` when HTTPS DNS certificates are available
* Do not user LocalTransport `public` server option on shared systems
* Never connect untrusted hardware to a USB, Bluetooth, or PCIe connection
* Use certificates and certificate based authentication when possible

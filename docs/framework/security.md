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
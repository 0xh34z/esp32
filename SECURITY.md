# Security Policy

## Responsible Use

This tool is designed for **educational and research purposes only**. It demonstrates vulnerabilities in Wi-Fi networks and the 802.11 standard.

## Legal Usage

**IMPORTANT**: Only use this tool on networks you own or have explicit written permission to test.

### Legal Use Cases:
- Testing your own network security
- Authorized penetration testing with written consent
- Educational research in controlled environments
- Security auditing with proper authorization

### Illegal Activities:
❌ Unauthorized access to Wi-Fi networks
❌ Attacking networks without permission
❌ Intercepting communications you're not authorized to access
❌ Any use that violates local, state, or federal laws

## Reporting Security Issues

If you discover a security vulnerability in this project itself (not in Wi-Fi protocols), please report it responsibly:

1. **Do NOT** open a public issue
2. Email the maintainer with details
3. Allow time for a fix before public disclosure

## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND. The authors and contributors are not responsible for any misuse or damage caused by this tool.

## Wi-Fi Security Best Practices

If you're using this tool to test your network:

### For Network Administrators:
1. **Use WPA3** if your hardware supports it
2. **Strong passwords** - minimum 16 characters, random
3. **Disable WPS** - it's inherently vulnerable
4. **Update firmware** regularly on all devices
5. **Use 802.11w (PMF)** - Management Frame Protection
6. **Monitor for deauth attacks** - unusual disconnections
7. **Segment networks** - guest network separate from main
8. **Disable SSID broadcast** if appropriate for your use case

### Known Vulnerabilities This Tool Exploits:

- **PMKID Attack** - WPA2 vulnerability allowing password extraction
- **4-Way Handshake Capture** - Classic WPA/WPA2 attack vector
- **Deauthentication** - Unprotected management frames
- **Denial of Service** - Protocol-level DoS attacks

### Mitigations:

| Vulnerability | Mitigation |
|--------------|------------|
| PMKID Attack | Use WPA3, very strong passwords (20+ chars) |
| Handshake Capture | WPA3, strong passwords, 802.11w/PMF |
| Deauthentication | Enable 802.11w (PMF) |
| DoS Attacks | Limited options - monitor and filter |

## Compliance

Users are responsible for complying with all applicable laws and regulations in their jurisdiction, including but not limited to:

- Computer Fraud and Abuse Act (CFAA) - United States
- Computer Misuse Act - United Kingdom  
- Cybercrime Act - Various countries
- Local telecommunications regulations

## Ethical Guidelines

1. **Obtain written permission** before testing any network
2. **Respect privacy** - don't intercept or examine actual traffic
3. **Report vulnerabilities responsibly** if found
4. **Use for good** - improve security, don't harm it
5. **Educate others** on proper security practices

## Changes to This Policy

This security policy may be updated periodically. Check back for changes.

---

**Remember**: With great power comes great responsibility. Use this tool wisely and ethically.

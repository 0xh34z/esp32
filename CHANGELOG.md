# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-12-03

### Added
- Version tracking system with build information
- Comprehensive error handling throughout codebase
- Input validation for webserver endpoints
- Memory leak prevention in attack content allocation
- Debug utilities for memory tracking and hexdump
- GitHub Actions CI/CD workflow for automated builds
- CONTRIBUTING.md with development guidelines
- SECURITY.md with responsible disclosure policy
- TESTING.md with testing procedures and examples
- Better logging with error codes in wifi_controller
- Request size validation in webserver
- Stack size configuration for webserver
- Memory statistics logging on startup

### Changed
- Improved error handling in wifi_controller.c
- Enhanced memory management in attack.c
- Better error reporting in webserver.c
- Main.c now displays version and build info on startup

### Fixed
- Potential NULL pointer dereferences in wifi_controller
- Memory leaks when reallocating attack content
- Missing error checks in webserver endpoint handlers
- Potential crashes from invalid request sizes

### Security
- Added input validation for all user inputs
- Improved bounds checking in network operations
- Better error handling prevents information leaks

## [0.9.0] - Previous Version

Initial release with core functionality:
- PMKID capture
- WPA/WPA2 handshake capture
- Deauthentication attacks
- DOS attacks
- PCAP serialization
- HCCAPX serialization
- Web management interface

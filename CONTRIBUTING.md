# Contributing to ESP32 Wi-Fi Penetration Tool

First off, thank you for considering contributing to this project! We welcome contributions from the community.

## Code of Conduct

Please be respectful and constructive in all interactions. This project is for educational purposes.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues. When creating a bug report, include:

- **Clear title and description**
- **Steps to reproduce** the issue
- **Expected vs actual behavior**
- **ESP32 board model** and ESP-IDF version
- **Log output** if applicable

### Suggesting Enhancements

Enhancement suggestions are welcome! Please provide:

- **Clear use case** for the enhancement
- **Detailed description** of the proposed functionality
- **Potential implementation approach** if you have ideas

### Pull Requests

1. **Fork the repository** and create your branch from `master`
2. **Follow the coding style** used in the project
3. **Add Doxygen comments** for new functions and files
4. **Test your changes** on actual ESP32 hardware if possible
5. **Update documentation** if you're adding/changing functionality
6. **Keep commits atomic** - one logical change per commit
7. **Write clear commit messages**

## Development Setup

### Prerequisites

- ESP-IDF v4.1+ (see README for specific version)
- ESP32 development board
- Serial connection for debugging

### Building

```bash
idf.py build
```

### Flashing

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## Coding Standards

### Style Guidelines

- **Indentation**: 4 spaces (no tabs)
- **Line length**: Keep under 120 characters when possible
- **Braces**: Opening brace on same line for functions
- **Naming**:
  - Functions: `lowercase_with_underscores`
  - Types: `lowercase_with_underscores_t`
  - Constants: `UPPERCASE_WITH_UNDERSCORES`
  - Macros: `UPPERCASE_WITH_UNDERSCORES`

### Documentation

Use Doxygen notation for all public functions:

```c
/**
 * @brief Brief description
 * 
 * Detailed description if needed
 * 
 * @param param1 Description of param1
 * @param param2 Description of param2
 * @return Description of return value
 */
```

### Error Handling

- Use ESP-IDF error codes (`esp_err_t`)
- Check return values with `ESP_ERROR_CHECK()` or handle explicitly
- Log errors with appropriate level (`ESP_LOGE`, `ESP_LOGW`, etc.)
- Validate input parameters

### Memory Management

- Always free allocated memory
- Use `malloc`/`free` for dynamic allocation
- Check for NULL after allocation
- Avoid memory leaks

## Component Structure

Each component should have:

- **CMakeLists.txt** - Build configuration
- **README.md** - Component documentation
- **interface/** directory - Public headers
- Doxygen comments throughout

## Testing

- Test on real ESP32 hardware when possible
- Verify against different AP configurations
- Check for memory leaks using ESP-IDF tools
- Test error handling paths

## Git Workflow

1. Create feature branch: `git checkout -b feature/my-feature`
2. Make changes and commit: `git commit -am 'Add new feature'`
3. Push to your fork: `git push origin feature/my-feature`
4. Create Pull Request on GitHub

### Commit Messages

- Use present tense ("Add feature" not "Added feature")
- First line: short summary (50 chars or less)
- Blank line, then detailed description if needed
- Reference issues: "Fixes #123"

Example:
```
Add MAC address validation to wifi_controller

- Validate MAC addresses before setting
- Add error logging for invalid formats
- Update documentation

Fixes #123
```

## Areas for Contribution

Good areas to contribute:

- **New attack implementations**
- **Improved error handling**
- **Performance optimizations**
- **Documentation improvements**
- **Bug fixes**
- **Unit tests**
- **Power consumption optimizations**

## Questions?

Feel free to open an issue for questions or discussions!

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

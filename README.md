# mini-exchange

A lightweight and robust Command Line Interface (CLI) tool for currency conversion, written in C99. It leverages the [Frankfurter API](https://api.frankfurter.dev/) to provide up-to-date exchange rates for over 150 currencies.

## Features

- **Multi-currency Conversion:** Convert a value from a base currency to one or more target currencies in a single command.
- **Currency Search:** Easily find currency codes and names using a case-insensitive search.
- **Support List:** View a complete list of all ISO 4217 currency codes and names supported by the tool.
- **Overkill Build System:** Compiled with an absurd number of strict flags (`-Wall`, `-Wextra`, `-Werror`, etc.) and security hardening (`-D_FORTIFY_SOURCE=2`, `-fstack-protector-strong`, etc.) because why not.
- **Consistency Check:** Includes a built-in test mode to verify the internal currency list against the live API.

## Dependencies

The project requires the following libraries:

- `libcurl`: For making HTTP requests to the Frankfurter API.
- `libcjson`: For parsing JSON responses.

## Building

To build the project, simply run:

```bash
make
```

This will format the code, run linting checks (using `clang-tidy`), and compile the `exchange` executable with optimization and those hardening flags.

### Other Build Targets

- `make clean`: Removes object files and the executable.
- `make install`: Installs the executable to `~/.local/bin/`.
- `make asan`: Builds the project with AddressSanitizer for memory error detection.
- `make format`: Formats source files using `clang-format`.
- `make lint`: Runs `clang-tidy` for static analysis.

## Usage

```text
Usage: exchange [options] <value> <Current currency> <Desired currency(ies)>

Options:
  -h, --help        Show this help message
  -l, --list        List all supported currency codes and names
  -s, --search <q>  Search for a currency code or name
  -t, --test <iso>  Test internal currency list against API for base <iso>

Positional Arguments:
  <Desired currency(ies)> can be a single code (e.g., USD) or a comma-separated list (e.g., EUR,ZAR,XCD)
```

### Examples

**Convert 100 USD to EUR:**

```bash
./exchange 100 USD EUR
```

**Convert 100 USD to multiple currencies (EUR, ZAR, and XCD):**

```bash
./exchange 100 USD EUR,ZAR,XCD
```

**Search for a currency:**

```bash
./exchange -s "Rand"
```

**List all supported currencies:**

```bash
./exchange -l
```

## Compilation & Overkill

This project is built with a focus on being as pedantic as possible. The `Makefile` includes over 30 strict compilation flags and several hardening options because I am overkill.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

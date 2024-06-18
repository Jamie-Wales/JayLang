# JayLang

![Header Image](JayLang a progr.png)

JayLang is a student compiler project for a custom scripting language that translates source code into Java bytecode and executes it using the GraalVM native image tool.

**Note: This project is in active development.**

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Grammar](#grammar)
  - [Lexical Grammar](#lexical-grammar)
  - [Syntactic Grammar](#syntactic-grammar)
- [Examples](#examples)
- [License](#license)

## Installation

### Prerequisites

- GraalVM
- C++17 compatible compiler
- CMake (optional, for build configuration)

### Setup

1. **Clone the repository:**

   ```sh
   git clone https://github.com/jamie-wales/jaylang.git
   cd jaylang
   ```

2. **Build the project:**

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

### Compiling and Running JayLang Scripts

To compile and run a JayLang script, use the `jj` executable. Ensure your scripts have the `.jay` extension.

```sh
./jj path/to/script.jay
```

This will generate Java bytecode, assemble it using Krakatoa, and execute the resulting program with GraalVM.

The executable will have the same name as the `.jay` script.

### Example

1. Create a script named `example.jay`:

   ```jay
   log "Hello, JayLang!"
   ```

2. Compile and run the script:

   ```sh
   ./jj example.jay
   ```

3. You will have the following folder structure after compilation:

```sh
output/
└── [script name]/
    ├── src/
    │   ├── [script name].j
    │   └── [script name].class
    └── bin/
        └── [script name]
```

## Grammar

### Lexical Grammar

JayLang tokens are defined by the `Scanner` class. Here is the lexical grammar based on the `scanToken` method:

- **Single-character tokens:**
  - `(` `)` `{` `}` `,` `.` `-` `+` `;` `*` `?` `:`

- **One or two character tokens:**
  - `!` `!=`
  - `=` `==`
  - `<` `<=`
  - `>` `>=`
  - `/` `//` `/* ... */`

- **Literals:**
  - **Strings:** Enclosed in double quotes `"`.
  - **Numbers:** Digits with optional decimal points.

- **Keywords:**
  - `log`
  - `if`
  - `fun`
  - `else`
  - `true`
  - `false`
  - `nil`

- **Identifiers:** Combinations of letters and digits, starting with a letter.

### Syntactic Grammar

JayLang's syntax is defined by the `Parser` class. Here is the syntactic grammar based on the `parse` method and related parsing methods:

#### Program Structure

- `program -> statement* EOF`

#### Statements

- `statement -> printStatement | ifStatement | blockStatement | expressionStatement`
- `printStatement -> "log" expression ";" `
- `ifStatement -> "if" expression block ("else" block)?`
- `blockStatement -> "{" declaration* "}"`
- `expressionStatement -> expression ";"`

#### Declarations

- `declaration -> jjdeclaration | statement`
- `jjdeclaration -> "jj" IDENTIFIER ("=" expression)? ";"`

#### Expressions

- `expression -> assignment`
- `assignment -> IDENTIFIER "=" assignment | equality`
- `equality -> comparison (("!=" | "==") comparison)*`
- `comparison -> term ((">" | ">=" | "<" | "<=") term)*`
- `term -> factor (("+" | "-") factor)*`
- `factor -> unary (("/" | "*") unary)*`
- `unary -> ("!" | "-") unary | primary`
- `primary -> IDENTIFIER | NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"`

## Examples

### Basic Example

```jay
log "Hello, JayLang!";
```

### Conditional Example

```jay
if (true) {
    log "This is true";
} else {
    log "This is false";
}
```

## License

JayLang is licensed under the GNU General Public License (GPL) version 3. See `LICENSE` for more information.

For the full text of the license, see [GNU General Public License, version 3 (GPL-3.0)](https://www.gnu.org/licenses/gpl-3.0.en.html).

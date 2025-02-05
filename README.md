# Sneklang 🐍🚀

Sneklang is an **interpreted, statically-typed scripting language** with built-in **garbage collection** and **dynamic class loading**. Inspired by Python, but with static types and a focus on efficient execution, Sneklang aims to provide a modern yet lightweight scripting experience.

## **🔹 Current Features**
✅ **Recursive Descent Parser** with full AST generation  
✅ **Statically-Typed Variable Declarations** (e.g., `x: int = 5`)  
✅ **Operator Precedence & Associativity** (`+`, `-`, `*`, `/`)  
✅ **Garbage Collection** to manage memory efficiently  
✅ **Lexer & Tokenization** to break down scripts into structured tokens  
✅ **Expression Parsing** with precedence-aware tree structures  
✅ **AST Printing** with visually structured output  
✅ **Multiline Statement Support** (handles newlines properly)  

## **📜 Example Code in Sneklang**
```snek
x: int = 5 + 10 * 2
y: int = x * 3 + 1
```

### **Expected AST Output**
```
Declaration: x : int
├── Binary Op: +
│   ├── Literal: 5
│   └── Binary Op: *
│       ├── Literal: 10
│       └── Literal: 2
Declaration: y : int
├── Binary Op: +
│   ├── Binary Op: *
│   │   ├── Variable: x
│   │   └── Literal: 3
│   └── Literal: 1
```

## **🚀 Upcoming Features**
🔜 **Function Definitions & Calls** (e.g., `def foo(a: int) -> int { return a * 2 }`)  
🔜 **Control Flow (if/else, loops)**  
🔜 **Class Definitions & Inheritance**  
🔜 **Interpreter Execution Engine**  
🔜 **Dynamic Class Loading for Modules**  

## **💻 Building & Running Sneklang**
### **🔧 Prerequisites**
- **C Compiler** (e.g., `gcc` or `clang`)
- **Make**

### **📦 Build the Project**
```sh
make clean && make
```

### **▶️ Run a Sneklang Script**
```sh
./sneklang tests/scripts/test.snek
```

## **📂 Project Structure**
```
├── src/
│   ├── lexer/       # Tokenizer (Lexical Analysis)
│   ├── parser/      # Recursive Descent Parser & AST
│   ├── interpreter/ # Future execution engine
│   ├── core/        # Main entry point
│   ├── gc/          # Garbage Collector
├── tests/           # Test scripts
├── Makefile         # Build system
├── README.md        # Project documentation
```

## **📜 License**
Sneklang is open-source under the **GPL-3.0 License**.

---
🚀 **Current development is focused on AST refinement & interpreter execution.** Stay tuned for updates! 🔥🐍



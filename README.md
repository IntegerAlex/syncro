
---

# Syncro - Version Control System (VCS)

Syncro is a **version control system (VCS)** inspired by Git. It is a minimal implementation designed for learning purposes, offering insight into how version control systems work under the hood. Syncro provides basic functionality to manage and track changes in files and directories. This project is open to collaboration and contributions from developers interested in version control systems or learning low-level implementations of such tools.

## Features

- **Initialize repositories** with a structured directory layout.
- **Track changes** and manage references (branches and tags).
- **Add files** to the version control system.
- **Remove files** from the version control system.
- **Commit changes** to record modifications in the repository.
- **Simple implementation** to understand core VCS concepts.

## Purpose

Syncro was created as a personal project to explore the internals of version control systems like Git. It aims to demystify concepts like:
- Directory and file structure for versioning.
- References (e.g., branches and tags).
- HEAD management.

## Getting Started

### Prerequisites
- GCC or any C compiler.
- Basic understanding of C programming and file systems.

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/IntegerAlex/syncro.git
   cd syncro
   ```

2. Build the project:

   ```bash
   make
   ```

3. Install the executable:

   ```bash
   make install
   ```

4. Run the executable:

   ```bash
   syncro init
   ```

### Usage

- **Initialize a repository**:  

   ```bash
   syncro init
   ```

   This creates a `.syncro` directory with the necessary structure for version control.

- **Add files to version control**:

   ```bash
   syncro add <file1> <file2> ...
   ```

   This adds the specified files to the repository's staging area.

- **Remove files from version control**:

   ```bash
   syncro remove <file1> <file2> ...
   ```

   This removes the specified files from the repository.

- **Commit changes**:

   ```bash
   syncro commit -m "Commit message"
   ```

   This records changes made to the files in the repository with a commit message.

## Contributing

We welcome contributions! Here’s how you can help:

1. Fork the repository.
2. Create a new branch for your feature.
3. Commit your changes.
4. Push to your branch.
5. Open a pull request.

## License

This project is licensed under the **Global Open Source Software Free License (GOFL-V1)**.  
For details, see the [LICENSE](LICENSE.md) file.

## Contact

Feel free to connect and discuss ideas or improvements:
- **Author:** Akshat Kotpalliwar
- **GitHub:** [IntegerAlex](https://github.com/IntegerAlex)

---


# KelasLog v0.1

A small student-record program written in C. I made this as a command-line project to practice structs, functions, arrays, input validation, and reading/writing files.

## Features

- Add a student with an ID, name, and class/section
- List all saved students
- Find a student by ID
- Edit a student's name or class
- Save records in `students.db`, so they are still there next time
- Reject duplicate student IDs

The program supports up to 100 students in this version. IDs are case-insensitive on input and are stored in uppercase. The `|` character is reserved for the local data format, so it is not accepted in fields.

## Build and run

On macOS or Linux with a C compiler and `make` installed:

```sh
make
./kelaslog
```

To remove the compiled program:

```sh
make clean
```

You can also compile it directly:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/*.c -o kelaslog
```

## Files

- `src/main.c` contains the menu and the main flow.
- `src/student.c` contains the basic student-list operations.
- `src/storage.c` loads and saves the records.
- `src/input.c` handles keyboard input and validation.
- `include/` contains the matching header files.

`students.db` is created in the folder where you run the program. It is a plain text file; each line contains `ID|Name|Class`. It is local data, not a database server.

## Current limits

This is a terminal app for a small class, not a multi-user system. It has no login, network access, or backup automation. Keep a copy of `students.db` if the records matter.

## Next version

The next version will add dated attendance records and a summary for each student.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

Copyright © 2026 Mikael Kalesaran. All rights reserved.
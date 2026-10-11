# KelasLog v0.2

KelasLog is a command-line student and attendance tracker written in C. This version builds on the student list and adds dated attendance records.

## Features

- Add, list, search, and edit student records
- Record attendance as Present, Absent, Excused, or Sick
- View the attendance records for a date
- View a student's attendance totals and rate
- Save student and attendance data between runs
- Reject invalid dates and duplicate attendance for the same student on the same date

There is room for 100 students and 5,000 attendance records. The program records one student at a time, which keeps this version small enough to follow and improve.

## Attendance percentage

The rate is calculated as `Present / (Present + Absent) * 100`. Excused and Sick entries are not included in the denominator. A student with no Present or Absent entries is shown as `N/A`.

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

Or compile directly:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/*.c -o kelaslog
```

## Data files

- `students.db`: `ID|Name|Class`
- `attendance.db`: `ID|YYYY-MM-DD|Status`

Both files are plain text and are created in the folder where you run the program. Back them up manually if you need to keep the data.

## Current limits

This is intended for one person using one computer. There is no account system, cloud sync, or simultaneous editing. Attendance is entered individually, rather than marking an entire class in one step.

## Next version

The next version will add a low-attendance report, CSV export, and a few quality-of-life improvements.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

Copyright © 2026 Mikael Kalesaran. All rights reserved.

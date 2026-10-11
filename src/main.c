#include "attendance.h"
#include "input.h"
#include "storage.h"
#include "student.h"

#include <stdio.h>
#include <string.h>

#define STUDENT_FILE "students.db"
#define ATTENDANCE_FILE "attendance.db"

static void print_menu(void)
{
    puts("\nKelasLog - Student Attendance");
    puts("-----------------------------");
    puts("1. Add student");
    puts("2. List students");
    puts("3. Search student");
    puts("4. Edit student");
    puts("5. Record attendance");
    puts("6. View attendance by date");
    puts("7. Student attendance summary");
    puts("8. Exit");
}

static void print_student(const Student *student)
{
    printf("%-15s %-34s %-12s\n", student->id, student->name, student->class_name);
}

static void list_students(const Student students[], size_t count)
{
    size_t i;
    if (count == 0) {
        puts("No students yet. Add a student first.");
        return;
    }
    puts("\nStudent ID      Name                               Class");
    puts("---------------------------------------------------------------");
    for (i = 0; i < count; i++) print_student(&students[i]);
    printf("\nTotal students: %zu/%d\n", count, MAX_STUDENTS);
}

static int add_student(Student students[], size_t *count)
{
    Student student;
    int result;
    if (*count >= MAX_STUDENTS) {
        puts("The student list is full.");
        return 1;
    }
    puts("\nAdd student");
    result = input_read_id("Student ID: ", student.id, sizeof(student.id));
    if (result <= 0) return result;
    if (student_find_index(students, *count, student.id) != *count) {
        puts("That student ID is already in the list.");
        return 1;
    }
    result = input_read_required("Full name: ", student.name, sizeof(student.name));
    if (result <= 0) return result;
    result = input_read_required("Class / section: ", student.class_name, sizeof(student.class_name));
    if (result <= 0) return result;
    if (!student_add(students, count, &student)) {
        puts("Could not add the student. The ID may already exist or the list is full.");
        return 1;
    }
    if (!storage_save_students(STUDENT_FILE, students, *count)) {
        (*count)--;
        puts("The student was not added because the data file could not be saved.");
        return 1;
    }
    puts("Student saved.");
    return 1;
}

static void search_student(const Student students[], size_t count)
{
    char id[STUDENT_ID_SIZE];
    size_t index;
    int result = input_read_id("Student ID to find: ", id, sizeof(id));
    if (result <= 0) return;
    index = student_find_index(students, count, id);
    if (index == count) {
        puts("No student found with that ID.");
        return;
    }
    puts("\nStudent ID      Name                               Class");
    puts("---------------------------------------------------------------");
    print_student(&students[index]);
}

static int edit_student(Student students[], size_t count)
{
    char id[STUDENT_ID_SIZE];
    Student updated, original;
    size_t index;
    int result = input_read_id("Student ID to edit: ", id, sizeof(id));
    if (result <= 0) return result;
    index = student_find_index(students, count, id);
    if (index == count) {
        puts("No student found with that ID.");
        return 1;
    }
    original = students[index];
    updated = original;
    printf("Current name: %s\n", updated.name);
    result = input_read_required("New name: ", updated.name, sizeof(updated.name));
    if (result <= 0) return result;
    printf("Current class: %s\n", updated.class_name);
    result = input_read_required("New class / section: ", updated.class_name, sizeof(updated.class_name));
    if (result <= 0) return result;
    students[index] = updated;
    if (!storage_save_students(STUDENT_FILE, students, count)) {
        students[index] = original;
        puts("The changes were cancelled because the data file could not be saved.");
        return 1;
    }
    puts("Student updated.");
    return 1;
}

static int read_date(char date[ATTENDANCE_DATE_SIZE])
{
    int result;
    for (;;) {
        result = input_read_required("Date (YYYY-MM-DD): ", date, ATTENDANCE_DATE_SIZE);
        if (result <= 0) return result;
        if (attendance_is_valid_date(date)) return 1;
        puts("Use a real date in YYYY-MM-DD format (years 1900-2100). ");
    }
}

static int record_attendance(const Student students[], size_t student_count,
                             Attendance records[], size_t *record_count)
{
    Attendance record;
    char date[ATTENDANCE_DATE_SIZE];
    size_t student_index;
    int result, status_choice, add_result;

    if (student_count == 0) {
        puts("Add a student before recording attendance.");
        return 1;
    }
    puts("\nRecord attendance");
    result = read_date(date);
    if (result <= 0) return result;
    result = input_read_id("Student ID: ", record.student_id, sizeof(record.student_id));
    if (result <= 0) return result;
    student_index = student_find_index(students, student_count, record.student_id);
    if (student_index == student_count) {
        puts("That student ID is not in the student list.");
        return 1;
    }
    if (attendance_find_index(records, *record_count, record.student_id, date) != *record_count) {
        puts("Attendance for this student and date already exists.");
        return 1;
    }

    puts("1. Present");
    puts("2. Absent");
    puts("3. Excused");
    puts("4. Sick");
    result = input_read_int("Status: ", 1, 4, &status_choice);
    if (result <= 0) return result;
    record.status = (AttendanceStatus)status_choice;
    strcpy(record.date, date);

    add_result = attendance_add(records, record_count, &record);
    if (add_result == 0) {
        puts("Attendance for this student and date already exists.");
        return 1;
    }
    if (add_result < 0) {
        puts("The attendance file has reached its record limit.");
        return 1;
    }
    if (!storage_save_attendance(ATTENDANCE_FILE, records, *record_count)) {
        (*record_count)--;
        puts("Attendance was not added because the file could not be saved.");
        return 1;
    }
    printf("Saved: %s - %s on %s.\n", students[student_index].name,
           attendance_status_name(record.status), record.date);
    return 1;
}

static void view_attendance_by_date(const Student students[], size_t student_count,
                                    const Attendance records[], size_t record_count)
{
    char date[ATTENDANCE_DATE_SIZE];
    size_t i, shown = 0;
    int result = read_date(date);
    if (result <= 0) return;

    puts("\nStudent ID      Name                               Status");
    puts("---------------------------------------------------------------");
    for (i = 0; i < record_count; i++) {
        size_t index;
        if (strcmp(records[i].date, date) != 0) continue;
        index = student_find_index(students, student_count, records[i].student_id);
        if (index == student_count) {
            printf("%-15s %-34s %s\n", records[i].student_id, "(deleted student)",
                   attendance_status_name(records[i].status));
        } else {
            printf("%-15s %-34s %s\n", students[index].id, students[index].name,
                   attendance_status_name(records[i].status));
        }
        shown++;
    }
    if (shown == 0) puts("No attendance records for that date.");
    else printf("\nRecords shown: %zu\n", shown);
}

static void print_stats(const AttendanceStats *stats)
{
    size_t countable = stats->present + stats->absent;
    printf("Present: %zu\n", stats->present);
    printf("Absent:  %zu\n", stats->absent);
    printf("Excused: %zu\n", stats->excused);
    printf("Sick:    %zu\n", stats->sick);
    if (countable == 0) {
        puts("Attendance rate: N/A (no countable sessions)");
    } else {
        printf("Attendance rate: %.1f%% (excused and sick entries are excluded)\n",
               100.0 * (double)stats->present / (double)countable);
    }
}

static void student_summary(const Student students[], size_t student_count,
                            const Attendance records[], size_t record_count)
{
    char id[STUDENT_ID_SIZE];
    size_t index;
    AttendanceStats stats;
    int result = input_read_id("Student ID: ", id, sizeof(id));
    if (result <= 0) return;
    index = student_find_index(students, student_count, id);
    if (index == student_count) {
        puts("No student found with that ID.");
        return;
    }
    attendance_get_stats(records, record_count, id, &stats);
    printf("\nAttendance summary: %s (%s)\n", students[index].name, students[index].id);
    print_stats(&stats);
}

int main(void)
{
    Student students[MAX_STUDENTS];
    Attendance records[MAX_ATTENDANCE];
    size_t student_count = 0, record_count = 0;
    int choice, result;

    if (!storage_load_students(STUDENT_FILE, students, &student_count) ||
        !storage_load_attendance(ATTENDANCE_FILE, records, &record_count)) {
        puts("Could not load the data files. Check the files and try again.");
        return 1;
    }
    printf("Loaded %zu student(s) and %zu attendance record(s).\n", student_count, record_count);

    for (;;) {
        print_menu();
        result = input_read_int("Choose an option: ", 1, 8, &choice);
        if (result < 0) break;
        if (result == 0) continue;
        switch (choice) {
            case 1:
                result = add_student(students, &student_count);
                if (result < 0) return 0;
                break;
            case 2: list_students(students, student_count); break;
            case 3: search_student(students, student_count); break;
            case 4:
                result = edit_student(students, student_count);
                if (result < 0) return 0;
                break;
            case 5:
                result = record_attendance(students, student_count, records, &record_count);
                if (result < 0) return 0;
                break;
            case 6: view_attendance_by_date(students, student_count, records, record_count); break;
            case 7: student_summary(students, student_count, records, record_count); break;
            case 8: puts("Goodbye."); return 0;
            default: break;
        }
    }
    puts("\nInput ended. Goodbye.");
    return 0;
}

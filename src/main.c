#include "input.h"
#include "storage.h"
#include "student.h"

#include <stdio.h>
#include <string.h>

#define STUDENT_FILE "students.db"

static void print_menu(void)
{
    puts("\nKelasLog - Student Records");
    puts("-------------------------");
    puts("1. Add student");
    puts("2. List students");
    puts("3. Search student");
    puts("4. Edit student");
    puts("5. Exit");
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
    for (i = 0; i < count; i++) {
        print_student(&students[i]);
    }
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

    if (result < 0) return;
    if (result == 0) return;
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
    int result;

    result = input_read_id("Student ID to edit: ", id, sizeof(id));
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

int main(void)
{
    Student students[MAX_STUDENTS];
    size_t count = 0;
    int choice;
    int result;

    if (!storage_load_students(STUDENT_FILE, students, &count)) {
        puts("Could not load student records. Check the file and try again.");
        return 1;
    }

    printf("Loaded %zu student record(s).\n", count);
    for (;;) {
        print_menu();
        result = input_read_int("Choose an option: ", 1, 5, &choice);
        if (result < 0) break;
        if (result == 0) continue;

        switch (choice) {
            case 1:
                result = add_student(students, &count);
                if (result < 0) return 0;
                break;
            case 2:
                list_students(students, count);
                break;
            case 3:
                search_student(students, count);
                break;
            case 4:
                result = edit_student(students, count);
                if (result < 0) return 0;
                break;
            case 5:
                puts("Goodbye.");
                return 0;
            default:
                break;
        }
    }

    puts("\nInput ended. Goodbye.");
    return 0;
}

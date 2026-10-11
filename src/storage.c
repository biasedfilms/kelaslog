#include "storage.h"
#include "input.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static int save_file_finish(FILE *file, const char *temporary, const char *filename)
{
    if (fclose(file) != 0) {
        perror("Could not finish saving data");
        remove(temporary);
        return 0;
    }
    if (rename(temporary, filename) != 0) {
        perror("Could not replace data file");
        remove(temporary);
        return 0;
    }
    return 1;
}

static FILE *open_temporary(const char *filename, char *temporary, size_t size)
{
    if (snprintf(temporary, size, "%s.tmp", filename) >= (int)size) {
        fprintf(stderr, "Data filename is too long.\n");
        return NULL;
    }
    return fopen(temporary, "w");
}

static int parse_student(char *line, Student *student)
{
    char *first = strchr(line, '|');
    char *second;

    if (first == NULL) return 0;
    *first = '\0';
    second = strchr(first + 1, '|');
    if (second == NULL) return 0;
    *second = '\0';

    if (strchr(second + 1, '|') != NULL || !input_is_valid_id(line) ||
        first[1] == '\0' || second[1] == '\0' ||
        strlen(line) >= sizeof(student->id) ||
        strlen(first + 1) >= sizeof(student->name) ||
        strlen(second + 1) >= sizeof(student->class_name)) return 0;

    strcpy(student->id, line);
    strcpy(student->name, first + 1);
    strcpy(student->class_name, second + 1);
    return 1;
}

int storage_load_students(const char *filename, Student students[], size_t *count)
{
    FILE *file;
    char line[256];
    size_t loaded = 0;
    int line_number = 0;

    *count = 0;
    file = fopen(filename, "r");
    if (file == NULL) {
        if (errno == ENOENT) return 1;
        perror(filename);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Student student;
        size_t length;
        line_number++;
        length = strlen(line);
        if (length > 0 && line[length - 1] == '\n') line[--length] = '\0';
        else if (!feof(file)) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) { }
            fprintf(stderr, "Skipping a long row in %s at line %d.\n", filename, line_number);
            continue;
        }
        if (length > 0 && line[length - 1] == '\r') line[--length] = '\0';
        if (!parse_student(line, &student) || loaded >= MAX_STUDENTS ||
            student_find_index(students, loaded, student.id) != loaded) {
            fprintf(stderr, "Skipping an invalid or duplicate row in %s at line %d.\n", filename, line_number);
            continue;
        }
        students[loaded++] = student;
    }
    if (ferror(file)) {
        perror("Could not read student data");
        fclose(file);
        return 0;
    }
    fclose(file);
    *count = loaded;
    return 1;
}

int storage_save_students(const char *filename, const Student students[], size_t count)
{
    FILE *file;
    char temporary[300];
    size_t i;

    file = open_temporary(filename, temporary, sizeof(temporary));
    if (file == NULL) {
        perror(filename);
        return 0;
    }
    for (i = 0; i < count; i++) {
        if (fprintf(file, "%s|%s|%s\n", students[i].id, students[i].name,
                    students[i].class_name) < 0) {
            perror("Could not write student data");
            fclose(file);
            remove(temporary);
            return 0;
        }
    }
    return save_file_finish(file, temporary, filename);
}

static int parse_attendance(char *line, Attendance *record)
{
    char *first = strchr(line, '|');
    char *second;
    AttendanceStatus status;

    if (first == NULL) return 0;
    *first = '\0';
    second = strchr(first + 1, '|');
    if (second == NULL) return 0;
    *second = '\0';
    if (strchr(second + 1, '|') != NULL || !input_is_valid_id(line) ||
        strlen(line) >= sizeof(record->student_id) ||
        strlen(first + 1) != 10 || !attendance_is_valid_date(first + 1) ||
        !attendance_status_from_name(second + 1, &status)) return 0;

    strcpy(record->student_id, line);
    strcpy(record->date, first + 1);
    record->status = status;
    return 1;
}

int storage_load_attendance(const char *filename, Attendance records[], size_t *count)
{
    FILE *file;
    char line[256];
    size_t loaded = 0;
    int line_number = 0;

    *count = 0;
    file = fopen(filename, "r");
    if (file == NULL) {
        if (errno == ENOENT) return 1;
        perror(filename);
        return 0;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        Attendance record;
        size_t length;
        line_number++;
        length = strlen(line);
        if (length > 0 && line[length - 1] == '\n') line[--length] = '\0';
        else if (!feof(file)) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) { }
            fprintf(stderr, "Skipping a long row in %s at line %d.\n", filename, line_number);
            continue;
        }
        if (length > 0 && line[length - 1] == '\r') line[--length] = '\0';
        if (!parse_attendance(line, &record) || loaded >= MAX_ATTENDANCE ||
            attendance_find_index(records, loaded, record.student_id, record.date) != loaded) {
            fprintf(stderr, "Skipping an invalid or duplicate row in %s at line %d.\n", filename, line_number);
            continue;
        }
        records[loaded++] = record;
    }
    if (ferror(file)) {
        perror("Could not read attendance data");
        fclose(file);
        return 0;
    }
    fclose(file);
    *count = loaded;
    return 1;
}

int storage_save_attendance(const char *filename, const Attendance records[], size_t count)
{
    FILE *file;
    char temporary[300];
    size_t i;

    file = open_temporary(filename, temporary, sizeof(temporary));
    if (file == NULL) {
        perror(filename);
        return 0;
    }
    for (i = 0; i < count; i++) {
        if (fprintf(file, "%s|%s|%s\n", records[i].student_id, records[i].date,
                    attendance_status_name(records[i].status)) < 0) {
            perror("Could not write attendance data");
            fclose(file);
            remove(temporary);
            return 0;
        }
    }
    return save_file_finish(file, temporary, filename);
}

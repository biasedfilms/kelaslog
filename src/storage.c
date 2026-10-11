#include "storage.h"
#include "input.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static int parse_student(char *line, Student *student)
{
    char *first = strchr(line, '|');
    char *second;

    if (first == NULL) {
        return 0;
    }
    *first = '\0';
    second = strchr(first + 1, '|');
    if (second == NULL) {
        return 0;
    }
    *second = '\0';

    if (strchr(second + 1, '|') != NULL || !input_is_valid_id(line) ||
        first[1] == '\0' || second[1] == '\0' ||
        strlen(line) >= sizeof(student->id) ||
        strlen(first + 1) >= sizeof(student->name) ||
        strlen(second + 1) >= sizeof(student->class_name)) {
        return 0;
    }

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
        if (errno == ENOENT) {
            return 1;
        }
        perror(filename);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Student student;
        size_t length;
        line_number++;
        length = strlen(line);
        if (length > 0 && line[length - 1] == '\n') {
            line[--length] = '\0';
        } else if (!feof(file)) {
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF) {
                /* Skip a damaged, overlong row. */
            }
            fprintf(stderr, "Skipping a long row in %s at line %d.\n", filename, line_number);
            continue;
        }
        if (length > 0 && line[length - 1] == '\r') {
            line[--length] = '\0';
        }
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

    if (snprintf(temporary, sizeof(temporary), "%s.tmp", filename) >= (int)sizeof(temporary)) {
        fprintf(stderr, "Student data filename is too long.\n");
        return 0;
    }

    file = fopen(temporary, "w");
    if (file == NULL) {
        perror(temporary);
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

    if (fclose(file) != 0) {
        perror("Could not finish saving student data");
        remove(temporary);
        return 0;
    }
    if (rename(temporary, filename) != 0) {
        perror("Could not replace student data file");
        remove(temporary);
        return 0;
    }
    return 1;
}

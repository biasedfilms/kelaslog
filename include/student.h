#ifndef STUDENT_H
#define STUDENT_H

#include <stddef.h>

#define MAX_STUDENTS 100
#define STUDENT_ID_SIZE 16
#define STUDENT_NAME_SIZE 80
#define CLASS_NAME_SIZE 24

typedef struct {
    char id[STUDENT_ID_SIZE];
    char name[STUDENT_NAME_SIZE];
    char class_name[CLASS_NAME_SIZE];
} Student;

size_t student_find_index(const Student students[], size_t count, const char *id);
int student_add(Student students[], size_t *count, const Student *student);

#endif

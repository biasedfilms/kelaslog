#include "student.h"

#include <string.h>

size_t student_find_index(const Student students[], size_t count, const char *id)
{
    size_t i;

    for (i = 0; i < count; i++) {
        if (strcmp(students[i].id, id) == 0) {
            return i;
        }
    }
    return count;
}

int student_add(Student students[], size_t *count, const Student *student)
{
    if (*count >= MAX_STUDENTS || student_find_index(students, *count, student->id) != *count) {
        return 0;
    }

    students[*count] = *student;
    (*count)++;
    return 1;
}

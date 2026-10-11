#ifndef STORAGE_H
#define STORAGE_H

#include "student.h"

int storage_load_students(const char *filename, Student students[], size_t *count);
int storage_save_students(const char *filename, const Student students[], size_t count);

#endif

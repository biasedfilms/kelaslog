#ifndef STORAGE_H
#define STORAGE_H

#include "attendance.h"
#include "student.h"

int storage_load_students(const char *filename, Student students[], size_t *count);
int storage_save_students(const char *filename, const Student students[], size_t count);
int storage_load_attendance(const char *filename, Attendance records[], size_t *count);
int storage_save_attendance(const char *filename, const Attendance records[], size_t count);

#endif

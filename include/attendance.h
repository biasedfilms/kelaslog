#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <stddef.h>

#define MAX_ATTENDANCE 5000
#define ATTENDANCE_DATE_SIZE 11

typedef enum {
    STATUS_PRESENT = 1,
    STATUS_ABSENT,
    STATUS_EXCUSED,
    STATUS_SICK
} AttendanceStatus;

typedef struct {
    char student_id[16];
    char date[ATTENDANCE_DATE_SIZE];
    AttendanceStatus status;
} Attendance;

typedef struct {
    size_t present;
    size_t absent;
    size_t excused;
    size_t sick;
} AttendanceStats;

int attendance_is_valid_date(const char *date);
const char *attendance_status_name(AttendanceStatus status);
int attendance_status_from_name(const char *name, AttendanceStatus *status);
size_t attendance_find_index(const Attendance records[], size_t count,
                             const char *student_id, const char *date);
int attendance_add(Attendance records[], size_t *count, const Attendance *record);
void attendance_get_stats(const Attendance records[], size_t count,
                          const char *student_id, AttendanceStats *stats);

#endif

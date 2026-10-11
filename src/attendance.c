#include "attendance.h"

#include <ctype.h>
#include <string.h>

int attendance_is_valid_date(const char *date)
{
    int year, month, day;
    int days_in_month;
    size_t i;

    if (date == NULL || strlen(date) != 10 || date[4] != '-' || date[7] != '-') {
        return 0;
    }
    for (i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) return 0;
    }

    year = (date[0] - '0') * 1000 + (date[1] - '0') * 100 +
           (date[2] - '0') * 10 + (date[3] - '0');
    month = (date[5] - '0') * 10 + (date[6] - '0');
    day = (date[8] - '0') * 10 + (date[9] - '0');

    if (year < 1900 || year > 2100 || month < 1 || month > 12) return 0;

    switch (month) {
        case 4: case 6: case 9: case 11:
            days_in_month = 30;
            break;
        case 2:
            days_in_month = 28;
            if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) {
                days_in_month = 29;
            }
            break;
        default:
            days_in_month = 31;
            break;
    }
    return day >= 1 && day <= days_in_month;
}

const char *attendance_status_name(AttendanceStatus status)
{
    switch (status) {
        case STATUS_PRESENT: return "Present";
        case STATUS_ABSENT: return "Absent";
        case STATUS_EXCUSED: return "Excused";
        case STATUS_SICK: return "Sick";
        default: return "Unknown";
    }
}

int attendance_status_from_name(const char *name, AttendanceStatus *status)
{
    if (strcmp(name, "Present") == 0) *status = STATUS_PRESENT;
    else if (strcmp(name, "Absent") == 0) *status = STATUS_ABSENT;
    else if (strcmp(name, "Excused") == 0) *status = STATUS_EXCUSED;
    else if (strcmp(name, "Sick") == 0) *status = STATUS_SICK;
    else return 0;
    return 1;
}

size_t attendance_find_index(const Attendance records[], size_t count,
                             const char *student_id, const char *date)
{
    size_t i;
    for (i = 0; i < count; i++) {
        if (strcmp(records[i].student_id, student_id) == 0 &&
            strcmp(records[i].date, date) == 0) {
            return i;
        }
    }
    return count;
}

int attendance_add(Attendance records[], size_t *count, const Attendance *record)
{
    if (*count >= MAX_ATTENDANCE) return -1;
    if (attendance_find_index(records, *count, record->student_id, record->date) != *count) return 0;
    records[*count] = *record;
    (*count)++;
    return 1;
}

void attendance_get_stats(const Attendance records[], size_t count,
                          const char *student_id, AttendanceStats *stats)
{
    size_t i;
    memset(stats, 0, sizeof(*stats));
    for (i = 0; i < count; i++) {
        if (strcmp(records[i].student_id, student_id) != 0) continue;
        switch (records[i].status) {
            case STATUS_PRESENT: stats->present++; break;
            case STATUS_ABSENT: stats->absent++; break;
            case STATUS_EXCUSED: stats->excused++; break;
            case STATUS_SICK: stats->sick++; break;
            default: break;
        }
    }
}

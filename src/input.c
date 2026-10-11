#include "input.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int input_read_line(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    int ch;
    char *start;

    if (size < 2) {
        return 0;
    }

    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return -1;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[--length] = '\0';
    } else if (!feof(stdin)) {
        ch = getchar();
        if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
            buffer[0] = '\0';
            puts("That input is too long. Please try again.");
            return 0;
        }
    }

    if (length > 0 && buffer[length - 1] == '\r') {
        buffer[--length] = '\0';
    }

    start = buffer;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != buffer) {
        memmove(buffer, start, strlen(start) + 1);
    }

    length = strlen(buffer);
    while (length > 0 && isspace((unsigned char)buffer[length - 1])) {
        buffer[--length] = '\0';
    }

    return 1;
}

int input_read_required(const char *prompt, char *buffer, size_t size)
{
    int result;

    for (;;) {
        result = input_read_line(prompt, buffer, size);
        if (result <= 0) {
            return result;
        }
        if (buffer[0] == '\0') {
            puts("This field cannot be empty.");
            continue;
        }
        if (strchr(buffer, '|') != NULL || strchr(buffer, '\t') != NULL) {
            puts("Please do not use | or tab characters in this field.");
            continue;
        }
        return 1;
    }
}

int input_is_valid_id(const char *value)
{
    size_t i;

    if (value == NULL || value[0] == '\0') {
        return 0;
    }

    for (i = 0; value[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)value[i];
        if (!isalnum(ch) && ch != '-' && ch != '_') {
            return 0;
        }
    }

    return i < 16;
}

int input_read_id(const char *prompt, char *buffer, size_t size)
{
    size_t i;
    int result;

    for (;;) {
        result = input_read_required(prompt, buffer, size);
        if (result <= 0) {
            return result;
        }
        if (!input_is_valid_id(buffer)) {
            puts("Use up to 15 letters or numbers. Hyphens and underscores are fine.");
            continue;
        }
        for (i = 0; buffer[i] != '\0'; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }
        return 1;
    }
}

int input_read_int(const char *prompt, int min_value, int max_value, int *value)
{
    char buffer[64];
    char *end;
    long parsed;
    int result = input_read_line(prompt, buffer, sizeof(buffer));

    if (result <= 0) {
        return result;
    }
    if (buffer[0] == '\0') {
        puts("Enter a number from the menu.");
        return 0;
    }

    errno = 0;
    parsed = strtol(buffer, &end, 10);
    if (errno != 0 || *end != '\0' || parsed < min_value || parsed > max_value) {
        printf("Enter a whole number between %d and %d.\n", min_value, max_value);
        return 0;
    }

    *value = (int)parsed;
    return 1;
}

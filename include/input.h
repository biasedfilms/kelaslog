#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

int input_read_line(const char *prompt, char *buffer, size_t size);
int input_read_required(const char *prompt, char *buffer, size_t size);
int input_read_id(const char *prompt, char *buffer, size_t size);
int input_read_int(const char *prompt, int min_value, int max_value, int *value);
int input_is_valid_id(const char *value);

#endif

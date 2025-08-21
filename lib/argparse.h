#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    unsigned char* pkg; // Path to the PKG file
    

    bool flag_info;
    bool flag_patch_content_id;

    unsigned char new_content_id[0x24];

    bool exit; // If true, pkg4t exits after argument parsing
    int  exitcode; // Exitcode if exit is true
} Args;

Args* parse_args(int, char**);
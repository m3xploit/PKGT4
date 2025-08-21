#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "lib/argparse.h"
#include "lib/pkg.h"

int main(int argc, char* argv[]) {
    Args* args = parse_args(argc, argv);
    if (args == NULL) // malloc() failed somewhere
        return 1;
    
    if (args->exit) {
        free(args);
        return args->exitcode;
    }

    struct stat pkg_stat;
    if (stat(args->pkg, &pkg_stat) != 0) { // Check if PKG file exists
        fprintf(stderr, "Hmm, that PKG file does not seem to exist ...\n");
        
        free(args->pkg);
        free(args);

        return 1;
    }

    FILE* pkg_fp = (FILE*)fopen(args->pkg, "r+b");
    if (pkg_fp == NULL) {
        fprintf(stderr, "Failed to open PKG (??? fopen() returned NULL) Dont worry, your PKG is fine!\n");
        
        free(args->pkg);
        free(args);

        return 1;
    }

    PKG_Header pkg_header;
    fread(&pkg_header, 1, sizeof(PKG_Header), pkg_fp);

    // Check if PKG is an valid PS4 package
    if (
        pkg_header.pkg_magic[0] != 0x7F || // .
        pkg_header.pkg_magic[1] != 0x43 || // C
        pkg_header.pkg_magic[2] != 0x4E || // N
        pkg_header.pkg_magic[3] != 0x54    // T
    ) { // 7F 43 4E 54 (.CNT)
        fprintf(stderr, "Are you sure this is an PS4 PKG file?\n");

        fclose(pkg_fp);
        free(args->pkg);
        free(args);

        return 1;
    }

    // Here we start doing what the user wants us to do

    if (args->flag_info) { // User wants us to display as much information as possible about the PKG
        // I know that this is not "as much information as possible" but more will be added soon

        unsigned char* title_id = get_title_id_from_content_id(pkg_header.pkg_content_id);
        
        printf("Package : %s\n\n", args->pkg);

        printf("Content ID: %s\n", pkg_header.pkg_content_id);
        printf("Title ID: %s\n", title_id);

        free(title_id);
    }

    if (args->flag_patch_content_id) { // User wants us to patch the current content id to a new one
        printf("Seeking to offset 0x40 ...\n");
        fseek(pkg_fp, 0x40, SEEK_SET); // Goto content ID offset

        printf("Current content ID: %s\n", pkg_header.pkg_content_id);

        if (strcmp(pkg_header.pkg_content_id, args->new_content_id) != 0) { // Check if content ids already match
            for (int content_id_index = 0; content_id_index < 36; content_id_index++) {

                if (pkg_header.pkg_content_id[content_id_index] == args->new_content_id[content_id_index]) { // Check if the byte does not need to be patched
                    printf("No need to patch byte at position 0x%x, byte already matches with %x\n", 0x40 + content_id_index, pkg_header.pkg_content_id[content_id_index]);
                    fseek(pkg_fp, ftell(pkg_fp) + 1, SEEK_SET);
                }
                else { // Byte needs to be patched
                    printf("Patching %x to %x at position 0x%x ...\n", pkg_header.pkg_content_id[content_id_index], args->new_content_id[content_id_index], 0x40 + content_id_index);
                    fwrite(&args->new_content_id[content_id_index], 1, 1, pkg_fp);
                }
            }

            // Validate patch
            printf("Validating patch ...\n");
            fseek(pkg_fp, 0, SEEK_SET);

            PKG_Header pkg_header_tmp;
            fread(&pkg_header_tmp, 1, sizeof(PKG_Header), pkg_fp);
            
            if (strcmp(pkg_header_tmp.pkg_content_id, args->new_content_id) == 0) { 
                printf("Patch was successful! :)\n");
                printf("New content ID: %s\n", args->new_content_id);
            }
            else
                printf("Patch failed! :(\n");
            
        }
        else
            printf("Content IDs match! No need to patch.\n");
    }

    if (args->flag_patch_title_id) { // User wants us to patch the current content id to a new one
        printf("Seeking to offset 0x47 ...\n");
        fseek(pkg_fp, 0x47, SEEK_SET); // Goto title ID offset

        unsigned char* current_title_id = get_title_id_from_content_id(pkg_header.pkg_content_id);
        printf("Current title ID: %s\n", current_title_id);

        if (strcmp(current_title_id, args->new_title_id) != 0) { // Check if title IDs already match
            
            for (int title_id_index = 0; title_id_index < 9; title_id_index++) {
                if (current_title_id[title_id_index] == args->new_title_id[title_id_index]) { // Check if the byte does not need to be patched
                    printf("No need to patch byte at position 0x%x, byte already matches with %x\n", 0x47 + title_id_index, current_title_id[title_id_index]);
                    fseek(pkg_fp, ftell(pkg_fp) + 1, SEEK_SET);
                }
                else { // Byte needs to be patched
                    printf("Patching %x to %x at position 0x%x ...\n", current_title_id[title_id_index], args->new_title_id[title_id_index], 0x47 + title_id_index);
                    fwrite(&args->new_title_id[title_id_index], 1, 1, pkg_fp);
                }
            }

            // Validate patch
            printf("Validating patch ...\n");
            fseek(pkg_fp, 0, SEEK_SET);

            PKG_Header pkg_header_tmp;
            fread(&pkg_header_tmp, 1, sizeof(PKG_Header), pkg_fp);
            
            unsigned char* current_title_id_tmp = get_title_id_from_content_id(pkg_header_tmp.pkg_content_id);

            if (strcmp(current_title_id_tmp, args->new_title_id) == 0) { 
                printf("Patch was successful! :)\n");
                printf("New title ID: %s\n", args->new_title_id);
            }
            else
                printf("Patch failed! :(\n");

            free(current_title_id_tmp);
        }
        else
            printf("Title IDs match! No need to patch.\n");

        free(current_title_id);
    }
    
    if (!args->flag_patch_content_id && !args->flag_info && !args->flag_patch_title_id) // User specified PKG file but no args
        printf("What do you want me to do?? Please take a look at the examples\n");

    fclose(pkg_fp);
    free(args->pkg);
    free(args);

    return 0;
}
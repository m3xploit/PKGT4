#include "argparse.h"

Args* parse_args(int argc, char* argv[]) {
    Args* args = (Args*)malloc(sizeof(Args));
    if (args == NULL) { // Failed to allocate memory on the heap
        fprintf(stderr, "Failed to allocate memory on the heap for struct (??? malloc() returned NULL)\n");
        return NULL;
    }

    // Init values in args struct
    args->exit = false;
    args->flag_info = false;
    args->flag_patch_content_id = false;
    args->flag_patch_title_id = false;
    args->pkg = NULL;

    printf("PKGT4 - Command line utility to manipulate PS4 packages - v1.02\n\n");

    if (argc == 1) { // No args
        show_tool_help:

        printf("%s [options] ps4_package.pkg\n", argv[0]);
        printf("  --info                              Display as much info as possible about the PKG.\n");
        printf("  --patch-content-id  NEW_CONTENT_ID  Patches the current content ID of the PKG to the new one specified.\n");
        printf("  --patch-title-id    NEW_TITLE_ID    Patches the current title ID of the PKG to the new one specified.\n\n");

        printf("examples:\n");
        printf("   %s --info GoW_Ragnarok_v100.pkg\n", argv[0]);
        printf("   %s --patch-content-id EP4497-CUSA16579_00-0000000000000001 GoW_Ragnarok_v100.pkg\n", argv[0]);

        args->exit = true;
        args->exitcode = 0;
        return args;
    }

    // Loop through argv array
    for (int arg_index = 1; arg_index < argc; arg_index++) {
        if (strcmp(argv[arg_index], "--info") == 0) {
            if (args->flag_info) { // Check if --info was already used
                if (args->pkg != NULL)
                    free(args->pkg);

                goto show_tool_help;
            }

            args->flag_info = true;
        }

        else if (strcmp(argv[arg_index], "--patch-content-id") == 0) {
            if (args->flag_patch_content_id) { // Check if --patch-content-id was already used
                if (args->pkg != NULL)
                    free(args->pkg);

                goto show_tool_help;
            }

            if (arg_index == argc - 1) { // Check if after --patch-content-id actually comes an argument
                if (args->pkg != NULL)
                    free(args->pkg);

                goto show_tool_help;
            }

            if (strlen(argv[arg_index + 1]) != 0x24) { // Check if the content ID is 36 bytes long (0x24)
                fprintf(stderr, "The content ID must be 36 bytes long!\n");
                if (args->pkg != NULL)
                    free(args->pkg);

                args->exit = true;
                args->exitcode = 1;

                return args;
            }

            args->flag_patch_content_id = true;
            strcpy(args->new_content_id, argv[arg_index + 1]);
        }

        else if (strcmp(argv[arg_index], "--patch-title-id") == 0) {
            if (args->flag_patch_title_id) { // Check if --patch-title-id was already used
                if (args->pkg != NULL)
                    free(args->pkg);

                goto show_tool_help;
            }

            if (arg_index == argc - 1) { // Check if after --patch-title-id actually comes an argument
                if (args->pkg != NULL)
                    free(args->pkg);

                goto show_tool_help;
            }

            if (strlen(argv[arg_index + 1]) != 9) { // Check if the title ID is 9 bytes long
                fprintf(stderr, "The title ID must be 9 bytes long!\n");
                if (args->pkg != NULL)
                    free(args->pkg);

                args->exit = true;
                args->exitcode = 1;

                return args;
            }

            args->flag_patch_title_id = true;
            strcpy(args->new_title_id, argv[arg_index + 1]);
        }

        else {
            if (strcmp(argv[arg_index - 1], "--patch-content-id") == 0) // Prevent the new content id to be interpreted as the PKG
                continue;

            if (strcmp(argv[arg_index - 1], "--patch-title-id") == 0) // Prevent the new title id to be interpreted as the PKG
                continue;


            if (args->pkg != NULL) { // args->pkg was already set
                free(args->pkg);
                goto show_tool_help;
            }

            args->pkg = (char*)malloc(strlen(argv[arg_index]) + 1); // Added 1 byte just to be safe
            if (args->pkg == NULL) {
                fprintf(stderr, "Failed to allocate memory on the heap for char pointer (??? malloc() returned NULL)\n");
                
                free(args);
                return NULL;
            }
            
            strcpy(args->pkg, argv[arg_index]);
        }
    }
    
    return args;
}
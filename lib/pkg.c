#include "pkg.h"

// After calling, free return value manually
unsigned char* get_title_id_from_content_id(unsigned char* content_id) {
    unsigned char* title_id = (unsigned char*)malloc(11); // 11 Bytes -> C U S A 0 0 0 0 0 \0
    
    for (int content_id_index = 0; content_id_index < strlen(content_id); content_id_index++) {
        if (
            content_id[content_id_index] == 'C' &&
            content_id[content_id_index + 1] == 'U' &&
            content_id[content_id_index + 2] == 'S' &&
            content_id[content_id_index + 3] == 'A'
        ) {
            int title_id_index = 0;

            for (; content_id_index < strlen(content_id); content_id_index++) {
                if (content_id[content_id_index] == '_')
                    break;

                title_id[title_id_index++] = content_id[content_id_index];
            }
            title_id[title_id_index] = '\0';

            return title_id;
        }
    }

    // Alternative not so reliable way

    for (int content_id_index = 0; content_id_index < strlen(content_id); content_id_index++) {
        if (content_id_index == 7) {
            int title_id_index = 0;

            for (; content_id_index < strlen(content_id); content_id_index++) {
                if (content_id_index == 16)
                    break;

                title_id[title_id_index++] = content_id[content_id_index];
            }
            title_id[title_id_index] = '\0';

            return title_id;
        }
    }

    return NULL; // Failed
}
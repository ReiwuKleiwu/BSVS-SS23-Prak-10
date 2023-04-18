//
// Created by struc on 17.04.2023.
//

#include <stdio.h>
#include "validate_user_input.h"
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>

char* validateFormat(char* req) {
    regex_t regex;
    int requestREGEX = regcomp(&regex, "^PUT:[^\\s]+:[^\\s]+|^GET:[^\\s]+|^DELETE:[^\\s]+", REG_EXTENDED);

    if(requestREGEX) {
        fprintf(stderr, "Could not compile regex\n");
        exit(-1);
    }

    regmatch_t pmatch[1]; // Array to store matched substring information
    int requestREGEXTest = regexec(&regex, req, 1, pmatch, 0);

    char* extracted_substring = NULL;

    if(!requestREGEXTest) {
        // Allocate memory and copy the matched substring
        int length = pmatch[0].rm_eo - pmatch[0].rm_so;
        extracted_substring = (char*) malloc((length + 1) * sizeof(char));
        strncpy(extracted_substring, req + pmatch[0].rm_so, length);
        extracted_substring[length] = '\0';
    }

    regfree(&regex);
    return extracted_substring;
}


void removeWhitespaceChars(char* req) {
    char *src, *dst;
    for (src = dst = req; *src != '\0'; src++) {
        if (!isspace(*src)) {
            *dst++ = *src;
        }
    }
    *dst = '\0';
}

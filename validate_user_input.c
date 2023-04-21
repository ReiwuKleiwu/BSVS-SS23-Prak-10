//
// Created by struc on 17.04.2023.
//

#include <stdio.h>
#include "validate_user_input.h"
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>

void validateFormat(char* req, char* res) {

    regex_t regex;
    int requestREGEX = regcomp(&regex, "^PUT:[^\\s]+:.+|^GET:[^\\s]+|^DELETE:[^\\s]+|^QUIT", 1);

    if(requestREGEX) {
        fprintf(stderr, "Could not compile regex\n");
        exit(-1);
    }

    int requestREGEXTest = regexec(&regex, req, 0, NULL, 0);

    if(requestREGEXTest) {
        strcpy(res, "Der Befehl muss mit PUT:, GET: oder DELETE: beginnen und das richtige Format haben. \r\n");
    }

    regfree(&regex);
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

bool isControlChar(char c) {
    return c >= 0 && c <= 31;
}

void removeControlChars(char *str) {
    int i = 0, j = 0;

    while (str[i]) {
        if (!isControlChar(str[i])) {
            str[j++] = str[i];
        }
        i++;
    }
    str[j] = '\0';
}

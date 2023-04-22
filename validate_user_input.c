#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include "validate_user_input.h"

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

void sanitizeUserInput(char *str) {
    int i = 0, j = 0;

    while(str[i]) {
        if(!(str[i] == '\n' || str[i] == '\r')) {
            str[j++] = str[i];
        }
        i++;
    }

    str[j] = '\0';
}

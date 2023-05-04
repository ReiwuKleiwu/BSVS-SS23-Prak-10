#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include "validate_user_input.h"

void validateFormat(Request client_request) {

    regex_t regex;
    int requestREGEX = regcomp(&regex, "^PUT:[^\\s]+:.+|^GET:[^\\s]+|^DELETE:[^\\s]+|^QUIT|^SUB:[^\\s]+|^UNSUB:[^\\s]+", 1);

    if(requestREGEX) {
        fprintf(stderr, "Could not compile regex\n");
        exit(-1);
    }

    int requestREGEXTest = regexec(&regex, client_request.body, 0, NULL, 0);

    if(requestREGEXTest) {
        strcpy(client_request.response, "The command must start with PUT:, GET:, DELETE:, QUIT, SUB: or UNSUB: and be in the correct format. \r\n");
    }

    regfree(&regex);
    send_response(client_request);
}


void removeWhitespaceChars(char* str) {
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
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

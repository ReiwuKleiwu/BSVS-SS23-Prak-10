#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include "validate_user_input.h"

bool isValidateFormat(Request client_request) {
    regex_t regex;
    int requestREGEX = regcomp(&regex, "^(((GET|DELETE|SUB|UNSUB):(\\S+))|((PUT):(\\S+):(\\w|\\d|\\s)+)|(QUIT|BEG|END))$", 1);

    if(requestREGEX) {
        fprintf(stderr, "Could not compile regex\n");
        exit(-1);
    }

    int requestREGEXTest = regexec(&regex, client_request.body, 0, NULL, 0);

    if(requestREGEXTest) {
        strcpy(client_request.response, "The command must start with PUT:, GET:, DELETE:, QUIT, BEG, END, SUB: or UNSUB: and be in the correct format. \r\n");
        send_response(client_request);
        regfree(&regex);
        return false;
    }

    regfree(&regex);
    return true;
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

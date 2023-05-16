#include <stdbool.h>
#include "request.h"

#ifndef BSVS_SS23_PRAK_VALIDATE_USER_INPUT_H
#define BSVS_SS23_PRAK_VALIDATE_USER_INPUT_H

bool isValidateFormat(Request client_request);
void removeWhitespaceChars(char* str);
void sanitizeUserInput(char* str);

#endif //BSVS_SS23_PRAK_VALIDATE_USER_INPUT_H

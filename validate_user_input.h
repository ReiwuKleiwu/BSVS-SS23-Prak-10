//
// Created by struc on 17.04.2023.
//

#include <stdbool.h>

#ifndef BSVS_SS23_PRAK_VALIDATE_USER_INPUT_H
#define BSVS_SS23_PRAK_VALIDATE_USER_INPUT_H

void validateFormat(char* req, char* res);
void removeWhitespaceChars(char* req);
bool isControlChar(char c);
void removeControlChars(char *str);

#endif //BSVS_SS23_PRAK_VALIDATE_USER_INPUT_H

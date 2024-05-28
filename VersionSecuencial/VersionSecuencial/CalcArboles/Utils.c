//
// Created by Fernando Cores Prado on 12/4/24.
//

#include "Utils.h"

#include <stdio.h>
#include <stdlib.h>

void Error(char *msg)
{
    perror(msg);
    exit(-1);
}
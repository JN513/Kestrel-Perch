#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "config.h"
#include "perch.h"

int main(void)
{
    perch_init();

    while (1)
    {
        perch_task();
    }

    return 0;
}
/*
 * main.c
 * wordlesolve
 *
 * Created by Jeremy Rand on 2022-07-21.
 * Copyright (c) 2022 ___ORGANIZATIONNAME___. All rights reserved.
 *
 */



#pragma cda "wordlesolve" start shutdown


#include <stdio.h>


char str[256];

void start(void)
{
    printf("Hello, world!\n");
    printf("\n\n   Press ENTER to quit...");
    
    fgets(str, sizeof(str), stdin);
}


void shutdown(void)
{
}


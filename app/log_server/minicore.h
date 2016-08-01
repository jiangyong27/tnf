#include <unistd.h>
#include <execinfo.h>
#include <fcntl.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

static const uint32_t MAX_STACK_BACKSTACE_SIZE = 100;
static void *stack_array[MAX_STACK_BACKSTACE_SIZE];
static char minicore_file_name[512] = {0};


void HandleMiniCore(int32_t signal_num)
{
    size_t size = backtrace(stack_array, MAX_STACK_BACKSTACE_SIZE);

    int fd = open(minicore_file_name, O_CREAT | O_WRONLY, S_IRUSR | S_IRGRP | S_IROTH);
    backtrace_symbols_fd(stack_array, size, fd);
    close(fd);

    _exit(EXIT_FAILURE);
}

int32_t InitMiniCore()
{
    snprintf(minicore_file_name, sizeof(minicore_file_name), "rbu_minicore.%u", getpid());
    memset(stack_array, 0, sizeof(stack_array));

    signal(SIGFPE, HandleMiniCore);
    signal(SIGBUS, HandleMiniCore);
    signal(SIGSEGV, HandleMiniCore);
    return 0;
}

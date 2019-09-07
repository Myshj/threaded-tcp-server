//
// Created by Alexey on 07.09.2019.
//

#ifndef THREADED_TCP_SERVER_LOGGING_H
#define THREADED_TCP_SERVER_LOGGING_H

#include <stdio.h>

#ifdef DEBUG
#define debug(template, ...) printf(template, ##__VA_ARGS__);
#else
#define debug(template, ...)
#endif

#endif //SOCKET_ARRAY_V1_POC_LOGGING_H

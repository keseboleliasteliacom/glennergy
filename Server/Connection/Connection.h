#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>
#include "../../Libs/Utils/smw.h"

typedef struct{
    smw_task* task;
    int socket;
    int connected;
}Connection;


int Connection_Initialize(Connection** _Connection, int _Socket);


void Connection_Dispose(Connection** _Connection);

#endif
#ifndef PIPE_SERVER
#define PIPE_SERVER

#include <windows.h>

class NamePipeServ {
private:
    HANDLE pipe;
    const size_t BUF_SIZE = 1024;
public:
    NamePipeServ();
    ~NamePipeServ();
    char readPipe();
};

#endif
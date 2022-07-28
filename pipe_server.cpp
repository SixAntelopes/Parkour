#include "pipe_server.h"

#include <iostream>
using namespace std;

NamePipeServ::NamePipeServ() {
    pipe = CreateNamedPipe(
        TEXT("\\\\.\\Pipe\\ReadPipe"),
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_BYTE,
        1,
        BUF_SIZE,
        BUF_SIZE,
        0,
        NULL );
    if (pipe == INVALID_HANDLE_VALUE) {
        cout << "Create Read Pipe Error" << endl;
        pipe = NULL;
    }
    // cout << "Connectting to name pipe, please open the customer: ....";
    // ConnectNamedPipe(NamePipeServ::pipe, NULL);
    // cout << "Success!" << endl;
}

NamePipeServ::~NamePipeServ() {
    CloseHandle(pipe);
    cout << "over" << endl;
}

char NamePipeServ::readPipe() {
    char buf[BUF_SIZE] = {'\0'};
    DWORD dwReturn = 0;
    char ctrl_btn = '\0';
    PeekNamedPipe(NamePipeServ::pipe, buf, NamePipeServ::BUF_SIZE, NULL, NULL, NULL);
    if (buf[0] != '\0') {
        ReadFile(NamePipeServ::pipe, buf, NamePipeServ::BUF_SIZE, &dwReturn, NULL);
        ctrl_btn = buf[0];
        buf[0] = '\0';
    }
    // cout << ReadFile(NamePipeServ::pipe, buf, NamePipeServ::BUF_SIZE, &dwReturn, NULL) << endl;
    // ReadFile(NamePipeServ::pipe, buf, NamePipeServ::BUF_SIZE, &dwReturn, NULL);
    // cout << "Fucking 2" << endl;
    // if (ctrl_btn != '\0')
    //     cout << ctrl_btn << endl;
    return ctrl_btn;
}

#include <iostream>
#include "logger.h"
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include<sstream>
#include <sys/stat.h>
using namespace std;
#define MODEL_PATH "data/digit_classify.onnx"
int* PortnameChecker(char *);
int ConfigureSerialPort(int );
std::string readFromSerial(int serial_port_fd);
std::string ReadContiuFromSerial(int serial_fd);
void writeToSerial(int serial_port_fd, const std::string& command);
void readData(string,int);
bool CreateFileDir(char *, bool);
string DataReadContiuFromSerial(int); 

#include "serial_port_handler.h"

#define BAUD_RATE B9600
int* PortnameChecker(char *portname){
if (portname == NULL || !(strstr(portname, "/dev/ttyUSB") || strstr(portname, "/dev/ttyACOM"))){
		LOG(LOGGER_LEVEL_INFO,"enter correct port file name..");
		return 0;
	}
	int fd = open(portname, O_RDWR );
	int *fd_pointer= new int(fd); 
	if (fd == -1) {
		perror("Failed to open the port");
		return nullptr;
	}
    return fd_pointer;
}


int ConfigureSerialPort(int serial_port_fd) {
	    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_port_fd, &tty) != 0) {
        std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    tty.c_cflag &= ~PARENB; // No parity bit
    tty.c_cflag &= ~CSTOPB; // One stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable reading and ignore control lines

    tty.c_lflag &= ~ICANON; // Disable canonical mode
    tty.c_lflag &= ~ECHO;   // Disable echo
    tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT, SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable XON/XOFF flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable special byte handling

    tty.c_oflag &= ~OPOST;  // Prevent special interpretation of output bytes
    tty.c_oflag &= ~ONLCR;  // Prevent conversion of newline to carriage return

    tty.c_cc[VTIME] = 5;   // Wait up to 1 second for a response
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, BAUD_RATE);
    cfsetospeed(&tty, BAUD_RATE);

    if (tcsetattr(serial_port_fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
		
    }
	return 0;

}


void writeToSerial(int serial_port_fd, const std::string &command) {
    write(serial_port_fd, command.c_str(), command.length());
    write(serial_port_fd, "\n", 1); // Send newline to signal end of command
}

string ReadContiuFromSerial(int serial_fd) {

    int offset=0;
    char buffer[64];
    string mesg="";
    do {
	    memset(buffer, '\0', sizeof(buffer));
	    int num_bytes = read(serial_fd, buffer, sizeof(buffer));
	    if (num_bytes < 0) {
		//cerr << "Error reading: " << strerror(errno) << endl;
		return "";
	    }
	    else if(num_bytes ==0)
	    	continue;
	    if(offset ==0)
	   	mesg=string(buffer);
	   else 
	   	mesg=mesg+string(buffer);
	   
	   offset= num_bytes;		
    }
    while(strstr(mesg.c_str(),"End")==NULL);
    offset=0;
    
    return mesg;
}


string readFromSerial(int serial_port_fd) {
    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));
    
    int num_bytes = read(serial_port_fd, buffer, sizeof(buffer));
    
    cout<<"num_bytes " <<num_bytes<<endl;
      
    cout<<"buffer " <<buffer<<endl;
    
    
    
    
    if (num_bytes < 0) {
        cerr << "Error reading: " << strerror(errno) << endl;
        return "";
    }
    
    return string(buffer);

}

void readData(string response, int fd){
        stringstream ss(response);
        string item;
        while(getline(ss ,item,'\n')){
                if(!(strstr(item.c_str(),"start"))&& !(strstr(item.c_str(),"End"))){
                        write(fd,item.c_str(),strlen(item.c_str()));
                }
        }
}



string DataReadContiuFromSerial(int serial_fd) {
    int offset = 0;
    char buffer[64];
    int num_bytes = 0;
    string mesg = "";
 
    do {
        memset(buffer, '\0', sizeof(buffer));
        num_bytes = read(serial_fd, buffer, sizeof(buffer));
        // if (num_bytes==1 && buffer[0] == '\n' )
        //     continue;
        
        if (num_bytes < 0) {
            cerr << "Error reading: " << strerror(errno) << endl;
            return "";
        }
        else if (num_bytes == 0) {
            continue;
        }
 
        
        mesg.append(buffer, num_bytes);
 
        offset += num_bytes;
       // cout << "Buffer size (read bytes): " << num_bytes << endl;
       // cout << "Message size so far: " << mesg.size() << endl;
       // cout << "Offset = " << offset << endl;
 
    } while (mesg.find("End") == string::npos); // Continue until "End" is found in the message
 
   // cout << "Final offset = " << offset << endl;
    //cout << "Final message size = " << mesg.size() << endl;
    return mesg;
}

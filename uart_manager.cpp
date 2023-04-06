/*
 * UartManager.cpp
 *
 *  Created on: 2 mar 2023
 *      Author: lab
 */

#include "uart_manager.hpp"
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;
int UartManager::Configure_uart()
{
	struct termios tty;
	serial_port = open("/dev/ttyACM0", O_RDWR);
	if (serial_port < 0)
	{
		printf("Can't find a measurement device\n");
		return 1;
	}
	if(tcgetattr(serial_port, &tty) != 0)
	{
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		close(serial_port);
		return 1;
	}
		tty.c_cflag &= ~PARENB;//no parity
		tty.c_cflag &= ~CSTOPB;//1 stop bit
		tty.c_cflag &= ~CSIZE;//clear word length register
		tty.c_cflag |= CS8;//8 bits per word
		tty.c_cflag &= ~CRTSCTS;//no hardware flow control
		tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines
		tty.c_lflag &= ~ICANON;//non-canonical mode
		tty.c_lflag &= ~ECHO; // Disable echo
		tty.c_lflag &= ~ECHOE; // Disable erasure
		tty.c_lflag &= ~ECHONL; // Disable new-line echo
		tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
		tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow control
		tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
		tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
		tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
		tty.c_cc[VTIME] = 10;
		tty.c_cc[VMIN] = 0;
		cfsetispeed(&tty, B38400);
		cfsetospeed(&tty, B38400);
		if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
		{
		    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		    close(serial_port);
		    return 1;
		}
		tcflush(serial_port, TCIOFLUSH);

		return 0;
}

char UartManager::Recive_byte()
{

	char byte;
	serial_port = open("/dev/ttyACM0", O_RDWR);
	if(serial_port==-1)
	{
		close(serial_port);
		return -1;
	}
	read(serial_port, &byte, sizeof(byte));

	close(serial_port);
	return byte;
}

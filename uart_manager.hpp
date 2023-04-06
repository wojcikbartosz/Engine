/*
 * UartManager.h
 *
 *  Created on: 2 mar 2023
 *      Author: lab
 */

#ifndef UARTMANAGER_H_
#define UARTMANAGER_H_

class UartManager
{
private:
    int serial_port;

public:
    int Configure_uart();
    char Recive_byte();
};

#endif /* UARTMANAGER_H_ */

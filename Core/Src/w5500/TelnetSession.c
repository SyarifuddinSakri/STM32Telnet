#include "TelnetSession.h"
#include "socket.h"
#include "main.h"
#include <string.h>

#define MAX_BUFFER_SIZE 1024  // Adjust the size according to your needs

uint8_t buffer[MAX_BUFFER_SIZE];  // Assuming buffer is declared globally or passed as an argument
uint8_t telnetSocket = 0;
uint16_t telnetPort = 23;

void writeMessage(const char* message) {
    send(telnetSocket, (void*)message, strlen(message));
}

uint8_t* readMessage() {

    int receivedSize = 0;
    int indexChar = 0;
    uint8_t receivedChar;

    while (1) {
		receivedSize = recv(telnetSocket, &receivedChar, 1);

		// Check for errors or connection closure
		if (receivedSize <= 0) {
			// Handle error or connection closure
			break;
		}
		// Check if the received character is a newline
		if (receivedChar == '\n') {
			// If yes, terminate the string and break out of the loop
			buffer[indexChar] = '\0';
			break;
		}
		// Otherwise, add the character to the buffer
		buffer[indexChar] = receivedChar;
		// Increment the receivedSize counter
		indexChar++;
		// Check if the buffer is full (adjust the size accordingly)
		if (indexChar >= MAX_BUFFER_SIZE - 1) {
			// Handle buffer full condition
			break;
		}
    }
    // Add null terminator to make it a valid C string
    buffer[indexChar] = '\0';
    // Return the buffer (or you might return something else based on your needs)
    return buffer;
}

void toggleLeds(uint8_t* clientMessage) {
    if (clientMessage[0] == 'l'&&clientMessage[1]=='1') {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    } else if (clientMessage[0] == 'l'&&clientMessage[1]=='2') {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
    }
}
void mainMenu(){
	writeMessage("Select your inputs\r\n1. ");
}

void startTelnet() {
    // Initialize the socket outside the loop
    socket(telnetSocket, Sn_MR_TCP, 23, 0);
    listen(telnetSocket);

    while (1) {
        if (getSn_SR(telnetSocket) == SOCK_ESTABLISHED) {
            writeMessage("\033[32mTelnet Configuration Session\033[37m\r\n");
            toggleLeds(readMessage());
        }else{
            // Current socket is not in the established state
            // Close the socket and reinitialize it to listen for new connections
            close(telnetSocket);
            socket(telnetSocket, Sn_MR_TCP, 23, 0);
            listen(telnetSocket);
            // Optionally, add a delay to avoid rapid reconnection attempts
            HAL_Delay(300);
            continue;//continue to the next iteration of loop
        }
        // Add a condition to break out of the loop or handle other logic
        if (getSn_SR(telnetSocket) != SOCK_ESTABLISHED) {
        	break;
        }

    }
}

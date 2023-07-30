/*
 * UART.c
 *
 *  Created on: Dec 31, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#include <HAL/Timer.h>
#include <HAL/UART.h>

/**
 * Initializes the UART module except for the baudrate generation
 * Except for baudrate generation, all other uart configuration should match that of basic_example_UART
 *
 * @param moduleInstance:   The eUSCI module used to construct this UART
 * @param port:             The port the UART is connected to
 * @param pins:             The Rx and Tx pins UART is connected to
 *
 * @return the completed UART object
 */
UART UART_construct(uint32_t moduleInstance, uint32_t port, uint32_t pins)
{
    // Create the UART struct
    UART uart;

    // Initialize auxiliary members of the struct
    // -------------------------------------------------------------------------
    uart.moduleInstance = moduleInstance;
    uart.port = port;
    uart.pins = pins;

    GPIO_setAsPeripheralModuleFunctionInputPin(
            uart.port, uart.pins, GPIO_PRIMARY_MODULE_FUNCTION);


    // TODO: Determine what other variables need to be initialized in the config struct
    // TODO: all uart config parameters except those related to baudrate generation
    uart.config.parity           = EUSCI_A_UART_NO_PARITY;    // No Parity

    // Return the completed UART instance
    return uart;
}

/**
 * (Re)initializes and (re)enable the UART module to use a desired baudrate.
 *
 * @param uart_p        The pointer to the uart struct that needs a baudrate and should be enabled
 * @param baudChoice:   The new baud choice with which to update the module
 */
void UART_SetBaud_Enable(UART *uart_p, UART_Baudrate baudChoice)
{
    // We use the system clock for baudrate generation.
    // The processor clock runs at 48MHz in the project. In other words, system clock is 48MHz.
    uart_p->config.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;


    // Fortunately for us, for all the baudrates we are working with with f = 48MHz, we use oversampling mode N>16
    uart_p->config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;


    // When using the baudChoice to index the array, we are effectively saying
    // that different baudrate choices correspond with different numbers
    // without requiring excessive amounts of if-statements.
    // TODO: fill these tables such that they work with the proper baudChoice. The first column is given for 9600 BPS
    uint32_t clockPrescalerMapping[NUM_BAUD_CHOICES] = { 312, 156, 78, 52};
    uint32_t firstModRegMapping[NUM_BAUD_CHOICES] = { 8, 4, 2, 1};
    uint32_t secondModRegMapping[NUM_BAUD_CHOICES] = { 85, 0, 34, 2};


    //uint32_t clockPrescalerMapping[NUM_BAUD_CHOICES] = { 312, 156, 0, 52};
        //uint32_t firstModRegMapping[NUM_BAUD_CHOICES] = { 8, 9, 0, 0};
        //uint32_t secondModRegMapping[NUM_BAUD_CHOICES] = { 0, 0, 0, 0};

    // TODO: Replace 0s with the correct statement. Use line 71 as your guide
    uart_p->config.clockPrescalar = clockPrescalerMapping[baudChoice];
    uart_p->config.secondModReg = firstModRegMapping[baudChoice];
    uart_p->config.firstModReg = secondModRegMapping[baudChoice];
    uart_p->config.parity = EUSCI_A_UART_NO_PARITY;
    uart_p->config.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    uart_p->config.dataLength = EUSCI_A_UART_8_BIT_LEN;
    uart_p->config.msborLsbFirst = EUSCI_A_UART_MSB_FIRST;
    //uart_p->config.uartMode = EUSCI_A_UART_MODE;


    // TODO: initialize and enable uart instance (refer to the basic_example_UART project for guidance)
    UART_initModule(EUSCI_A1_BASE, &uart_p);
    UART_enableModule(EUSCI_A1_BASE);

}


// Not-a-real TODO: read this function and its comment to learn how to implement the other functions
// Notice how we use uart_p's module instance instead of the name of the instance from the basic example where we used no HAL
/**
 * Determines if the user has sent a UART data packet to the board by checking
 * the interrupt status of the proper system UART module.
 *
 * @param uart_p: The pointer to the UART instance with which to handle our operations.
 *
 * @return true if the user has entered a character, and false otherwise
 */
bool UART_hasChar(UART* uart_p)
{
    uint8_t interruptStatus = UART_getInterruptStatus(
            uart_p->moduleInstance, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    return (interruptStatus == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
}

// TODO: Complete the UART_getChar() function.
// TODO: Write a descriptive comment explaining HOW the function is implemented.
//       In the implementation file, prefer explaining HOW the function is
//       implemented over simply WHAT the function does.
/**
 * A getter method which should just return the character in the uart instance.
 * Uses the UART HAL UART_getChar() method taking the UART instance to detect
 * incoming data and the returns it.
 *
 * @param uart_p:   The uart_p object from which to retrieve the character
 *
 * @return the character retrieved from the UART
 */
char UART_getChar(UART* uart_p)
{
    // TODO: change this
    return UART_receiveData(uart_p->moduleInstance);
  //return '\0';
}

// TODO: Complete the UART_canSend() function.
// TODO: Write a descriptive comment explaining HOW the function is implemented.
//       In the implementation file, prefer explaining HOW the function is
//       implemented over simply WHAT the function does.
/**
 * A simple method which should just return the availability of the UART.
 * Uses the UART HAL UART_queryStatusFlags() method taking the UART instance to detect
 * and return whether the busy flag is thrown.
 *
 * @param uart_p:   The uart_p object from which to retrieve the character
 *
 * @return if the UART can or cannot send
 */
bool UART_canSend(UART* uart_p)
{
    // TODO: change this

    uint8_t uartStatus = UART_queryStatusFlags(
                uart_p->moduleInstance, EUSCI_A_UART_BUSY);

        return (!(uartStatus == EUSCI_A_UART_BUSY));

}

// TODO: Complete the UART_putChar() function.
// TODO: Write a descriptive comment explaining HOW the function is implemented.
//       In the implementation file, prefer explaining HOW the function is
//       implemented over simply WHAT the function does.
/**
 * A simple method which should send the passed character to the UART.
 * Uses the UART HAL UART_transmitData() method taking the UART instance
 * send the character parameter to the instance.
 *
 * @param uart_p:   The uart_p object from which to retrieve the character
 * @param c:   The character sent to the uart_p object
 */
void UART_putChar(UART* uart_p, char c)
{
  // TODO: add some lines here
    UART_transmitData(uart_p->moduleInstance, c);

    return;
}


1. Example Description
    This application demonstrates the usage of a subset of Real Time Drivers that are included in this code drop. It is not part of the production code deliverables.
    The application includes an EB Tresos project that configures the Resource Base Mcu EcuC Os Port Uart Platform.
    1.1 The aplication software functionality
            - Siul2_Port_Ip_Init
                    Initialize the pins used by the application using the Port driver.
            - Clock_Ip_Init
                    Initialize the clock sources for project.
            - IntCtrl_Ip_Init, IntCtrl_Ip_EnableIrq, IntCtrl_Ip_InstallHandler
                    Initializes the interrupts for Lpuart driver
            - Lpuart_Uart_Ip_Init
                    Initialize the Uart driver
            - Lpuart_Uart_Ip_AsyncSend
                    Send Uart data
            - Lpuart_Uart_Ip_AsyncReceive
                    Receive Uart data
            - Lpuart_Uart_Ip_GetStatus
                    Get status of Uart
    1.2 The aplication description
			- The welcome message is sent via UART: "This example is an simple echo using UART
													 The board will greet you if you send 'Hello Board'
													 Now you can begin typing:"
			- User shall send "Hello Board" string. If the board receives the user's string, then the greeting string shall be sent to user.	
2. Installation steps
    2.1 Hardware installation
        2.1.1 Hardware preparation
            - XS32KXXCVB-Q257 (Daughtercard)
            - X-S32K2XX-MB (Motherboard)
            - 1 Power Adapters 12V (only for validation board, EVB is powered through OpenSDA USB connection)
            - 1 microUSB cable
            - 1 Personal Computer
            - 1 S32Debugger: Lauterbach
        2.1.2 Connections
            - It uses Lpuart instance 6, which is internally connected to the serial-to-USB module on the board.  Make sure that positions of 2 jumpers on the board are corrected: Set jump J318 to 1-2 position and Set jump J319 to 1-2 position.
            - A terminal interface (termite/putty etc) needs to be opened on the host computer and connected to the USB port communicating with the board.
        2.1.3 Debugger
    2.2 Software installation
        2.2.1 Importing the S32 Design Studio project
            After opening S32 Design Studio, go to "File -> New -> S32DS Project From Example" and select this example. Then click on "Finish".
            The project should now be copied into you current workspace.
       
    2.2 Software installation
   
3. Generating, building and running the example application
    3.1 Generating the S32 configuration
        Before running the example a configuration needs to be generated.  First go to Project Explorer View in S32 DS and select the current project. Select the "S32 Configuration Tool" menu then click on the desired configuration tool (Pins, Cocks, Peripherals etc...). Clicking on any one of those will generate all the components. Make the desired changes(if any) then click on the "S32 Configuration Tool->Update Code" button.
    3.2 Compiling the application
        Select the configuration to be built: RAM (Debug_RAM), or FLASH (Debug_FLASH) by left clicking on the downward arrow corresponding to the build button in eclipse. 
        Use Project > Build to build the project.
        Wait for the build action to be completed before continuing to the next step. Check the compiler console for error messages; upon completion, the *.elf binary file
        should be created.
    3.2 Running the application on the board
        Go to Run and select Debug Configurations. There will be a debug configuration for this project:
        
        Configuration Name                  Description
        -------------------------------     -----------------------
        $Lpuart_Uart_Ip_example_UCT_debug_ram_pemicro        Debug the RAM configuration using PEmicro probe
        $Lpuart_Uart_Ip_example_UCT_debug_flash_pemicro      Debug the FLASH configuration using PEmicro probe
        
        Select the desired debug configuration and click on Launch. Now the perspective will change to the Debug Perspective.
        Use the controls to control the program flow.
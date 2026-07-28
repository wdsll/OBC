1、功能说明

    /* 简单描述工程功能 */
        这个例程配置并演示CAN在环回测试模式下收发CAN报文情况


2、使用环境

        软件开发环境：KEIL MDK-ARM V5.34.0.0 
                      IAR EWARM 8.50.1
    
        硬件环境：基于评估板N32G430C8L7-STB V1.0开发


3、使用说明
    
    /* 描述相关模块配置方法；例如:时钟，I/O等 */
        SystemClock：128MHz
        CAN: RX-PB8, TX-PB9，波特率500K，环回测试模式

    /* 描述Demo的测试步骤和现象 */
        1.编译后下载程序复位运行；
        2.CAN发送一帧消息，然后在验证通过后再发送另一帧消息。通过CAN设备，可以看到消息是在循环发送。


4、注意事项


1. Function description

    /* Briefly describe the engineering function */
    This example configures and demonstrates CAN sending and receiving CAN messages in loopback test mode


2. Use environment

    Software development environment: KEIL MDK-ARM V5.34.0.0
    Hardware environment: Developed based on the evaluation board N32G430C8L7-STB V1.0

3. Instructions for use
    
    /* Describe the configuration method of related modules; for example: clock, I/O, etc. */
    SystemClock: 128MHz
    CAN: RX-PB8, TX-PB9, baud rate 500K, loopback test mode

    /* Describe the test steps and phenomena of Demo */
    1. After compiling, download the program to reset and run;
    2. CAN sends a frame of message, and then sends another frame of message after the verification is passed. Through the CAN device, you can see that the message is sent in a loop.


4. Matters needing attention

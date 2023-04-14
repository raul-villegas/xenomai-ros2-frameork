#include <stdio.h>
#include <unistd.h>
#include <iterator>
#include <signal.h>
#include <vector>
#include <sys/syscall.h>

#include "framework/multiCommClass.h"
#include "framework/runnableClass.h"
#include "framework/superThread.h"
#include "framework/icoCommClass.h"
#include "model/Controller.h"
// #include <Your 20-sim-code-generated h-file?> Don't forget to compile the cpp file by adding it to CMakeLists.txt

volatile bool exitbool = false;

void exit_handler(int s)
{
    printf("Caught signal %d\n", s);
    exitbool = true;
}



int main()
{
    //CREATE CNTRL-C HANDLER
    signal(SIGINT, exit_handler);

    printf("Press Ctrl-C to stop program\n"); // Note: this will 
        // not kill the program; just jump out of the wait loop. Hence,
        // you can still do proper clean-up. You are free to alter the
        // way of determining when to stop (e.g., run for a fixed time).


   
    

    int sendParameters[8] = {0, -1, 1, -1, 2, -1, 3, -1};
    int receiveParameters[12] = {0, -1, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1};

    IcoComm *IcoComm = new IcoComm(sendParameters, receiveParameters);
    IcoComm->setReadConvertFcn()
    // IDDPComm *iddpComm = new IDDPComm(NULL, NULL, NULL, receiveParameters);
    //XSSPComm *
    frameworkComm *controller_uPorts[] = {
        IcoComm};
    
    frameworkComm *controller_yPorts[] = {
        IcoComm};
    
    Controller *controller_class = new Controller;
    runnable *controller_runnable = new wrapper<Controller>(
        controller_class, controller_uPorts, controller_yPorts, 1, 1
    );
    controller_runnable->setSize(12,8);

    xenoThread controllerThread(controller_runnable);
    controllerThread.init(1000000, 80 , 1);
    controllerThread.start("Controller");



    // WAIT FOR CNTRL-C
    timespec t = {.tv_sec=0, .tv_nsec=100000000}; // 1/10 second

    while (!exitbool)
    {
        // Let the threads do the real work
        nanosleep(&t, NULL);
        // Wait for Ctrl-C to exit
    }
    printf("Ctrl-C was pressed: Stopping gracefully...\n");
    controllerThread.stopThread();
    
    //CLEANUP HERE


    return 0;
}

#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>       
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/ipc.h>                    
#include <sys/shm.h> 



#define SHM_KEY 0x1234
struct SharedData {
    bool isChildDone;
    int multiple;
    int counter;
};

int main() { 
    pid_t pid = fork(); 
    int counter1; 

    int shmid = shmget(SHM_KEY, sizeof(struct SharedData), 0666 | IPC_CREAT);  
    struct SharedData *data = (struct SharedData*) shmat(shmid, NULL, 0);      
    if (data == (void*)-1) { perror("shmat failed"); return 1; }     
    
    data -> isChildDone = false;
    data->multiple = 3;                   
    data->counter  = 0;                  
     
    //Child Process
    if(pid == 0) { 
        execl("./process2", "process2", NULL);   
        perror("Exec failed");  
        exit(1);
    } 
    
    //Parent Process
    else if(pid > 0) { 
        counter1 = 0; 
        while(data -> counter <= 500 && !data -> isChildDone) {
            if (data->counter % data->multiple == 0)                                  
                printf("Cycle number: %d – %d is a multiple of %d (Parent %d)\n",    
                counter1, data->counter, data->multiple, getpid());            
            else                                                                       
                printf("Cycle number: %d\n", counter1);   

            data->counter++;          
            counter1++; 
            //if (data->counter > 500)  
                //break;    
            sleep(1);
        }

        waitpid(pid, NULL, 0);
        shmdt(data);                    
        shmctl(shmid, IPC_RMID, NULL);  
    } 
    
    else { 
        printf("Unable to create proccess"); 
    } 
    
    return 0; 
}
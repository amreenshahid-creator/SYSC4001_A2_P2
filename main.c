#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>                     
#include <sys/ipc.h>                    
#include <sys/shm.h> 

#define SHM_KEY 0x1234
struct SharedData {
    int multiple;
    int counter;
};

int main() { 
    pid_t pid = fork(); 
    int counter1; int counter2; 

    int shmid = shmget(SHM_KEY, sizeof(struct SharedData), 0666 | IPC_CREAT);  
    struct SharedData *data = (struct SharedData*) shmat(shmid, NULL, 0);      
    if (data == (void*)-1) { perror("shmat failed"); return 1; }               
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
        counter2 = 0; 
        while(1) {
            if (data->counter % data->multiple == 0)                                  
                printf("Cycle number: %d – %d is a multiple of %d (Parent %d)\n",    
                counter2, data->counter, data->multiple, getpid());            
            else                                                                       
                printf("Cycle number: %d\n", counter2);   

            data->counter++;          
            counter2++; 
            if (data->counter > 500)  
                break;    
            sleep(2);
        }

        shmdt(data);                    
        shmctl(shmid, IPC_RMID, NULL);  
    } 
    
    else { 
        printf("Unable to create proccess"); 
    } 
    
    return 0; 
}

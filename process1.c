#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>       
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/ipc.h>                    
#include <sys/shm.h> 
#include <sys/sem.h>


#define SHM_KEY 0x1234
#define SEM_KEY 0x4321

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

    if (data == (void*)-1) { 
        perror("shmat failed"); 
        return 1; 
    }  

    int sem_id = semget(SEM_KEY, 1, 0666 | IPC_CREAT); //creates one semaphore at 0x4321

    if(sem_id == -1) {
        perror("semget failed");
        return 1;
    }

    semctl(sem_id, 0, SETVAL, 1); //initialize semaphore to 1

    struct sembuf wait = { //decrements semaphore
        .sem_num = 0,
        .sem_op = -1,
        .sem_flg = 0
    };

    struct sembuf signal = { //increments semaphore
        .sem_num = 0,
        .sem_op = 1,
        .sem_flg = 0
    };
 
    data->isChildDone = false;
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
            semop(sem_id, &wait, 1); //Protects shared data so child process cant moddify it 

            if (data->counter % data->multiple == 0)                                  
                printf("Cycle number: %d – %d is a multiple of %d (Parent %d)\n",    
                counter1, data->counter, data->multiple, getpid());            
            else                                                                       
                printf("Cycle number: %d\n", counter1);   

            data->counter++;          
            counter1++;

            semop(sem_id, &signal, 1); //Child process now has access to shared data

            if (data->counter > 500){
                break;  
            }   

            sleep(1); 
        }

        waitpid(pid, NULL, 0);
        shmdt(data);                    
        shmctl(shmid, IPC_RMID, NULL);  
        semctl(sem_id, 0, IPC_RMID); //deletes semaphore 

    } 
    
    else { 
        printf("Unable to create proccess"); 
    } 
    
    return 0; 
}
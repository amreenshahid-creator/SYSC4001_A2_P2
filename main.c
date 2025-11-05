#include <stdio.h> 
#include <unistd.h> 

int main() { 
    pid_t pid = fork(); 
    int counter1; int counter2; 
     
    //Child Process
    if(pid == 0) { 
        counter1 = 0; 
        while(1) { 
            counter1++; 
            printf("Child %d: %d\n", getpid(), counter1); 
            sleep(2);
        } 
    } 
    
    //Parent Process
    else if(pid > 0) { 
        counter2 = 0; 
        while(1) {
            counter2++; 
            printf("Parent %d: %d\n", getpid(), counter2); 
            sleep(2);
        }
    } 
    
    else { 
        printf("Unable to create proccess"); 
    } 
    
    return 0; 
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    if(argc >= 2){
        printf("usage: pingpong\n");
        exit(0);
    }

    int pid, p_pipe[2], c_pipe[2];       
    
    pipe(p_pipe);
    pipe(c_pipe);
    char buffer[2];
    pid = fork();
    if(pid == 0)
    {
        if(read(p_pipe[0], buffer, 1) != 0){
            printf("Received from parent: %s\n", buffer);
            printf("%d: received ping\n", getpid());
        }
        
        write(c_pipe[1], "c", 1);
        close(c_pipe[1]);
        close(p_pipe[0]);      
    }
    else
    {   
        write(p_pipe[1], "p", 1);
        if(read(c_pipe[0], buffer, 1) == 1)
        {
            printf("Received from child: %s\n", buffer);
            printf("%d: received pong\n", getpid());
        }
        close(p_pipe[1]);
        close(c_pipe[0]);
        
    }
    exit(0);
}
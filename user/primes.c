#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int * m_pipe)
{
    //printf("the child Id is Running: %d\n", getpid());
    int first;
    int buffer[20];
    close(m_pipe[1]);
    if(read(m_pipe[0], &first, 4))
    {
        printf("prime %d\n", first);
        int num, p = 0;
        while(read(m_pipe[0], buffer + p, 4) != 0){
            ++p;
        }
        close(m_pipe[0]);

        pipe(m_pipe);
        if(fork() == 0){
            prime(m_pipe);
        }
        else
        {
            close(m_pipe[0]);
            for(int i = 0; i < p; ++i)
            {
                num = *(buffer + i);
                if(num % first){
                    write(m_pipe[1], &num, 4);
                }
            }
            close(m_pipe[1]);
        }
    }
    
}

int main(int argc, char*argv[])
{
    if(argc > 1){
        fprintf(2, "usage: prime\n");
        exit(0);
    }

    int status;
    int m_pipe[2];
    pipe(m_pipe);
    printf("prime %d\n", 2);
    if(fork() == 0)
    {
        prime(m_pipe);
        printf("Parent [%d], child [%d] terminate\n", getpid(), wait(&status));
    }   
    else
    {
        close(m_pipe[0]);
        for(int i = 3; i <= 35; ++i)
        {
            if(i % 2){
                write(m_pipe[1], &i, 4);
            }
        }
        write(m_pipe[1], 0, 0);
        close(m_pipe[1]);
        printf("Parent [%d], child [%d] terminate\n", getpid(), wait(&status));
    }
    exit(0);
}
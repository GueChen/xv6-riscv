#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"



int main(int argc, char* argv[])
{
    if(argc < 2){
        printf("usage : src_cmd [argument] | xargs target_cmd [argument]\n");
        exit(0);
    }
    char *cmd =argv[1]; 
    char m_argbuff[MAXARG][100];
    char *m_args[MAXARG];
    memset(m_argbuff, 0, sizeof m_argbuff);
    memset(m_args,    0, sizeof m_args);

    for(int i = 0; i < argc - 1; ++i){
        strcpy(m_argbuff[i], argv[i + 1]);
        m_args[i] = m_argbuff[i];
    }

    int  count  = argc - 1, cursor = 0;
    char buff;
    while(read(0, &buff, 1)){
        if(buff == '\n')
        {
            m_argbuff[count][cursor] = 0;
            m_args[count] = m_argbuff[count];
            if(fork() == 0){
                exec(cmd, m_args);
                exit(0);
            }
            else{
                wait(0);
            }

            for(int i = argc - 1; i < count + 1; ++i){
                m_args[i] = 0;
            }
            count  = argc - 1;
            cursor = 0;
        }
        else if(buff == ' ')
        {
            m_argbuff[count][cursor] = 0;
            m_args[count] = m_argbuff[count];
            ++count;
            cursor = 0;
        }
        else
        {
            m_argbuff[count][cursor++] = buff;
        }
    }

    exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>//non-blocking
#include <sys/mman.h>
#include <sys/stat.h>
#include "api.h"


mailbox_t mailbox_open(int id)
{
    /*
    mailbox_open() creates and opens a new, or opens an existing mailbox object
    Returns NULL on failure
    id specifies the name of the shared memory object
    Ex: mailbox_open(12) will open the mailbox object: “/dev/shm/__mailbox_12” by calling shm_open()
    Server’s mailbox id fixed to 0*/
    char mailbox_str1[100] = "__mailbox_";
    char mailbox_str2[100] = {'\0'};
    sprintf(mailbox_str2,"%d",id);
    strncat(mailbox_str1, mailbox_str2, 12);
    int fd = shm_open(mailbox_str1,O_RDWR | O_CREAT,0666);
    if(fd < 0)
    {
        printf("open %s failed\n",mailbox_str1);
        exit(1);
    }
    printf("mailbox_open:open %s successed\n",mailbox_str1);
    // printf("file descriptor = %d\n",fd);
    ftruncate(fd,409600);//sizeof(char)*2 + sizeof(mail_t)*10);
    mailbox_t mailbox_address =  mmap(NULL,409600, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);
    if(mailbox_address == MAP_FAILED)
    {
        printf("memory map failed\n");
        fprintf(stderr,"Error number:%d\n",errno);
        return NULL;
    }
    char buf[3]= "0";
    memcpy(mailbox_address, buf, sizeof(char)*3);
    // fcntl(fd, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    return mailbox_address;
}
int mailbox_unlink(int id)
{
    char mailbox_str1[100] = "__mailbox_";
    char mailbox_str2[100] = {'\0'};
    sprintf(mailbox_str2,"%d",id);
    strncat(mailbox_str1, mailbox_str2, 12);
    int return_value;
    if((return_value=shm_unlink(mailbox_str1))==-1)
    {
        printf("unlink mailbox %d Error\n",id);
    }
    return return_value;
}
int mailbox_close(mailbox_t box)
{
    return munmap(box, 409600);
}
int mailbox_send(mailbox_t box, mail_t* mail)
{
    fcntl(0, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    fcntl(1, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    // printf("mailbox_send process start...\n");
    if(mailbox_check_full(box)!=0)
    {
        char buf[409600] = {'\0'};
        memcpy(buf, box , 409600);
        // buf[409599]='\0';
        int num;
        sscanf(buf,"%d",&num);
        num++;
        // char new_buf[409600] = {'\0'};
        sprintf(buf,"%d\t",num);
        if(num>=100)
        {
            printf("ERROR:num is too big!\n");
            exit(1);
        }
        // printf("strlen(buf)=%d\n",strlen(buf));
        // printf("strlen(buf)=%d",strlen(buf));
        // int var;
        // for(var = 0; var < 50; var++)
        // {
        //     printf("buf[%d]= %c\n",var,buf[var]);
        //     if(buf[var]=='\0')
        //     {
        //         printf("^here!\n");
        //     }
        // }
        buf[2]='\t';
        // buf[2]='\0';
        if(num==1)
        {
            sprintf(buf + 3,"%d\t%d\t%d\t%s\n%s\n\0",(*mail).from,(*mail).to,(*mail).type,(*mail).sstr,(*mail).lstr);
        }
        else
        {
            // int strlen(buf);
            // buf[strlen(buf)]='\t';
            // printf("strlen(buf)=%d\n",strlen(buf));
            sprintf(buf + strlen(buf),"%d\t%d\t%d\t%s\n%s\n\0",(*mail).from,(*mail).to,(*mail).type,(*mail).sstr,(*mail).lstr);
        }
        // printf("newbuf\n");
        // for(var = 0; var < 100; var++)
        // {
        //     printf("afterbuf[%d]= %c\n",var,buf[var]);
        //     if(buf[var]=='\0')
        //     {
        //         printf("^here!\n");
        //     }
        // }
        memcpy(box, buf , 409600);
        // printf("...mailbox_send process completed:successful\n\n");
        return 0;
    }
    // printf("...mailbox_send process completed:failed\n\n");
    return -1;
    // // On success, return 0; on failure, return -1
    // return 0;
}
int mailbox_recv(mailbox_t box, mail_t* mail)//mail=&mailbox
{
    // printf("mailbox_recv process start...\n");
    fcntl(0, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    fcntl(1, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    if(mailbox_check_empty(box)!=0)
    {
        // printf("mailbox_recv process start...\n");
        char buf[409600] = {'\0'};
        memcpy(buf, box , 409600);
        buf[409599]='\0';
        // int var;
        // for(var = 0; var < 100; var++)
        // {
        //     printf("buf_number[%d]= %d\n",var,buf[var]);
        //     printf("buf[%d]= %c\n",var,buf[var]);
        // }
        int num;
        int init_length = strlen(buf);//initial data length
        sscanf(buf,"%d%*c%d%*c%d%*c%d%*c%[^\n]%*c%[^\n]%*c",&num,&(*mail).from,&(*mail).to,&(*mail).type,(*mail).sstr,(*mail).lstr);
        // printf("mailbox recv info:\n");
        // printf("-------------------------------------------------------\n");
        // printf("num=%d\nfrom=%d\nto=%d\ntype=%d\nsstr=%s\nlstr=%s\n",num,(*mail).from,(*mail).to,(*mail).type,(*mail).sstr,(*mail).lstr);//,((*mail).from),((*mail).type));//,((*mail).sstr),((*mail).lstr));
        // printf("-------------------------------------------------------\n");
        num--;
        int count;//for count
        int after_length;
        for(after_length = 0, count = 0; after_length < init_length; after_length++)
        {
            if(buf[after_length]=='\n')
            {
                count++;
                if(count==2)
                {
                    break;
                }
            }
        }
        // printf("after_length=%d\n",after_length);
        char num_buf[409600] = {'\0'};
        sprintf(num_buf,"%d",num);
        num_buf[2]='\t';
        memcpy(num_buf+3, buf+after_length+1, 409600-after_length);
        // for(var = 0; var < 100; var++)
        // {
        //     printf("num_buf_number[%d]= %d\n",var,num_buf[var]);
        //     printf("num_buf[%d]= %c\n",var,num_buf[var]);
        // }
        memcpy(box, num_buf, 409600);
        // printf("...mailbox_recv process completed:successful\n\n");
        return 0;
    }
    // printf("...mailbox_recv process completed:failed\n\n");
    return -1;
}
int mailbox_check_empty(mailbox_t box)
{
    char buf[409600];
    memcpy(buf, box , 409600);
    int num;
    sscanf(buf,"%d",&num);
    // printf("mailbox width num = %d\n", num);
    if(num==0)
    {
        return 0;
    }
    return -1;
}
int mailbox_check_full(mailbox_t box)
{
    char buf[409600];
    memcpy(buf, box , 409600);
    int num;
    sscanf(buf,"%d",&num);
    // printf("mailbox width num = %d\n", num);
    if(num==99)
    {
        return 0;
    }
    return -1;
}

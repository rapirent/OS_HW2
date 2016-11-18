#include "api.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

struct node{
    int id;
    char name[SIZE_OF_SHORT_STRING];
    int chatroom_number;
    mailbox_t mailbox_address;
    struct node *next;
};
int main(void)
{
	mailbox_t server_mailbox;
    if((server_mailbox = mailbox_open(0))==NULL)
	{
		printf("\nServer Open ERROR\n");
		exit(1);
	}
    printf("Server Opened Successed!\n");
    printf("Welcome! Server user!\n\n");
    printf("-------------------------------------------------------\n\n");
    printf("what do you want to do next?\n");
    printf("(you can choose command from the command list below)\n");
    printf("1)END this server(Usage: E)\n\n");
    printf("-------------------------------------------------------\n\n");
    // JOIN, BROADCAST and LEAVE
    fcntl(0, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    fcntl(1, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    struct node* first = NULL;
    mail_t* recv_mail =((mail_t*)malloc(sizeof(mail_t)));
    char input = 'F';
    while((input= getchar())!='E')
    {
        if(mailbox_recv(server_mailbox,recv_mail)==0)
        {
            printf("you got a mail!!!!\n");
            printf("the recv mail info:\n");
            printf("-------------------------------------------------------\n");
            printf("from=%d\nto=%d\ntype=%d\nsstr=%s\nlstr=%s\n",(*recv_mail).from,(*recv_mail).to,(*recv_mail).type,(*recv_mail).sstr,(*recv_mail).lstr);//,((*mail).from),((*mail).type));//,((*mail).sstr),((*mail).lstr));
            printf("-------------------------------------------------------\n");
            struct node *ptr;
            struct node *cur;
            struct node *prev;
            int exsisted_user;
            int select_chatroom_number;
            switch((*recv_mail).type)
            {
//                 #define BROADCAST 1
// #define JOIN 2
// #define LEAVE 3
// #define WHISPER 4
// #define LIST 5
// #define NO_PRIVILEGE 6
                case BROADCAST:
                    printf("this mail is about BROADCAST\n");
                    exsisted_user = 0;
                    for(ptr = first; ptr != NULL; ptr = ptr->next)
                    {
                        if(ptr->id ==(*recv_mail).from)
                        {
                            //已經有了！
                            exsisted_user = 1;
                            strncpy((*recv_mail).sstr,ptr->name,SIZE_OF_SHORT_STRING);
                            //取得BROADCAST發送者名字，為了與投影片上一致
                            select_chatroom_number = ptr->chatroom_number;
                            (*recv_mail).from = 0;
                            break;
                        }
                    }
                    if(exsisted_user)
                    {
                        //信件轉送
                        // int samed_chatroom = 1;
                        (*recv_mail).from = 0;
                        (*recv_mail).to = select_chatroom_number;
                        //以to來儲存chatroom number
                        for(ptr = first; ptr != NULL; ptr = ptr->next)
                        {
                            if(ptr->chatroom_number == select_chatroom_number)
                            {
                                // samed_chatroom = 0;
                                if(mailbox_send(ptr->mailbox_address, recv_mail)==-1)
                                {
                                    printf("\nsend ERROR\n");
                                    fprintf(stderr,"%d\n",errno);
                                    exit(1);
                                }
                            }
                        }
                    }
                    else
                    {
                        printf("this user is not in list\n\n");
                        mailbox_t response_address = mailbox_open((*recv_mail).from);
                        mail_t response_mail;
                        response_mail.from = 0;
                        response_mail.type = NO_PRIVILEGE;
                        strcpy(response_mail.lstr,"you are not in chatroom");
                        if(mailbox_send(response_address, &response_mail)==-1)
                        {
                            printf("\nsend ERROR\n");
                            fprintf(stderr,"%d\n",errno);
                            exit(1);
                        }
                        mailbox_close(response_address);
                    }
                    break;
                case JOIN:
                    printf("this mail is about JOIN\n");
                    exsisted_user = 1;
                    for(ptr = first; ptr != NULL; ptr = ptr->next)
                    {
                        if(ptr->id ==(*recv_mail).from)
                        {
                            //已經有了！
                            exsisted_user = 0;
                            break;
                        }
                    }
                    if(exsisted_user)
                    {
                        struct node *new_node = malloc(sizeof(struct node));
                        new_node->id = (*recv_mail).from;
                        new_node->mailbox_address = mailbox_open((*recv_mail).from);
                        new_node->chatroom_number = (*recv_mail).to;
                        // select_chatroom_number = (*recv_mail).to;
                        strcpy(new_node->name,(*recv_mail).sstr);
                        new_node->next = first;
                        first = new_node;
                        //信件轉送
                        (*recv_mail).from = 0;
                        for(ptr = first; ptr != NULL; ptr = ptr->next)
                        {
                            // if(ptr->chatroom_number == select_chatroom_number)
                            // {
                                // 相同chatroom!
                            if(mailbox_send(ptr->mailbox_address, recv_mail)==-1)
                            {
                                printf("\nsend ERROR\n");
                                fprintf(stderr,"%d\n",errno);
                                exit(1);
                            }
                            // }
                        }
                    }
                    else
                    {
                        printf("this user is already exsisted\n\n");
                        mailbox_t response_address = mailbox_open((*recv_mail).from);
                        mail_t response_mail;
                        response_mail.from = 0;
                        response_mail.type = NO_PRIVILEGE;
                        strcpy(response_mail.lstr,"you are already exsisted in a chatroom");
                        if(mailbox_send(response_address, &response_mail)==-1)
                        {
                            printf("\nsend ERROR\n");
                            fprintf(stderr,"%d\n",errno);
                            exit(1);
                        }
                        mailbox_close(response_address);
                    }
                    break;
                case WHISPER:
                    printf("this mail is about WHISPER\n");
                    exsisted_user = 0;
                    for(ptr = first; ptr != NULL; ptr = ptr->next)
                    {
                        if(ptr->id ==(*recv_mail).from)
                        {
                            //已經有了！
                            exsisted_user = 1;
                            select_chatroom_number = ptr->chatroom_number;
                            break;
                        }
                    }
                    if(exsisted_user)
                    {
                        int whisper_user_exsisted = 1;
                        int samed_chatroom = 1;
                        mailbox_t response_address = ptr->mailbox_address;
                        for(ptr = first; ptr != NULL; ptr = ptr->next)
                        {
                            if(ptr->id == (*recv_mail).to)
                            {
                                whisper_user_exsisted = 0;
                                //這個人存在！
                                if(select_chatroom_number == ptr->chatroom_number)
                                {
                                    samed_chatroom = 0;
                                    //在相同的chatroom!
                                    if(mailbox_send(ptr->mailbox_address, recv_mail)==-1)
                                    {
                                        printf("\nsend ERROR\n");
                                        fprintf(stderr,"%d\n",errno);
                                        exit(1);
                                    }
                                    break;
                                }
                            }
                        }
                        if(whisper_user_exsisted)
                        {
                            printf("whisper user is not in list\n\n");
                            mail_t response_mail;
                            response_mail.from = 0;
                            response_mail.type = NO_PRIVILEGE;
                            strcpy(response_mail.lstr,"whisper user is not in list");
                            if(mailbox_send(response_address, &response_mail)==-1)
                            {
                                printf("\nsend ERROR\n");
                                fprintf(stderr,"%d\n",errno);
                                exit(1);
                            }
                        }
                        else if(samed_chatroom)
                        {
                            printf("whisper user is not in your chatroom\n\n");
                            mail_t response_mail;
                            response_mail.from = 0;
                            response_mail.type = NO_PRIVILEGE;
                            strcpy(response_mail.lstr,"whisper user is not in your chatroom");
                            if(mailbox_send(response_address, &response_mail)==-1)
                            {
                                printf("\nsend ERROR\n");
                                fprintf(stderr,"%d\n",errno);
                                exit(1);
                            }
                            mailbox_close(response_address);
                        }
                    }
                    else
                    {
                        mailbox_t response_address = mailbox_open((*recv_mail).from);
                        printf("this user is not in list\n\n");
                        mail_t response_mail;
                        response_mail.from = 0;
                        response_mail.type = NO_PRIVILEGE;
                        strcpy(response_mail.lstr,"you are not in any chatroom");
                        if(mailbox_send(response_address, &response_mail)==-1)
                        {
                            printf("\nsend ERROR\n");
                            fprintf(stderr,"%d\n",errno);
                            exit(1);
                        }
                        mailbox_close(response_address);
                    }
                    break;
                case LIST:
                    printf("this mail is about LIST\n");
                    exsisted_user = 0;
                    for(ptr = first; ptr != NULL; ptr = ptr->next)
                    {
                        if(ptr->id ==(*recv_mail).from)
                        {
                            //已經有了！
                            select_chatroom_number = ptr->chatroom_number;
                            exsisted_user = 1;
                            break;
                        }
                    }
                    if(exsisted_user)
                    {
                        mail_t list_mail;
                        list_mail.from = 0;
                        list_mail.type = LIST;
                        strcpy(list_mail.sstr,"\t");
                        strcpy(list_mail.lstr,"");
                        int count=0;
                        mailbox_t response_address = ptr->mailbox_address;
                        char temp[512]={'\0'};
                        printf("list=%s\n",list_mail.lstr);
                        printf("user_list:\n");
                        for(ptr = first; ptr != NULL; ptr = ptr->next)
                        {
                            printf("%s:%d(%d)\n",ptr->name,ptr->id,ptr->chatroom_number);
                            if(ptr->chatroom_number == select_chatroom_number)
                            {
                                sprintf(temp,"(%d)\t\0",ptr->id);
                            // printf("temp=%s\n",temp);
                            // printf("strlen(temp)=%d\n",strlen(temp));
                            // printf("strlen(ptr->name)=%d\n",strlen(ptr->name));
                            // int foo;
                            // for(foo=0;foo<strlen(temp);foo++)
                            // {
                                // printf("temp[%d]=%c\n",foo,temp[foo]);
                            // }
                                count=count + strlen(ptr->name) + strlen(temp);
                            // temp[strlen(temp)] = '\0';
                                if(count<SIZE_OF_LONG_STRING)
                                {
                                    strcat(list_mail.lstr,ptr->name);
                                // printf("list=%s\n",list_mail.lstr);
                                    strcat(list_mail.lstr,temp);
                                // printf("list=%s\n",list_mail.lstr);
                                }
                            }
                        }
                        list_mail.to = select_chatroom_number;
                        //以to來接收該chatroom之number
                        // printf("%s\n",list_mail.lstr);
                        if(mailbox_send(response_address, &list_mail)==-1)
                        {
                            printf("\nsend ERROR\n");
                            fprintf(stderr,"%d\n",errno);
                            exit(1);
                        }
                    }
                    else
                    {
                        printf("this user is not in list\n\n");
                        mailbox_t response_address = mailbox_open((*recv_mail).from);
                        mail_t response_mail;
                        response_mail.from = 0;
                        response_mail.type = NO_PRIVILEGE;
                        strcpy(response_mail.lstr,"you are not in chatroom");
                        if(mailbox_send(response_address, &response_mail)==-1)
                        {
                            printf("\nsend ERROR\n");
                            fprintf(stderr,"%d\n",errno);
                            exit(1);
                        }
                        mailbox_close(response_address);
                    }
                    break;
                case LEAVE:
                    printf("this mail is about LEAVE\n");
                    exsisted_user = 0;
                    for(cur = first, prev = NULL; cur !=NULL&& cur->id!=(*recv_mail).from; prev = cur, cur = cur->next);
                    if(cur != NULL)
                    {
                        exsisted_user = 1;
                        select_chatroom_number = cur->chatroom_number;
                    }
                    if(exsisted_user)
                    {
                        //信件轉送
                        (*recv_mail).from = 0;
                        (*recv_mail).to = select_chatroom_number;
                        strncpy((*recv_mail).sstr,cur->name,SIZE_OF_SHORT_STRING);
                        //刪除節點
                        if(prev == NULL)
                        {
                            first = first->next;
                        }
                        else
                        {
                            prev->next=cur->next;
                        }
                        for(ptr = first; ptr != NULL; ptr = ptr->next)
                        {
                            if(mailbox_send(ptr->mailbox_address, recv_mail)==-1)
                            {
                                printf("\nsend ERROR\n");
                                fprintf(stderr,"%d\n",errno);
                                exit(1);
                            }
                        }
                    }
                    else
                    {
                        printf("this user is not in list\n\n");
                        mailbox_t response_address = mailbox_open((*recv_mail).from);
                        mail_t response_mail;
                        response_mail.from = 0;
                        response_mail.type = NO_PRIVILEGE;
                        strcpy(response_mail.lstr,"you are not in any chatroom");
                        if(mailbox_send(response_address, &response_mail)==-1)
                        {
                            printf("\nsend ERROR\n");
                            fprintf(stderr,"%d\n",errno);
                            exit(1);
                        }
                        mailbox_close(response_address);
                    }
                    break;
                default:
                    printf("Unexpected Mail!\n\n");
                    break;
            }
            free(recv_mail);
            recv_mail =((mail_t*)malloc(sizeof(mail_t)));
        }
    }
    int a;
    scanf("%d",&a);
    if(mailbox_unlink(0)==-1)
    {
        printf("\nERROR!\n");
        exit(1);
    }
    return 0;
}

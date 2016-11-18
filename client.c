#include "api.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("Error: wrong usage!\n");
        printf("usage:%s [client_id] [client_name]\n",argv[0]);
        printf("(client_id should greater than 1)\n");
        exit(1);
    }
    int id;
    sscanf (argv[1],"%d",&id);
    if(id<=0)
    {
        printf("Error: wrong usage!\n");
        printf("usage:%s [client_id] [client_name]\n",argv[0]);
        printf("(client_id should greater than 1)\n");
        exit(1);
    }
    mailbox_t client_mailbox = mailbox_open(id);
    printf("Welcome! client user:%s you are using %d mailbox\n\n",argv[2],id);
    mailbox_t server_mailbox = mailbox_open(0);
    printf("you have connected to server mailbox!\n");
    printf("-------------------------------------------------------\n\n");
    printf("what do you want to do next?\n");
    printf("(you can choose commands from the commands list below)\n");
    printf("1)JOIN server(Usage: J [chatroom_number])(chatroom_number should greater than 0)\n");
    printf("2)LEAVE server(Usage: L)\n");
    printf("3)BROADCAST to all(Usage:B [the messange you want to BROADCAST])\n");
    printf("4)WHISPER to someone(Usage: W [user id] [the messange you want to WHISPER])\n");
    printf("5)LIST all client in server(Usage: l)\n");
    printf("6)END this client(Usage: E)\n\n");
    printf("-------------------------------------------------------\n\n");
    fcntl(0, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    fcntl(1, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK);
    char input = 'F';
    // char buffer[512];
    mail_t* recv_mail = ((mail_t*)malloc(sizeof(mail_t)));
    // int leaved = 1;
    while((input = getchar())!='E')
    {
        if(mailbox_recv(client_mailbox,recv_mail)==0)
        {
            // printf("you got a mail!!!!\n");
            // printf("the recv mail info:\n");
            // printf("-------------------------------------------------------\n");
            // printf("from=%d\nto=%d\ntype=%d\nsstr=%s\nlstr=%s\n",(*recv_mail).from,(*recv_mail).to,(*recv_mail).type,(*recv_mail).sstr,(*recv_mail).lstr);//,((*mail).from),((*mail).type));//,((*mail).sstr),((*mail).lstr));
            // printf("-------------------------------------------------------\n");
            switch((*recv_mail).type)
            {
                case BROADCAST:
                    printf("user:%s in chatroom(%d) want to BROADCAST to all chatroom member\n\n",(*recv_mail).sstr,(*recv_mail).to);
                    printf("%s(%d):「%s」\n",(*recv_mail).sstr,(*recv_mail).to,(*recv_mail).lstr);
                    break;
                case JOIN:
                    // printf("this mail is about JOIN\n");
                    printf("user:%s has JOIN the chatroom(%d)!!\n\n",(*recv_mail).sstr,(*recv_mail).to);
                    break;
                case WHISPER:
                    printf("user:%s (who are in your chatroom) want to WHISPER with you!\n\n",(*recv_mail).sstr);
                    printf("%s:「%s」\n",(*recv_mail).sstr,(*recv_mail).lstr);
                    break;
                case LIST:
                    printf("user_list about chatroom(%d):\n%s\n",(*recv_mail).to,(*recv_mail).lstr);
                    break;
                case LEAVE:
                    printf("user:%s had LEFT the chatroom(%d)!!\n\n",(*recv_mail).sstr,(*recv_mail).to);
                    break;
                case NO_PRIVILEGE:
                    printf("ERROR:you have no privilege to access this command!!\n");
                    printf("「%s」\n\n",(*recv_mail).lstr);
                    break;
                default:
                    printf("Unexpected Mail\n\n");
                    break;
            }
            free(recv_mail);
            recv_mail = ((mail_t*)malloc(sizeof(mail_t)));
            printf("-------------------------------------------------------\n\n");
            printf("what do you want to do next?\n");
        }
        mail_t join_request;
        mail_t broadcast_request;
        mail_t leave_request;
        mail_t list_request;
        mail_t whisper_request;
        switch(input)
        {
            case 'J':
                printf("you choose JOIN command\n");
                // mail_t join_request;
                join_request.from = id;//sender’s mailbox id
                // join_request.to = 0;
                join_request.type = JOIN;
                scanf("%*c%d",&join_request.to);
                if(join_request.to <= 0)
                {
                    printf("please input a chatroom_number greater than 1\n");
                }
                else
                {
                    strcpy(join_request.lstr,"\t");
                    strcpy(join_request.sstr,argv[2]);
                    // }
                    if(mailbox_send(server_mailbox, &join_request)==-1)
                    {
                        printf("\nsend ERROR\n");
                        fprintf(stderr,"%d\n",errno);
                        exit(1);
                    }
                }
                break;
            case 'B':
                printf("you choose BROADCAST command\n");
                // printf("plase type the messange you want to BROADCAST:\n\n");
                // mail_t broadcast_request;
                // wait(5);
                // n = read(0,buffer,512);
                // char str_temp[4096]={'\0'};
                scanf("%*c%[^\n]",broadcast_request.lstr);
                broadcast_request.from = id;
                broadcast_request.to = 0;
                broadcast_request.type = BROADCAST;
                strcpy(broadcast_request.sstr,argv[2]);
                // strcpy(join_request.lstr,buffer);
                // m = write (1,buffer,n);
                if(mailbox_send(server_mailbox, &broadcast_request)==-1)
                {
                    printf("\nsend ERROR\n");
                    fprintf(stderr,"%d\n",errno);
                    exit(1);
                }
                break;
            case 'L':
                printf("you choose LEAVE command\n");
                // mail_t leave_request;
                leave_request.from = id;
                leave_request.to = 0;
                leave_request.type = LEAVE;
                strcpy(leave_request.sstr,"\t");
                strcpy(leave_request.lstr,"\t");
                if(mailbox_send(server_mailbox, &leave_request)==-1)
                {
                    printf("\nsend ERROR\n");
                    fprintf(stderr,"%d\n",errno);
                    exit(1);
                }
                printf("you have left the chatroom\n");
                break;
            case 'l':
                printf("you choose LIST command\n");
                // mail_t list_request;
                list_request.from = id;
                list_request.to = 0;
                list_request.type = LIST;
                strcpy(list_request.sstr,argv[2]);
                strcpy(list_request.lstr,"\t");
                if(mailbox_send(server_mailbox, &list_request)==-1)
                {
                    printf("\nsend ERROR\n");
                    fprintf(stderr,"%d\n",errno);
                    exit(1);
                }
                break;
            case 'W':
                printf("you choose WHISPER command\n");
                // mail_t whisper_request;
                whisper_request.from = id;
                whisper_request.type = WHISPER;
                scanf("%d",&whisper_request.to);
                scanf("%*c%[^\n]",whisper_request.lstr);
                strcpy(list_request.sstr,argv[2]);
                if(mailbox_send(server_mailbox, &whisper_request)==-1)
                {
                    printf("\nsend ERROR\n");
                    fprintf(stderr,"%d\n",errno);
                    exit(1);
                }
                printf("-------------------------------------------------------\n\n");
                printf("what do you want to do next?\n");
                break;
            default:
                break;
        }
        // input = getchar()
        // n = read(0,buffer,4096);
             // if (input > 0)
             // {
                   // printf("Got %d byte data.\n", strlen(input));
             // }
             // if (input>0)
             //       m = write (1,input,1);
             // if ((input<0||m<0) && (errno != EAGAIN))
             //       break;
             // if ((input<0||m<0) && (errno == EAGAIN))
             // {
             //     perror( input<0 ? "stdin working in NONBLOCK, no data!" : "stdout working in NONBLOCK, no data");
             // }
         // sleep(3);
    }
    mail_t leave_request;
    leave_request.from = id;
    leave_request.to = 0;
    leave_request.type = LEAVE;
    strcpy(leave_request.sstr,"\t");
    strcpy(leave_request.lstr,"\t");
    if(mailbox_send(server_mailbox, &leave_request)==-1)
    {
        printf("\nsend ERROR\n");
        fprintf(stderr,"%d\n",errno);
        exit(1);
    }
    free(recv_mail);
    if(mailbox_close(0)==-1)
    {
        printf("\nCLOSE SERVER ERROR!\n");
        exit(1);
    }
    if(mailbox_unlink(id)==-1)
    {
        printf("\nUNLINK CLIENT ERROR!\n");
        exit(1);
    }
    printf("you have disconnected!\n");
    return 0;
}

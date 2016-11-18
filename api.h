#ifndef MAILBOX_API

#define MAILBOX_API
#define SIZE_OF_SHORT_STRING 64
#define SIZE_OF_LONG_STRING 512
#define BROADCAST 1
#define JOIN 2
#define LEAVE 3
#define WHISPER 4
#define LIST 5
#define NO_PRIVILEGE 6
typedef struct __MAIL {
	int from;//sender’s mailbox id
    int to;
	int type;
	char sstr[SIZE_OF_SHORT_STRING];
	char lstr[SIZE_OF_LONG_STRING];
}mail_t;
typedef void* mailbox_t;

mailbox_t mailbox_open(int id);
int mailbox_unlink(int id);
int mailbox_close(mailbox_t box);
int mailbox_send(mailbox_t box, mail_t* mail);
int mailbox_recv(mailbox_t box, mail_t* mail);
int mailbox_check_empty(mailbox_t box);
int mailbox_check_full(mailbox_t box);

#endif

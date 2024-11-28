//написать программу получающую в качестве параметров два номера. Первый номер это 
//номер для получения сообщений, второй номер для передачи сообщений. Можно  
//запустить несколько экземпляров такой программы и они все будут обмениваться  
//сообщениями между собой по номерам.
// Всё это нужно реализовать через одну очередь. 
#include <sys/types.h>	
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MESSAGE_LENGTH 255
#define LAST_MESSAGE 254

int main(int argc, char *argv[])
{
	int msqid;
	char pathname[]="3_queue.c";
	key_t  key;
	int len, maxlen;
	long input_type = 0;
	long output_type = 0;
	pid_t proc_id;
	
	struct mymsgbuf
	{
		long mtype;
		char mtext[MESSAGE_LENGTH];
	} mybuf;
	
	//тип получаемых сообщений
	input_type = atoi(argv[2]);
	//тип отправляемых
        output_type = atoi(argv[1]);
    
	key = ftok(pathname, 1);
	if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
	{
		printf("Can\'t get msqid\n");
                exit(-1);
	}
	
	proc_id = fork();

	if(proc_id > 0)
	{
		mybuf.mtype = input_type;
		//parent
		//printf("Parent\n");
		while (1) 
        	{
                	if ((len = msgrcv(msqid, (struct msgbuf *)&mybuf, MESSAGE_LENGTH, input_type, 0)) < 0)
                	{
                        	printf("Can\'t receive message from queue\n");
                        	exit(-1);
                	}

                	printf("message type = %ld, info = %s\n", mybuf.mtype, mybuf.mtext);
        	}
	}
	else if(proc_id == 0)
	{
		while(1)
		{	
			//child
			//printf("Child\n");
			mybuf.mtype = output_type;
			fgets(mybuf.mtext, MESSAGE_LENGTH, stdin);

			len = strlen(mybuf.mtext) + 1;
       
                	if (msgsnd(msqid, (struct msgbuf *)&mybuf, len, 0) < 0)
                	{
                        	printf("Can\'t send message to queue\n");
                        	msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
                        	exit(-1);
                	}
		}
             
	}

	return 0;
  //один из процессов проверяет очередь на предмет наличия в ней сообщений,
  // адресованных данному терминалу и выводит их на экран
  
  // второй процесс ожидает ввода с клавиатуры и отправляет сообщения,
  // встретив символ перевода строки 
}

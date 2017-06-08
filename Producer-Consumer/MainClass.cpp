/*
Created by IceTesla
2017/4/25
Rewrite after wrong delete
*/

#include<stdio.h> 
#include<stdlib.h> 
#include<time.h> 
#include<windows.h> 

/*���建����*/
typedef int buffer_item;
#define BUFFER_SIZE 10
buffer_item buffer[BUFFER_SIZE];

/*���建����ָ��*/
int in = 0;
int out = 0;

/*���建�������뺯��*/
int insert_item(buffer_item item)
{   
	/* insert item into buffer  
	return 0 if successful,otherwise   
	return -1 indicating an error condition */
	buffer[in] = item;
	in = (in + 1) % BUFFER_SIZE;
	return 0; 
} 

/*���建����ɾ������*/
int remove_item(buffer_item *item)
{   
	/*remove an object from buffer
	placing it in item
	return 0 if successful,otherwise
	reutrn -1 indication an error condition */
	if (buffer[out] == NULL)
		return -1;  
	*item = buffer[out];
	out = (out + 1) % BUFFER_SIZE;
	return 0; 
}

/*���廥���� ����Ϊ ��ȫ���ԣ��Ƿ�Ϊ��ʼӵ���ߣ���������*/
HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
/*�����ź��� ����Ϊ ��ȫ���ԣ���ֵ�����ֵ������*/
HANDLE empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
HANDLE full = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);

/*�����������̺߳���*/
DWORD WINAPI producer(LPVOID Param) 
{
	int rand_t;
	int n = (int)Param;
	srand(GetCurrentThreadId());

	while (true)
	{
		/*˯�����һ��ʱ��*/
		Sleep(rand() % 100);
		/*�ȴ��ź���empty���ڴ���̬*/
		WaitForSingleObject(empty, INFINITE);
		/*�ȴ�������mutex���ڴ���̬*/
		WaitForSingleObject(mutex, INFINITE);

		rand_t = rand();

		printf("producer %d produced %d\n", n, rand_t);
		if (insert_item(rand_t)) 
			printf("\tinsert error\n");

		/*�ͷŻ�����mutex*/
		ReleaseMutex(mutex);
		/*�����ź���*/
		ReleaseSemaphore(full, 1, NULL);
	}
	return 0;
}

/*�����������̺߳���*/
DWORD WINAPI consumer(LPVOID Param)
{
	int rand_t;
	int n = (int)Param;
	srand(GetCurrentThreadId());

	while (true)
	{
		/*˯�����һ��ʱ��*/
		Sleep(rand() % 100);
		/*�ȴ��ź���full���ڴ���̬*/
		WaitForSingleObject(full, INFINITE);
		/*�ȴ�������mutex���ڴ���̬*/
		WaitForSingleObject(mutex, INFINITE);

		if (remove_item(&rand_t)) 
			printf("\tremove error\n");
		else 
			printf(" consumer %d consumed %d\n", n, rand_t);

		/*�ͷŻ�����mutex*/
		ReleaseMutex(mutex);
		/*�����ź���*/
		ReleaseSemaphore(empty, 1, NULL);
	}
}

int main(int argc, char *argv[])
{
	//system("color 0F");
	/*��ȡ�����в���*/
	int sleeptime = atoi(argv[1]);
	int producer_num = atoi(argv[2]);
	int consumer_num = atoi(argv[3]);

	int Param;
	/*�����������߳�*/
	DWORD * ThreadID_producer = new DWORD[producer_num];
	HANDLE * ThreadHandle_producer = new HANDLE[producer_num];

	for (Param = 0; Param < producer_num; Param++)
		ThreadHandle_producer[Param] = CreateThread(NULL, 0, producer, (LPVOID)Param, 0, &ThreadID_producer[Param]);
	/*�����������߳�*/
	DWORD * ThreadID_consumer = new DWORD[consumer_num];
	HANDLE * ThreadHandle_consumer = new HANDLE[consumer_num];

	for (Param = 0; Param < consumer_num; Param++)
		ThreadHandle_consumer[Param] = CreateThread(NULL, 0, consumer, (LPVOID)Param, 0, &ThreadID_consumer[Param]);
	/*Sleep*/
	Sleep(sleeptime);
	/*�˳�*/
	return 0;
}
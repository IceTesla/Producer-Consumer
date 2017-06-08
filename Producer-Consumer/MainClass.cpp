/*
Created by IceTesla
2017/4/25
Rewrite after wrong delete
*/

#include<stdio.h> 
#include<stdlib.h> 
#include<time.h> 
#include<windows.h> 

/*定义缓冲区*/
typedef int buffer_item;
#define BUFFER_SIZE 10
buffer_item buffer[BUFFER_SIZE];

/*定义缓冲区指针*/
int in = 0;
int out = 0;

/*定义缓冲区插入函数*/
int insert_item(buffer_item item)
{   
	/* insert item into buffer  
	return 0 if successful,otherwise   
	return -1 indicating an error condition */
	buffer[in] = item;
	in = (in + 1) % BUFFER_SIZE;
	return 0; 
} 

/*定义缓冲区删除函数*/
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

/*定义互斥锁 参数为 安全属性，是否为初始拥有者，锁的命名*/
HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
/*定义信号量 参数为 安全属性，初值，最大值，命名*/
HANDLE empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
HANDLE full = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);

/*定义生产者线程函数*/
DWORD WINAPI producer(LPVOID Param) 
{
	int rand_t;
	int n = (int)Param;
	srand(GetCurrentThreadId());

	while (true)
	{
		/*睡眠随机一段时间*/
		Sleep(rand() % 100);
		/*等待信号量empty处于触发态*/
		WaitForSingleObject(empty, INFINITE);
		/*等待互斥锁mutex处于触发态*/
		WaitForSingleObject(mutex, INFINITE);

		rand_t = rand();

		printf("producer %d produced %d\n", n, rand_t);
		if (insert_item(rand_t)) 
			printf("\tinsert error\n");

		/*释放互斥锁mutex*/
		ReleaseMutex(mutex);
		/*递增信号量*/
		ReleaseSemaphore(full, 1, NULL);
	}
	return 0;
}

/*定义消费者线程函数*/
DWORD WINAPI consumer(LPVOID Param)
{
	int rand_t;
	int n = (int)Param;
	srand(GetCurrentThreadId());

	while (true)
	{
		/*睡眠随机一段时间*/
		Sleep(rand() % 100);
		/*等待信号量full处于触发态*/
		WaitForSingleObject(full, INFINITE);
		/*等待互斥锁mutex处于触发态*/
		WaitForSingleObject(mutex, INFINITE);

		if (remove_item(&rand_t)) 
			printf("\tremove error\n");
		else 
			printf(" consumer %d consumed %d\n", n, rand_t);

		/*释放互斥锁mutex*/
		ReleaseMutex(mutex);
		/*递增信号量*/
		ReleaseSemaphore(empty, 1, NULL);
	}
}

int main(int argc, char *argv[])
{
	//system("color 0F");
	/*获取命令行参数*/
	int sleeptime = atoi(argv[1]);
	int producer_num = atoi(argv[2]);
	int consumer_num = atoi(argv[3]);

	int Param;
	/*定义生产者线程*/
	DWORD * ThreadID_producer = new DWORD[producer_num];
	HANDLE * ThreadHandle_producer = new HANDLE[producer_num];

	for (Param = 0; Param < producer_num; Param++)
		ThreadHandle_producer[Param] = CreateThread(NULL, 0, producer, (LPVOID)Param, 0, &ThreadID_producer[Param]);
	/*定义消费者线程*/
	DWORD * ThreadID_consumer = new DWORD[consumer_num];
	HANDLE * ThreadHandle_consumer = new HANDLE[consumer_num];

	for (Param = 0; Param < consumer_num; Param++)
		ThreadHandle_consumer[Param] = CreateThread(NULL, 0, consumer, (LPVOID)Param, 0, &ThreadID_consumer[Param]);
	/*Sleep*/
	Sleep(sleeptime);
	/*退出*/
	return 0;
}
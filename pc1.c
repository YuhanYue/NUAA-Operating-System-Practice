#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


#define CAPACITY 4      //缓冲区最大容量
char buffer1[CAPACITY]; //缓冲区数组
char buffer2[CAPACITY];
int in1, in2;           //缓冲区写指针
int out1, out2;         //缓冲区读指针
int size1, size2;       //缓冲区中数据个数

//缓冲区函数定义
void buffer_init();
char buffer_get(int n);
void buffer_put(char c, int n);
int buffer_is_full(int);
int buffer_is_empty(int);



//互斥量：mutex1用于给buffer1的in1、out1、size1上锁，mutex1用于给buffer2上锁
pthread_mutex_t mutex1, mutex2;

//条件变量：同样分别对应buffer1和2
pthread_cond_t wait_empty_buffer1, wait_empty_buffer2;
pthread_cond_t wait_full_buffer1, wait_full_buffer2;
//初始化信号量


//生产者执行的线程
void *produce(void *arg)
{
    char item;
    
    for(int i = 0; i < CAPACITY*2; i++)//生产 CAPACITY*2 个数据
    {
        pthread_mutex_lock(&mutex1);
        
        // 当缓冲区为满时，生产者需要等待
        while(buffer_is_full(1))
            // 当前线程已经持有了mutex，首先释放mutex，然后阻塞，醒来后再次获取mutex
            pthread_cond_wait(&wait_empty_buffer1, &mutex1);
        
        //此时缓冲区不满
        item = 'a' + i;// a ~ h
        buffer_put(item, 1);
        
        // 改变条件变量并且解锁
        pthread_cond_signal(&wait_full_buffer1);
        pthread_mutex_unlock(&mutex1);
        
        printf("produce item: %c\n", item);


    }
    return NULL;
}

//消费者执行的线程
void *consume(void *arg)
{
    int item;
    for(int i = 0; i < CAPACITY*2; i++)//消费 CAPACITY*2 个数据
    {
        pthread_mutex_lock(&mutex2);
        // 当缓冲区为空时，消费者需要等待
        while(buffer_is_empty(2))
            //当前线程已经持有了mutex，首先释放mutex，然后阻塞，醒来后再次获取mutex
            pthread_cond_wait(&wait_full_buffer2, &mutex2);
        
        // 此时缓冲区非空
        item = buffer_get(2);

        pthread_mutex_unlock(&mutex2);

        // 缓冲区的状态发生了变化，唤醒其它的生产者或消费者
        pthread_cond_signal(&wait_empty_buffer2);

       printf("                consume item: %c\n", item);
    }
    return NULL;
}

//计算者执行的线程
void *compute(void *arg)
{
    char item;
    for(int i = 0; i < CAPACITY*2; i++)//计算CAPACITY*2个数据
    {
      //当buffer1为空时，计算者不能取数据
        pthread_mutex_lock(&mutex1);
        while(buffer_is_empty(1))
            pthread_cond_wait(&wait_full_buffer1, &mutex1);
      //从buffer1中取出要计算的数据
        item = buffer_get(1);
      //解锁，唤醒其他进程可以访问共享变量
        pthread_cond_signal(&wait_empty_buffer1);
        pthread_mutex_unlock(&mutex1);
      
            //进行计算
        item += 'A' - 'a';
      
            //当buffer2为满时，计算者不能存数据
        pthread_mutex_lock(&mutex2);
        while(buffer_is_full(2))
            pthread_cond_wait(&wait_empty_buffer2, &mutex2);
      //存入计算完毕的数据
        buffer_put(item, 2);
        printf("        compute item: %c\n", item);
            //唤醒，解锁
        pthread_cond_signal(&wait_full_buffer2);
        pthread_mutex_unlock(&mutex2);

    }
    return NULL;
}

int main()
{
    buffer_init();
    pthread_t producer_tid, computer_tid, consumer_tid;

    //初始化条件变量
   pthread_mutex_init(&mutex1, NULL);
   pthread_mutex_init(&mutex2, NULL);
   pthread_cond_init(&wait_empty_buffer1, NULL);
   pthread_cond_init(&wait_empty_buffer2, NULL);
   pthread_cond_init(&wait_full_buffer1, NULL);
   pthread_cond_init(&wait_full_buffer2, NULL);

    //创建三个线程
    pthread_create(&producer_tid, NULL, produce, NULL);
    pthread_create(&computer_tid, NULL, compute, NULL);
    pthread_create(&consumer_tid, NULL, consume, NULL);

    //等待线程执行
    pthread_join(producer_tid, NULL);
    pthread_join(computer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    return 0;
}


void buffer_init(){
    in1 = 0;
    in2 = 0;
    out1 = 0;
    out2 = 0;
    size1 = 0;
    size2 =0;
}

char buffer_get(int n)
{
    char item;
    if(n == 1)
    {
        item = buffer1[out1];
        out1 = (out1 + 1) % CAPACITY;
        size1--;
    }
    else if(n == 2)
    {
        item = buffer2[out2];
        out2 = (out2 + 1) % CAPACITY;
        size2--;
    }
    else
        exit(-1);
    return item;
}

void buffer_put(char item, int n)
{
    if(n == 1)
    {
        buffer1[in1] = item;
        in1 = (in1 + 1) % CAPACITY;
        size1++;
    }
    else if(n == 2)
    {
        buffer2[in2] = item;
        in2 = (in2 + 1) % CAPACITY;
        size2++;
    }
    else
        exit(-1);
}


int buffer_is_empty(int n)
{
    if(n == 1)
        return size1 == 0;
    else if(n == 2)
        return size2 == 0;
    else
        exit(-1);
}


int buffer_is_full(int n)
{
    if( n == 1)
        return size1 == CAPACITY;
    else if(n == 2)
        return size2 == CAPACITY;
    else
        exit(-1);
}

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define CAPACITY 4

//缓冲区共享变量定义
char buffer1[CAPACITY];
char buffer2[CAPACITY];
int in1, in2;
int out1, out2;
int size1, size2;

//缓冲区函数定义
void buffer_init();
char buffer_get(int n);
void buffer_put(char c, int n);


//信号量定义
typedef struct {//使用条件变量实现信号量sema_t
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}sema_t;

sema_t mutex_sema1, mutex_sema2;              //用于互斥访问缓冲区in1/in2...
sema_t empty_buffer_sema1, empty_buffer_sema2;//用于线程同步
sema_t full_buffer_sema1, full_buffer_sema2;  //用于线程同步

//初始化信号量
void sema_init(sema_t *sema, int value)
{
    sema->value = value;//记录信号量的值
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

//如果信号量的值<= 0，则等待条件变量
void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    while(sema->value <= 0)
    {
        pthread_cond_wait(&sema->cond, &sema->mutex);
    }
    sema->value--;//值-1
    pthread_mutex_unlock(&sema->mutex);
}


void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    sema->value += 1;
    pthread_cond_signal(&sema->cond);//唤醒等待条件变量的线程
    pthread_mutex_unlock(&sema->mutex);
}




//生产者线程
void *produce(void *arg)
{
    char item;
    for(int i = 0; i < CAPACITY * 2; i++)
    {
        sema_wait(&empty_buffer_sema1);//生产者需要一个空的buffer，所以申请信号量empty_buffer_sema
        sema_wait(&mutex_sema1);

        item = 'a' + i;
        buffer_put(item, 1);
        printf("produce item: %c\n", item);

        sema_signal(&mutex_sema1);//产生一个新的满buffer
        sema_signal(&full_buffer_sema1);//释放
    }
    return NULL;
}


//计算者线程
void *compute(void *arg)
{
    char item;
    for(int i = 0; i < CAPACITY * 2; i++)
    {
        sema_wait(&full_buffer_sema1);//对于buffer1，作为消费者
        sema_wait(&mutex_sema1);
        item = buffer_get(1);
        sema_signal(&mutex_sema1);
        sema_signal(&empty_buffer_sema1);

        item += 'A' - 'a';

        sema_wait(&empty_buffer_sema2);//对于buffer2，作为生产者
        sema_wait(&mutex_sema2);
        buffer_put(item, 2);
        printf("        compute item: %c\n", item);
        sema_signal(&mutex_sema2);
        sema_signal(&full_buffer_sema2);
    }
    return NULL;
}


//消费者线程
void *consume(void *arg)
{
    int item;
    for(int i = 0; i < CAPACITY * 2; i++)
    {
        sema_wait(&full_buffer_sema2);//消费者需要一个满的buffer，申请信号量full_buffer_sema
        sema_wait(&mutex_sema2);//上锁，解决互斥问题
        
        item = buffer_get(2);//取出数据
        printf("                consume item: %c\n", item);

        sema_signal(&mutex_sema2);//取走数据后，产生一个新的空buffer
        sema_signal(&empty_buffer_sema2);//释放信号量empty_buffer_sema
    }
    return NULL;
}


int main()
{
    buffer_init();
    pthread_t producer_tid, computer_tid, consumer_tid;

    //初始化信号量
    sema_init(&mutex_sema1, 1);
    sema_init(&mutex_sema2, 1);
    sema_init(&empty_buffer_sema1, CAPACITY - 1);
    sema_init(&empty_buffer_sema2, CAPACITY - 1);
    sema_init(&full_buffer_sema1, 0);
    sema_init(&full_buffer_sema2, 0);

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

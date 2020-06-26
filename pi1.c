#include <stdio.h>
#include <pthread.h>

#define N 10000

float worker_output;
float master_output;

int sign(int n)////注意从0开始算第一项，计算范围为0～N-1
{
    if(n % 2 == 0)
        return 1;//偶数为正
    else
        return -1;//奇数为负
}

void *worker(void *arg)
{
    int i;
    for(i = N / 2; i < N; i++)
        worker_output += (float)sign(i) / (2*i + 1);
    
    //printf("son_output = %.10f\n", son_output);
    return NULL;
}

void master()
{
    for(int i = 0; i < N / 2; i++)
        master_output += (float)sign(i) / (2*i + 1);
    
    //printf("master_output = %.10f\n", parent_output);
    return;
}

int main()
{
    pthread_t son_tid;
    float total;

    pthread_create(&son_tid, NULL, worker, NULL);
    master();
    pthread_join(son_tid, NULL);//等待辅助线程运算完毕
    total = master_output + worker_output;
    printf("PI = %.10f\n", total * 4);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define NR_TOTAL 100000
#define NR_CPU    2
#define NR_CHILD (NR_TOTAL/NR_CPU)

typedef struct param {//分段计算
    int start;//计算起点
    int end;//计算重点
}Param;

typedef struct result {
    float sum;
}Result;


int sign(int n)//判断符号
{
    if(n % 2 == 0)
        return 1;
    else
        return -1;
}


void *compute(void *arg)
{
    Param *param = (Param *)arg;
    Result *result;
    float sum = 0;
    int i=0;
    
    for(i = param->start; i < param->end; i++)
        sum += (float)sign(i) / (2 * i + 1);
    
    //printf("worker %d = %.10f\n", param->start / NR_CHILD, sum);
    result =(Result *)malloc(sizeof(Result));
    result->sum = sum;
    return result;
}


int main()
{
    pthread_t workers[NR_CPU];//workers数组保存了每一个工作线程的tid
    Param params[NR_CPU];

    int i =0;
    
    for(i = 0; i < NR_CPU; i++)
    {
        Param *param;
        param = &params[i];
        param->start = i * NR_CHILD;
        param->end = (i + 1) * NR_CHILD;
        pthread_create(&workers[i], NULL, compute, param);
    }

    float sum = 0;
    for(i = 0; i < NR_CPU; i++)
    {
        Result *result;
        pthread_join(workers[i], (void **)&result);
        sum += result->sum;
        free(result);
    }

    printf("PI = %.10f\n", sum * 4);
    return 0;
}

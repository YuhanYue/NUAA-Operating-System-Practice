#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>


#define N 20
int nums[N];

typedef struct param {//分段计算
    int start;//计算起点
    int end;//计算重点
    int *nums;
}Param;


//选择排序
void selectSort(int arr[], int n) {
    int i, j;
    int min, temp;
    for (i = 0; i < n - 1; ++i) {
        min = i;
        for (j = i + 1; j < n; ++j) {
            if (arr[j] < arr[min]) {
                min = j;
            }
        }
        if (min != i) {
            temp = arr[i];
            arr[i] = arr[min];
            arr[min] = temp;
        }
    }
}

//子线程
void *thread_func(void *args) {
    Param *param = (Param *)args;
    int left = param->start;
    int right = param->end;
    if(left >= right)
        return NULL;
    selectSort(&nums[param->start], N / 2);//前半段
     /*printf("------------\n");
    for(int i=N/2; i<N; i++){
          
           printf("%d ",nums[i]);
       }*/
    return NULL;
    
}

//生成随机数
void init_nums()
{

    for(int i = 0; i < N; i++)
        nums[i] = rand() % 10000;
    printf("unsorted:  ");
    for(int i = 0; i < N; i++)
        printf("%d ", nums[i]);
    printf("\n");
}


int main(int argc, char *argv[]) {
    init_nums();

    pthread_t child[2];
    Param params[2];
    params[0].start = 0;
    params[0].end = N / 2;
    params[1].start = N / 2;
    params[1].end = N;
    
    for(int i=0; i<2; i++){
        pthread_create(&child[i], NULL, thread_func, &params[i]);// 创建线程辅助运算
    }

    
    // 等待线程执行完毕
    for(int i=0; i<2; i++){
        pthread_join(child[i], NULL);
    }
   
   /*  printf("------------\n");
    for(int i=0; i<N; i++){
       
        printf("%d ",nums[i]);
    }*/
    // 进行归并排序
    int result[N];
    int p = 0, q = N / 2, r = 0;
    while (p < N / 2 || q < N) {
        if (p >= N / 2 && q < N) result[r++] = nums[q++];
        if (p < N / 2 && q >= N) result[r++] = nums[p++];
        if (p < N / 2 || q < N)
            result[r++] = nums[p] < nums[q] ? nums[p++] : nums[q++];
    }
    
    
    printf("result: ");
    for (int i = 0; i < N; i++) printf("%d ", result[i]);
    printf("\n");

    return 0;
}


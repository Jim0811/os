#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
struct timeval start, end;
unsigned long usec;

#define N_LIST (sizeof(original_list) / sizeof(int))
#define usec_elapsed(s, e) (1000000 * ((e).tv_sec - (s).tv_sec) + ((e).tv_usec - (s).tv_usec))

typedef struct
{
    char *id;
    int *first;
    int n;
    unsigned long elapsed;
} sort_args;

void *thread_sort(void *arg);
void *thread_merge(void *arg2);

int original_list[] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8};

int sum;
void *runner(void *param);
int *listncopy(int *dst, int *src, int n);
void print_list(char *id, char *msg, int *first, int n);
float do_sort(char *id, int *first, int n);
float do_merge(char *id, int *first, int n);

int main(int argc, char *argv[])
{
    int mylist[N_LIST];
    listncopy(mylist, original_list, N_LIST);
    do_sort("A1115528-X", mylist, N_LIST);

    print_list("A1115528-M", "All-Old", original_list, N_LIST);

    sort_args arg0 = {"A1115528#0", original_list, N_LIST / 2};
    sort_args arg1 = {"A1115528#1", original_list + N_LIST / 2, N_LIST / 2};

    pthread_t t0, t1;
    pthread_create(&t0, NULL, thread_sort, &arg0);
    pthread_create(&t1, NULL, thread_sort, &arg1);

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    sort_args argm = {"A1115528#M", original_list, N_LIST};
    pthread_t tm;
    pthread_create(&tm, NULL, thread_merge, &argm);
    pthread_join(tm, NULL);

    print_list("A1115528-M", "All-New", original_list, N_LIST);
    printf("A1115528-M spent %lu usec\n", arg0.elapsed + arg1.elapsed + argm.elapsed);

    // pthread_t tid;

    // pthread_create(&tid, NULL, runner, argv[1]);

    // pthread_join(tid, NULL);

    // printf("A1115528 sum = %d\n", sum);
    return 0;
}

void *runner(void *param)
{
    int i, upper = atoi(param);
    sum = 0;
    for (i = 1; i <= upper; i++)
        sum += i;

    pthread_exit(0);
}

int *listncopy(int *dst, int *src, int n)
{
    for (int i = 0; i < n; i++)
        dst[i] = src[i];
    return dst;
}

void print_list(char *id, char *msg, int *first, int n)
{
    printf("%s %s: ", id, msg);
    while (n-- > 0)
        printf(" %d", *first++);
    printf("\n");
}

float do_sort(char *id, int *first, int n)
{
    print_list(id, "Sub-Old", first, n);
    gettimeofday(&start, NULL);
    for (int i = 1; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            if (first[i] < first[j])
            {
                int temp = first[i];
                first[i] = first[j];
                first[j] = temp;
            }
        }
    }
    gettimeofday(&end, NULL);
    print_list(id, "SUB-New", first, n);
    printf("%s spent %lu usec\n", id, usec_elapsed(start, end));
    return usec_elapsed(start, end);
}

float do_merge(char *id, int *first, int n)
{
    gettimeofday(&start, NULL);

    int mid = n / 2;
    int *temp = (int *)malloc(n * sizeof(int));
    int i = 0, j = mid, k = 0;
    while (i < mid && j < n)
    {
        if (first[i] > first[j])
        {
            temp[k] = first[j];
            j++;
        }

        else
        {
            temp[k] = first[i];
            i++;
        }
        k++;
    }

    while (i < mid)
    {
        temp[k++] = first[i++];
    }
    while (j < n)
    {
        temp[k++] = first[j++];
    }

    for (int i = 0; i < n; i++)
    {
        first[i] = temp[i];
    }

    free(temp);
    gettimeofday(&end, NULL);
    return usec_elapsed(start, end);
}

void *thread_sort(void *arg)
{
    sort_args *args = (sort_args *)arg;
    args->elapsed = do_sort(args->id, args->first, args->n);
    pthread_exit(NULL);
}

void *thread_merge(void *arg2)
{
    sort_args *args = (sort_args *)arg2;
    args->elapsed = do_merge(args->id, args->first, args->n);
    pthread_exit(NULL);
}

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <sys/time.h>
#include <pthread.h>

#define THREAD_NUM 3
#define LOOP_COUNT 1000000
#define MUTEX_ON 1 

int count = 0;
int cnt_per_thread = LOOP_COUNT / THREAD_NUM;
int cnt_remain = LOOP_COUNT % THREAD_NUM;

#if MUTEX_ON
pthread_mutex_t cnt_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void *t_function()
{
	int i;

	for(i = 0; i < cnt_per_thread; i++) {
#if MUTEX_ON
			pthread_mutex_lock(&cnt_mutex);
#endif
		count++;
#if MUTEX_ON
		pthread_mutex_unlock(&cnt_mutex);
#endif
		//printf("[Thread] count = %3d, TGID = %d, TID = %d\n", count, getpid(), (int)syscall(__NR_gettid));
		//usleep(1000);
	}
}
pthread_t p_thread[THREAD_NUM];

int main(void)
{
	int i;
	int pid, status;
	struct timeval start_point, end_point;
	
	gettimeofday(&start_point, NULL);

//	printf("start_point = %f\n", (double)(start_point.tv_sec)+(double)(start_point.tv_usec)/1000000.0);


	for(i = 0; i < THREAD_NUM; i++) {
		if((pid = pthread_create(&p_thread[i], NULL, t_function, NULL) < 0)) {
			perror("thread create error");
			exit(1);
		}
	}

	for(i = 0; i < cnt_remain; i++) {
#if MUTEX_ON
		pthread_mutex_lock(&cnt_mutex);
#endif
		count++;
#if MUTEX_ON
		pthread_mutex_unlock(&cnt_mutex);
#endif
		//printf("[Thread] count = %3d, TGID = %d, TID = %d\n", count, getpid(), (int)syscall(__NR_gettid));
	}

	for(i = 0; i < THREAD_NUM; i++) {
		pthread_join(p_thread[i], (void **)&status);
	}

	gettimeofday(&end_point, NULL);

	//printf("end_point   = %f\n", (double)(end_point.tv_sec)+(double)(end_point.tv_usec)/1000000.0);
	printf("%f\n", ((double)(end_point.tv_sec)+(double)(end_point.tv_usec)/1000000.0) -
		((double)(start_point.tv_sec)+(double)(start_point.tv_usec)/1000000.0));

	printf("[Process] Final value : %d\n", count);

	return 0;
}

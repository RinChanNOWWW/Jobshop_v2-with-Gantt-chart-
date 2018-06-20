/**
* main
*
* main.c
*
* by 顾希之
* 2018-5-14
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "file.h"
#include "gene.h"

/* 见file.cpp */
extern int MachineNum[MAX_MATRIX][MAX_MATRIX];
extern int Time[MAX_MATRIX][MAX_MATRIX];
/* 见gene.cpp */
extern int best_fitness;
extern int elite;
extern int elite_chromo[Max_Length];
extern int time1;
extern int fix_machine;
extern int fix_time[20];
extern int fix_begin[20];
extern int fix_end[20];
extern int fix[20];
extern HANDLE Fix_Mutex;
clock_t start, finish;

int main(void)
{
	srand((unsigned int)time(NULL));

	//srand(clock());//产生一个计算机时间（毫秒为单位）的种子来生成随机数
	/* 计算运行整个程序需要的时间 */
	
	double t; 

	All_job job;      //记录输入的数据
	read_from_file(&job);
	int i;
	int** Population;
	int all_operation = 0;
	int j;
	start = clock();
	for (j = 1; j <= 1; j++) {
		/* 初始化种群 */
		Population = newPopulation(job, &all_operation);
		/* 进化（迭代） */

		for (i = 1; i <= ITERATE; i++) {
			Population = nextPopulation(Population, job.machine_amount, job.job_amount, all_operation);
			//printf("%d\n", best_fitness);
		}
		/* 输出计算结果 */
		//printf("%d\n", best_fitness);
		deletePopulation(Population);
	}
	output(elite_chromo, job.machine_amount, job.job_amount, all_operation);
	DWORD Fix_Thread_ID = 2;
	HANDLE Fix_Thread;
	Fix_Mutex = CreateMutex(NULL, false, NULL);
	Fix_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)input_fix, NULL, 0, &Fix_Thread_ID);
	output2(elite_chromo, job.machine_amount, job.job_amount, all_operation);
	/* 输出程序运行时间 */
	CloseHandle(Fix_Thread);
	Sleep(500);
	system("pause");
	return 0;
}

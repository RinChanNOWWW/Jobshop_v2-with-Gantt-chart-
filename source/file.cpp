/**
* def of file.h
*
* file.c
* by 何哲宇
* 2018-5-14
*/
#pragma once
#define SHOW_CONSOLE

#include<stdio.h>
#include <string.h>
#include <Windows.h>
#include <graphics.h>
#include <time.h>
#include "file.h"
#include "gene.h"

int MachineNum[MAX_MATRIX][MAX_MATRIX];       //工件i第j道工序加工所用的机器号
int Time[MAX_MATRIX][MAX_MATRIX];             //工件i第j道工序加工所用时间
extern int best_fitness;                      //最优解
int time1;
int fix_machine;
int fix_time[20];
int fix_begin[20];
int fix_end[20];
int fix[20];
int if_fix;
HANDLE Fix_Mutex;
extern clock_t start, finish;
double alltime;

/**
* 从文件（可选从键盘/stdin）中读取需要的数据
*
* job 存储所有基本数据的结构体类型
*/
void read_from_file(All_job * job)
{
	//	int opt;                            //选择类型
	FILE * fp;
	/* 基本文件操作部分 */
	fp = fopen("input.txt", "r");
	/* 读取数据部分 */
	fscanf(fp, "%d %d", &(*job).job_amount, &(*job).machine_amount);
	int i, j;
	int M, T;
	for (i = 1; i <= job->job_amount; i++) {
		for (j = 1; j <= job->machine_amount; j++) {
			fscanf(fp, "%d %d", &M, &T);
			MachineNum[i][j] = M + 1;
			Time[i][j] = T;
		}
		job->operation_amount[i - 1] = job->machine_amount;
	}
	/* 关闭文件 */
	fclose(fp);
}

/**
* 输出计算结果
*
* elite[] 最优解所对应的编码染色体
* machine 总机器数
* job 总工件数
* all_operation 总工序数
*/
void output(int * elite, int machine, int job, int all_operation)
{
	/* 基本文件操作 */
	FILE * fp;
	fp = fopen("output.txt", "w");
	/* 创建一个记录输出数据信息的二维数组 */
	Result result[MAX_MATRIX][MAX_MATRIX];         //机器i上第j道工序
												   /* 初始化结果数组 */
	int i, j;
	for (i = 0; i <= machine; i++)
		for (j = 0; j <= job; j++) {
			Result temp = { -1, -1, -1, -1 };
			result[i][j] = temp;
		}
	/* 计算结果结构体所需要的数据 */
	int machineEndTime[Max_Length];                //机器i上做完最新工序的时间
	int jobEndTime[Max_Length];                    //工件i做完最新工序的时间 
	int index[Max_Length];                         //最新工序在工件中的序号
												   /* 初始化清零 */
	memset(machineEndTime, 0, Max_Length);
	memset(jobEndTime, 0, Max_Length);
	memset(index, 0, Max_Length);
	/* 计算所需要的变量 */
	int jobNum;                                    //该工序所在工件号
	int operationNum;                              //该工序是工件的第几道工序
	int machineNum;                                //该工序所在机器号
	int begin = 0;                                     //该工序的开始时间
	int end;                                       //该工序的结束时间
												   /* 按顺序遍历编码染色体，逐工件操作 */
	for (i = 0; i < all_operation; i++) {
		jobNum = elite[i];
		index[jobNum]++;
		operationNum = index[elite[i]];
		machineNum = MachineNum[jobNum][operationNum];
		begin = machineEndTime[machineNum] > jobEndTime[jobNum] ? machineEndTime[machineNum] : jobEndTime[jobNum];
		end = begin + Time[jobNum][operationNum];
		machineEndTime[machineNum] = end;
		jobEndTime[jobNum] = end;
		j = 0;
		/* 按顺序找到该机器的结果数组中没有数据的位置 */
		while (result[machineNum][j].begin != -1)
			j++;
		/* 输入结果 */
		result[machineNum][j].begin = begin;
		result[machineNum][j].end = end;
		result[machineNum][j].job = jobNum;
		result[machineNum][j].operation = operationNum;
	}
	/* 打印结果 */
	initgraph(1300, 600);
	setcolor(GREEN);
	line(20, 445, 20, 10);
	line(10, 20, 20, 10);
	line(20, 10, 30, 20);
	line(20, 445, 1300, 445);
	line(1290, 435, 1300, 445);
	line(1290, 455, 1300, 445);
	for (i = 1; i <= machine; i++) {     //遍历每个机器的结果序列
										 /* 从有数据的位置开始 */
										 //if (result[i][0].begin == -1)
										 //continue;
		printf("M%d", i - 1);               //到屏幕
		fprintf(fp, "M%d", i - 1);          //到文件
											/* 逐列读取 */
		for (j = 0; j < job; j++) {
			if (result[i][j].begin == -1)     //到没有数据的位置结束
				break;
			printf(" (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job - 1, result[i][j].operation - 1, result[i][j].end);
			fprintf(fp, " (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job - 1, result[i][j].operation - 1, result[i][j].end);
			setfillcolor(hsl2rgb(float((result[i][j].job - 1) * 36), 1.0f, 0.5f));
			setfont(14, 0, "宋体");
			setcolor(WHITE);
			xyprintf(0, 10 * 4 * i, "M%d", i - 1);
			bar(result[i][j].begin  + 20, 10 * 4 * i, result[i][j].end  + 20, 10 * 4 * i + 20);
			setfont(10, 0, "宋体");
			setcolor(WHITE);
			xyprintf(result[i][j].begin + 20, 445, "%d", result[i][j].begin);
			xyprintf(result[i][j].end  + 20, 445, "%d", result[i][j].end);
			setfont(9, 0, "宋体");
			setcolor(BLACK);
			setbkmode(TRANSPARENT);
			xyprintf(result[i][j].begin +20, 10 * 4 * i, "%d-%d", result[i][j].job - 1, result[i][j].operation - 1);
		}
		printf("\n");
		fprintf(fp, "\n");
	}
	setcolor(WHITE);
	setfont(20, 0, "宋体");
	xyprintf(20, 500, "总时间：%d", best_fitness);
	finish = clock();
	alltime = (double)(finish - start) / 1000;
	printf("Time Used: %.3lfs\n", alltime);
	fprintf(fp, "Time Used: %.3lfs\n", alltime);
	printf("End Time: %d\n", best_fitness);
	fprintf(fp, "End Time: %d\n", best_fitness);
	fclose(fp); 
	getch();
	closegraph();
}

void input_fix()
{
	WaitForSingleObject(Fix_Mutex, INFINITE);
	while (1) {
		getchar();
		int fixtime;
	
		scanf("%d %d", &fix_machine, &fixtime);
		fix_time[fix_machine] = fixtime;
		fix[fix_machine] = 1;
		if_fix = 1;
	}
	ReleaseMutex(Fix_Mutex);
}

void output2(int * elite, int machine, int job, int all_operation)
{
	/* 基本文件操作 */
	FILE * fp;
	fp = fopen("output2.txt", "w");
	/* 创建一个记录输出数据信息的二维数组 */
	Result result[MAX_MATRIX][MAX_MATRIX];         //机器i上第j道工序
												   /* 初始化结果数组 */
	int i, j;
	for (i = 0; i <= machine; i++) 
		for (j = 0; j <= job; j++) {
			Result temp = { -1, -1, -1, -1 };
			result[i][j] = temp;
		}
	/* 计算结果结构体所需要的数据 */
	int machineEndTime[Max_Length];                //机器i上做完最新工序的时间
	int jobEndTime[Max_Length];                    //工件i做完最新工序的时间 
	int index[Max_Length];                         //最新工序在工件中的序号
												   /* 初始化清零 */
	memset(machineEndTime, 0, Max_Length);
	memset(jobEndTime, 0, Max_Length);
	memset(index, 0, Max_Length);
	/* 计算所需要的变量 */
	int jobNum;                                    //该工序所在工件号
	int operationNum;                              //该工序是工件的第几道工序
	int machineNum;                                //该工序所在机器号
	int begin;                                     //该工序的开始时间
	int end;                                       //该工序的结束时间
												   /* 按顺序遍历编码染色体，逐工件操作 */
	for (i = 0; i < all_operation; i++) {
		jobNum = elite[i];
		index[jobNum]++;
		operationNum = index[elite[i]];
		machineNum = MachineNum[jobNum][operationNum];
		begin = machineEndTime[machineNum] > jobEndTime[jobNum] ? machineEndTime[machineNum] : jobEndTime[jobNum];
		end = begin + Time[jobNum][operationNum];
		machineEndTime[machineNum] = end;
		jobEndTime[jobNum] = end;
		j = 0;
		/* 按顺序找到该机器的结果数组中没有数据的位置 */
		while (result[machineNum][j].begin != -1)
			j++;
		/* 输入结果 */
		result[machineNum][j].begin = begin;
		result[machineNum][j].end = end;
		result[machineNum][j].job = jobNum;
		result[machineNum][j].operation = operationNum;
	}
	time1 = 0;
	int flag;
	while (time1 <= best_fitness) {
		time1 = time1 + 3;
		printf("\n");
		fprintf(fp, "\n");
		printf("当前时间：%d\n", time1);
		fprintf(fp, "当前时间：%d\n", time1);
		int k;
		for (i = 1; i <= machine; i++) {

			j = 0;
			flag = 0;
			while (result[i][j].begin != -1) {
				if (result[i][j].begin <= time1 && result[i][j].end >= time1) {
					printf("M%d 加工 当前工件-工序：%d-%d 当前状态已持续时长：%d 当前状态结束时间: %d\n", i - 1, result[i][j].job - 1, result[i][j].operation - 1, time1 - result[i][j].begin, result[i][j].end);
					fprintf(fp, "M%d 加工 当前工件-工序：%d-%d 当前状态已持续时长：%d 当前状态结束时间: %d\n", i - 1, result[i][j].job - 1, result[i][j].operation - 1, time1 - result[i][j].begin, result[i][j].end);
					flag = 1;
					break;
				}
				j++;
			}
			k = 0;
			if (flag == 0) {
				if (fix[i - 1] == 2 && time1 >= fix_begin[i - 1] && time1 <= fix_end[i - 1]) {
					printf("M%d 检修\n", i - 1);
					fprintf(fp, "M%d 检修\n", i - 1);
				}
				else {                                
					printf("M%d 空闲\n", i - 1);
					fprintf(fp, "M%d 空闲\n", i - 1);
				}
			}
		}
		printf("\n");
		fprintf(fp, "\n");
		//Sleep(1000);
		
		api_sleep(3000);
		/* 检修 */
		int s;
		if (if_fix == 1) {
			best_fitness = 0;
			memset(machineEndTime, 0, Max_Length);
			memset(jobEndTime, 0, Max_Length);
			memset(index, 0, Max_Length);
			for (s = 0; s < machine; s++) {
				if (fix[s] != 0)
					fix[s] = 1;
			}
			for (int x = 0; x <= machine; x++)
				for (int y = 0; y <= job; y++) {
					Result temp = { -1, -1, -1, -1 };
					result[x][y] = temp;
				}
			fix_begin[fix_machine] = time1;
			fix_end[fix_machine] = fix_begin[fix_machine] + fix_time[fix_machine];
			for (int q = 0; q < all_operation; q++) {
				int M = 0;
				jobNum = elite[q];
				index[jobNum]++;
				operationNum = index[elite[q]];
				machineNum = MachineNum[jobNum][operationNum];
				begin = machineEndTime[machineNum] > jobEndTime[jobNum] ? machineEndTime[machineNum] : jobEndTime[jobNum];
				for (M = 0; M < machine; M++) {
					if (machineNum == M + 1 && fix[M] == 1 && begin < fix_begin[M] && begin + Time[jobNum][operationNum] > fix_begin[M]) {
						fix_begin[M] = begin + Time[jobNum][operationNum];
						fix_end[M] = fix_time[M] + fix_begin[M];
					}
					if ((machineNum == (M + 1)) && fix[M] == 1  && begin >= fix_begin[M] && fix_end[M] > jobEndTime[jobNum]) {
						begin = fix_end[M];
						fix[M] = 2;
					}
				}
				end = begin + Time[jobNum][operationNum];
				machineEndTime[machineNum] = end;
				jobEndTime[jobNum] = end;
				j = 0;
				/* 按顺序找到该机器的结果数组中没有数据的位置 */
				while (result[machineNum][j].begin != -1)
					j++;
				/* 输入结果 */
				result[machineNum][j].begin = begin;
				result[machineNum][j].end = end;
				result[machineNum][j].job = jobNum;
				result[machineNum][j].operation = operationNum;
				if (best_fitness <= end)
					best_fitness = end;
			}
			if_fix = 0;
		}
	}

	/*********************************************************************************************/
	/* 绘制检修后的甘特图 */
	initgraph(1300, 600);
	setcolor(GREEN);
	line(20, 445, 20, 10);
	line(10, 20, 20, 10);
	line(20, 10, 30, 20);
	line(20, 445, 1300, 445);
	line(1290, 435, 1300, 445);
	line(1290, 455, 1300, 445);
	for (i = 0; i <= machine; i++) {     //遍历每个机器的结果序列
										 /* 从有数据的位置开始 */
		if (result[i][0].begin == -1)
			continue;
											/* 逐列读取 */
		for (j = 0; j < job; j++) {
			if (result[i][j].begin == -1)     //到没有数据的位置结束
				break;
				
				setfont(12, 0, "宋体");
				setcolor(WHITE);
				xyprintf(0, 10 * 4 * i, "M%d", i - 1);
				if (fix[i - 1] == 2 && fix_begin[i - 1] <= result[i][j].begin) {
					setcolor(RED);
					setfillcolor(WHITE);
					bar(fix_begin[i - 1]  + 20, 10 * 4 * i, fix_end[i - 1]  + 20, 10 * 4 * i + 20);
					setfont(14, 0, "宋体");
					xyprintf(fix_begin[i - 1] /2 + fix_end[i - 1]  / 2 + 10, 10 * 4 * i, "检修");
				}
				setfillcolor(hsl2rgb(float((result[i][j].job - 1) * 36), 1.0f, 0.5f));
				bar(result[i][j].begin  + 20, 10 * 4 * i, result[i][j].end  + 20, 10 * 4 * i + 20);
				setfont(8, 0, "宋体");
				setcolor(WHITE);
				xyprintf(result[i][j].begin * 10 + 20, 445, "%d", result[i][j].begin);
				xyprintf(result[i][j].end * 10 + 20, 445, "%d", result[i][j].end);
				setfont(8, 0, "宋体");
				setcolor(BLACK); 
				setbkmode(TRANSPARENT);
				xyprintf(result[i][j].begin  + 20, 10 * 4 * i, "%d-%d", result[i][j].job - 1, result[i][j].operation - 1);
			
		}
	}
	setcolor(WHITE);
	setfont(20, 0, "宋体");
	xyprintf(20, 500, "总时间：%d",best_fitness);
	/************************************************************/
	for (i = 1; i <= machine; i++) {     //遍历每个机器的结果序列
										 /* 从有数据的位置开始 */
										 //if (result[i][0].begin == -1)
										 //continue;    C1 20    C2 10   C0 20
		printf("M%d", i - 1);               //到屏幕
		fprintf(fp, "M%d", i - 1);          //到文件
											/* 逐列读取 */
		for (j = 0; j < job; j++) {
			if (result[i][j].begin == -1)     //到没有数据的位置结束
				break;
			if (/*i == fix_machine + 1 && */result[i][j].begin >= fix_begin[i - 1] && fix[i - 1] == 2) {
				printf(" (%d,检修中,%d)", fix_begin[i - 1], fix_end[i - 1]);
				fprintf(fp, " (%d,检修中,%d)", fix_begin[i - 1], fix_end[i - 1]);
				fix[i - 1] = -1;
			}
			
			printf(" (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job - 1, result[i][j].operation - 1, result[i][j].end);
			fprintf(fp, " (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job - 1, result[i][j].operation - 1, result[i][j].end);
		}
		printf("\n");
		fprintf(fp, "\n");
	}
	/************************************************************/
	finish = clock();
	alltime = (double)(finish - start) / 1000;
	printf("Time Used: %.3lfs", alltime);
	fprintf(fp, "Time Used: %.3lfs", alltime);
	printf("End Time: %d\n", best_fitness);
	fprintf(fp, "End Time: %d\n", best_fitness);
	fclose(fp);
	getch();
	closegraph();
}

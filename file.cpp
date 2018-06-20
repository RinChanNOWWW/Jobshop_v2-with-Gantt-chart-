/**
* def of file.h
*
* file.c
* by ������
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

int MachineNum[MAX_MATRIX][MAX_MATRIX];       //����i��j������ӹ����õĻ�����
int Time[MAX_MATRIX][MAX_MATRIX];             //����i��j������ӹ�����ʱ��
extern int best_fitness;                      //���Ž�
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
* ���ļ�����ѡ�Ӽ���/stdin���ж�ȡ��Ҫ������
*
* job �洢���л������ݵĽṹ������
*/
void read_from_file(All_job * job)
{
	//	int opt;                            //ѡ������
	FILE * fp;
	/* �����ļ��������� */
	fp = fopen("input.txt", "r");
	/* ��ȡ���ݲ��� */
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
	/* �ر��ļ� */
	fclose(fp);
}

/**
* ���������
*
* elite[] ���Ž�����Ӧ�ı���Ⱦɫ��
* machine �ܻ�����
* job �ܹ�����
* all_operation �ܹ�����
*/
void output(int * elite, int machine, int job, int all_operation)
{
	/* �����ļ����� */
	FILE * fp;
	fp = fopen("output.txt", "w");
	/* ����һ����¼���������Ϣ�Ķ�ά���� */
	Result result[MAX_MATRIX][MAX_MATRIX];         //����i�ϵ�j������
												   /* ��ʼ��������� */
	int i, j;
	for (i = 0; i <= machine; i++)
		for (j = 0; j <= job; j++) {
			Result temp = { -1, -1, -1, -1 };
			result[i][j] = temp;
		}
	/* �������ṹ������Ҫ������ */
	int machineEndTime[Max_Length];                //����i���������¹����ʱ��
	int jobEndTime[Max_Length];                    //����i�������¹����ʱ�� 
	int index[Max_Length];                         //���¹����ڹ����е����
												   /* ��ʼ������ */
	memset(machineEndTime, 0, Max_Length);
	memset(jobEndTime, 0, Max_Length);
	memset(index, 0, Max_Length);
	/* ��������Ҫ�ı��� */
	int jobNum;                                    //�ù������ڹ�����
	int operationNum;                              //�ù����ǹ����ĵڼ�������
	int machineNum;                                //�ù������ڻ�����
	int begin = 0;                                     //�ù���Ŀ�ʼʱ��
	int end;                                       //�ù���Ľ���ʱ��
												   /* ��˳���������Ⱦɫ�壬�𹤼����� */
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
		/* ��˳���ҵ��û����Ľ��������û�����ݵ�λ�� */
		while (result[machineNum][j].begin != -1)
			j++;
		/* ������ */
		result[machineNum][j].begin = begin;
		result[machineNum][j].end = end;
		result[machineNum][j].job = jobNum;
		result[machineNum][j].operation = operationNum;
	}
	/* ��ӡ��� */
	initgraph(1300, 600);
	setcolor(GREEN);
	line(20, 445, 20, 10);
	line(10, 20, 20, 10);
	line(20, 10, 30, 20);
	line(20, 445, 1300, 445);
	line(1290, 435, 1300, 445);
	line(1290, 455, 1300, 445);
	for (i = 1; i <= machine; i++) {     //����ÿ�������Ľ������
										 /* �������ݵ�λ�ÿ�ʼ */
										 //if (result[i][0].begin == -1)
										 //continue;
		printf("M%d", i - 1);               //����Ļ
		fprintf(fp, "M%d", i - 1);          //���ļ�
											/* ���ж�ȡ */
		for (j = 0; j < job; j++) {
			if (result[i][j].begin == -1)     //��û�����ݵ�λ�ý���
				break;
			printf(" (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job - 1, result[i][j].operation - 1, result[i][j].end);
			fprintf(fp, " (%d,%d-%d,%d)", result[i][j].begin, result[i][j].job - 1, result[i][j].operation - 1, result[i][j].end);
			setfillcolor(hsl2rgb(float((result[i][j].job - 1) * 36), 1.0f, 0.5f));
			setfont(14, 0, "����");
			setcolor(WHITE);
			xyprintf(0, 10 * 4 * i, "M%d", i - 1);
			bar(result[i][j].begin  + 20, 10 * 4 * i, result[i][j].end  + 20, 10 * 4 * i + 20);
			setfont(10, 0, "����");
			setcolor(WHITE);
			xyprintf(result[i][j].begin + 20, 445, "%d", result[i][j].begin);
			xyprintf(result[i][j].end  + 20, 445, "%d", result[i][j].end);
			setfont(9, 0, "����");
			setcolor(BLACK);
			setbkmode(TRANSPARENT);
			xyprintf(result[i][j].begin +20, 10 * 4 * i, "%d-%d", result[i][j].job - 1, result[i][j].operation - 1);
		}
		printf("\n");
		fprintf(fp, "\n");
	}
	setcolor(WHITE);
	setfont(20, 0, "����");
	xyprintf(20, 500, "��ʱ�䣺%d", best_fitness);
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
	/* �����ļ����� */
	FILE * fp;
	fp = fopen("output2.txt", "w");
	/* ����һ����¼���������Ϣ�Ķ�ά���� */
	Result result[MAX_MATRIX][MAX_MATRIX];         //����i�ϵ�j������
												   /* ��ʼ��������� */
	int i, j;
	for (i = 0; i <= machine; i++) 
		for (j = 0; j <= job; j++) {
			Result temp = { -1, -1, -1, -1 };
			result[i][j] = temp;
		}
	/* �������ṹ������Ҫ������ */
	int machineEndTime[Max_Length];                //����i���������¹����ʱ��
	int jobEndTime[Max_Length];                    //����i�������¹����ʱ�� 
	int index[Max_Length];                         //���¹����ڹ����е����
												   /* ��ʼ������ */
	memset(machineEndTime, 0, Max_Length);
	memset(jobEndTime, 0, Max_Length);
	memset(index, 0, Max_Length);
	/* ��������Ҫ�ı��� */
	int jobNum;                                    //�ù������ڹ�����
	int operationNum;                              //�ù����ǹ����ĵڼ�������
	int machineNum;                                //�ù������ڻ�����
	int begin;                                     //�ù���Ŀ�ʼʱ��
	int end;                                       //�ù���Ľ���ʱ��
												   /* ��˳���������Ⱦɫ�壬�𹤼����� */
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
		/* ��˳���ҵ��û����Ľ��������û�����ݵ�λ�� */
		while (result[machineNum][j].begin != -1)
			j++;
		/* ������ */
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
		printf("��ǰʱ�䣺%d\n", time1);
		fprintf(fp, "��ǰʱ�䣺%d\n", time1);
		int k;
		for (i = 1; i <= machine; i++) {

			j = 0;
			flag = 0;
			while (result[i][j].begin != -1) {
				if (result[i][j].begin <= time1 && result[i][j].end >= time1) {
					printf("M%d �ӹ� ��ǰ����-����%d-%d ��ǰ״̬�ѳ���ʱ����%d ��ǰ״̬����ʱ��: %d\n", i - 1, result[i][j].job - 1, result[i][j].operation - 1, time1 - result[i][j].begin, result[i][j].end);
					fprintf(fp, "M%d �ӹ� ��ǰ����-����%d-%d ��ǰ״̬�ѳ���ʱ����%d ��ǰ״̬����ʱ��: %d\n", i - 1, result[i][j].job - 1, result[i][j].operation - 1, time1 - result[i][j].begin, result[i][j].end);
					flag = 1;
					break;
				}
				j++;
			}
			k = 0;
			if (flag == 0) {
				if (fix[i - 1] == 2 && time1 >= fix_begin[i - 1] && time1 <= fix_end[i - 1]) {
					printf("M%d ����\n", i - 1);
					fprintf(fp, "M%d ����\n", i - 1);
				}
				else {                                
					printf("M%d ����\n", i - 1);
					fprintf(fp, "M%d ����\n", i - 1);
				}
			}
		}
		printf("\n");
		fprintf(fp, "\n");
		//Sleep(1000);
		
		api_sleep(3000);
		/* ���� */
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
				/* ��˳���ҵ��û����Ľ��������û�����ݵ�λ�� */
				while (result[machineNum][j].begin != -1)
					j++;
				/* ������ */
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
	/* ���Ƽ��޺�ĸ���ͼ */
	initgraph(1300, 600);
	setcolor(GREEN);
	line(20, 445, 20, 10);
	line(10, 20, 20, 10);
	line(20, 10, 30, 20);
	line(20, 445, 1300, 445);
	line(1290, 435, 1300, 445);
	line(1290, 455, 1300, 445);
	for (i = 0; i <= machine; i++) {     //����ÿ�������Ľ������
										 /* �������ݵ�λ�ÿ�ʼ */
		if (result[i][0].begin == -1)
			continue;
											/* ���ж�ȡ */
		for (j = 0; j < job; j++) {
			if (result[i][j].begin == -1)     //��û�����ݵ�λ�ý���
				break;
				
				setfont(12, 0, "����");
				setcolor(WHITE);
				xyprintf(0, 10 * 4 * i, "M%d", i - 1);
				if (fix[i - 1] == 2 && fix_begin[i - 1] <= result[i][j].begin) {
					setcolor(RED);
					setfillcolor(WHITE);
					bar(fix_begin[i - 1]  + 20, 10 * 4 * i, fix_end[i - 1]  + 20, 10 * 4 * i + 20);
					setfont(14, 0, "����");
					xyprintf(fix_begin[i - 1] /2 + fix_end[i - 1]  / 2 + 10, 10 * 4 * i, "����");
				}
				setfillcolor(hsl2rgb(float((result[i][j].job - 1) * 36), 1.0f, 0.5f));
				bar(result[i][j].begin  + 20, 10 * 4 * i, result[i][j].end  + 20, 10 * 4 * i + 20);
				setfont(8, 0, "����");
				setcolor(WHITE);
				xyprintf(result[i][j].begin * 10 + 20, 445, "%d", result[i][j].begin);
				xyprintf(result[i][j].end * 10 + 20, 445, "%d", result[i][j].end);
				setfont(8, 0, "����");
				setcolor(BLACK); 
				setbkmode(TRANSPARENT);
				xyprintf(result[i][j].begin  + 20, 10 * 4 * i, "%d-%d", result[i][j].job - 1, result[i][j].operation - 1);
			
		}
	}
	setcolor(WHITE);
	setfont(20, 0, "����");
	xyprintf(20, 500, "��ʱ�䣺%d",best_fitness);
	/************************************************************/
	for (i = 1; i <= machine; i++) {     //����ÿ�������Ľ������
										 /* �������ݵ�λ�ÿ�ʼ */
										 //if (result[i][0].begin == -1)
										 //continue;    C1 20    C2 10   C0 20
		printf("M%d", i - 1);               //����Ļ
		fprintf(fp, "M%d", i - 1);          //���ļ�
											/* ���ж�ȡ */
		for (j = 0; j < job; j++) {
			if (result[i][j].begin == -1)     //��û�����ݵ�λ�ý���
				break;
			if (/*i == fix_machine + 1 && */result[i][j].begin >= fix_begin[i - 1] && fix[i - 1] == 2) {
				printf(" (%d,������,%d)", fix_begin[i - 1], fix_end[i - 1]);
				fprintf(fp, " (%d,������,%d)", fix_begin[i - 1], fix_end[i - 1]);
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

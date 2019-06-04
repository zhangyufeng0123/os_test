#include<stdio.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>

//定义进程
struct pcb{
	int xyTime;			//响应时间
	int order;      		//输入的顺序
	char name[10];  		//进程名称
	int ComeTime;   		//到达时间
	int ServerTime; 		//服务时间
	int tmp;        		//代替服务时间
	int FinishTime; 		//结束时间
	int TurnoverTime;       	//周转时间
	double WeightTurnaroundTime;    //带权周转时间
	char process_order[10];		//目前进程的状态
	int WaitTime;			//等待时间
};


//定义初始队列
struct queue {
	int firstTime;
	int secondTime;
};

const int maxn = 150;
struct queue process_test;      //定义队列
struct pcb process[maxn];
int process_sum;        //进程数量
int mapp[2000][maxn];   //甘特图
char process_first[3] = "Q1", process_second[3] = "Q2", process_rr[3] = "RR";   //定义进程状态
char process_end[4] = "END";
int num = 0;    //定义一个时间进程
int times = 0;	//定义实在第几个轮转片中

//所有的函数
void queue_init();		//初始化反馈队列
void init();			//初始化进程,随机生成进程的内容
void sort_comeTime();		//对进程进行排序。关键词-‘到达时间’
int compare(int a, int b);	//将两个数进行比较，取较小值
void process_first_begin();	//在第一个时间片内
void process_second_begin();	//在第二个时间片内
void process_rr_FCFS();		//在最后一个时间片内
void output_init_information();	//输出进程的初始信息
void output_graph();		//输出甘特图
void calculateTime();		//计算周转时间以及带权周转事件
void output_final_information();//输出最后的信息以及结果
void output_after_queue();	//每次轮转完之后输出当前所有进程的状态
void calcluateWaitTime();	//计算等待时间

//初始化反馈队列
void queue_init() {
	//根据题目要求将前两个反馈队列
	process_test.firstTime = 2;
	process_test.secondTime = 4;
}

//随机生成进程的数据
void init() {
	srand((unsigned)time(NULL));
	process_sum = rand() % 5 + 5;
	//process_sum = 1;
	//cout << "进程的总数为" << process_sum << endl;
	printf("进程的总数为%d\n", process_sum);
	int comeTime, serverTime, finishTime;
	for (int i = 0; i < process_sum; i++) {
		srand((unsigned )time(NULL));
		comeTime = rand() % 10; //随机生成到达时间
		serverTime = rand() % 10;       //随机生成服务时间
		finishTime = 0;
		//cout << "请输入进程" << i+1 << "的名称" << endl;
		printf("请输入进程%d的名称：\t", i + 1);
		fflush(stdout);
		scanf("%s", process[i].name);
		printf("进程%s的到达时间为%d\n", process[i].name, comeTime);
		//cout << "进程" << process[i].name << "的到达时间为" << comeTime << endl;
		printf("进程%s的服务时间为%d\n", process[i].name, serverTime);
		//cout << "进程" << process[i].name << "的服务时间为" <<serverTime << endl;
		process[i].order = i + 1;
		//process[i].ComeTime = 10;
		process[i].ComeTime = comeTime;
		//process[i].ServerTime = 5;
		process[i].ServerTime = serverTime;
		process[i].tmp = serverTime;
		process[i].xyTime = 0;
		process[i].FinishTime = finishTime;
		if (process[i].ServerTime == 0) {
			strcpy(process[i].process_order, process_end);
			process[i].FinishTime = process[i].ComeTime;
		} else {
			strcpy(process[i].process_order, process_first);
		}
	}
}

/*
//根据到达时间进行排序
bool cmp1(struct pcb & a, struct pcb & b){
    if(a.ComeTime == b.ComeTime){
    	return a.ServerTime < b.ServerTime;
    }
    return a.ComeTime < b.ComeTime;
}

//根据输入的顺序进行排序
bool cmp2(struct pcb & a, struct pcb & b){
	return a.order < b.order;
}
*/

//对进程进行排序
void sort_comeTime() {
	struct pcb tmp;
	for (int i = 0; i < process_sum; i++) {
		for (int j = i + 1; j < process_sum; j++) {
			 if (process[i].ComeTime > process[j].ComeTime) {
				 tmp = process[i];
				 process[i] = process[j];
				 process[j] = tmp;
			 }
			 else if(process[i].ComeTime == process[j].ComeTime) {
				 if(process[i].ServerTime  > process[j].ServerTime){
					 tmp = process[i];
					 process[i] = process[j];
					 process[j] = tmp;
				 }
			 }
		}
	}
}

//取较小值
int compare(int a, int b) {
	return a > b ? b : a;
}

//在第一个时间片内
void process_first_begin() {
	num = process[0].ComeTime;
	for (int i = 0; i < process_sum; i++) {
		/*
		 if(process[i].process_order != process_first){
		 	continue;
		}*/
		if (!(process[i].process_order[0] == 'Q' && process[i].process_order[1] == '1')){
			continue;
		}
		//当前进程的到达时间大于num的时候把当前进程的事件赋值给num
		if (num < process[i].ComeTime) {
			num = process[i].ComeTime;
		}
		
		if(i != 0){
			if(process[i].ComeTime == process[i-1].ComeTime){
				if(process[i - 1].tmp != 0){
					process[i].xyTime = process[i-1].xyTime + 2;
				}
			}
		}

		int minn;
		minn = compare(process_test.firstTime, process[i].ServerTime);
		//int minn = min(process_test.firstTime, process[i].ServerTime);
		for (int j = num; j < num + minn; j++) {
			 mapp[j][process[i].order] = 1;
		}
		process[i].ServerTime -= minn;
		if (process[i].ServerTime == 0) {
			strcpy(process[i].process_order, process_end);
			process[i].FinishTime = num + minn;
		} else {
			strcpy(process[i].process_order, process_second);
		}
		num += minn;
	}
}

//在第二个时间片内
void process_second_begin() {
	for (int i = 0; i < process_sum; i++) {
		/*if(process[i].process_order != process_second){
		 	continue;
		}*/
		if (!(process[i].process_order[0] == 'Q' && process[i].process_order[1] == '2')) {
			continue;
		}

		int minn = compare(process_test.secondTime, process[i].ServerTime);
		//int minn = min(process_test.secondTime, process[i].ServerTime);
		for (int j = num; j < num + minn; j++) {
			mapp[j][process[i].order] = 1;
		}
		process[i].ServerTime -= minn;
		if (process[i].ServerTime == 0) {
			strcpy(process[i].process_order, process_end);
			process[i].FinishTime = num + minn;
		} else {
			strcpy(process[i].process_order, process_rr);
		}
		num += minn;
	}
}

//最后一个时间片内
void process_rr_FCFS() {
	for (int i = 0; i < process_sum; i++) {
		if(!(process[i].process_order[0] == 'R')){
			 continue;
		}
		/*
		 if (process[i].process_order != process_rr) {
		 	continue;
		}
		*/
		for (int j = num; j < num + process[i].ServerTime; j++) {
			mapp[j][process[i].order] = 1;
		}
		num += process[i].ServerTime;
		process[i].FinishTime = num;
	}
}

//输出进程的初始信息
void output_init_information() {
	for(int i = 0; i < 3; i++){
		printf("\n");
	}
	printf("进程名称\t进程到达时间\t进程服务时间\n");
	//cout << "进程名称\t进程到达时间\t进程服务时间" << endl;
	for (int i = 0; i < process_sum; i++) {
		printf("%s\t%9d\t%9d\n", process[i].name, process[i].ComeTime, process[i].ServerTime);
		//cout << process[i].name << '\t' << process[i].ComeTime << "\t" << process[i].ServerTime << endl;
	}

	for (int i = 0; i < 5; i++) {
		printf("\n");
		//cout << endl;
	}
}

//输出甘特图
void output_graph() {
	//方块■█▇█▆▅■▇▇
	//cout << "进程名称\t";
	printf("进程名称");
	for (int i = 0; i < num + 5; i++) {
		if (i < 10) {
			//cout << "00" << i << ' ';
			printf("00%d ", i);
		} else if (i < 100) {
			//cout << "0" << i << ' ';
			printf("0%d ", i);
		} else {
			//cout << i << ' ';
			printf("%d ", i);
		}
	}
	printf("\n");
	//cout << endl;
	for (int i = 0; i < process_sum; i++) {
		printf("%s    \t", process[i].name);
		if (process[i].tmp == 0) {
			printf("\n");
			//cout << endl;
			continue;
		}
		for (int j = 0; j < num + 5; j++) {
			if (mapp[j][process[i].order] == 1) {
				//cout << "████";
				//printf("█1");
				printf("██");
			} else {
				//cout << "    ";
				printf("    ");
			}
		}
		//cout << endl;
		printf("\n");
		//printf("%d", i+1);
		//printf("%s\n", process[i].name);
	}

	for(int i = 0; i < 5; i++){
		printf("\n");
	}
}

//计算周转时间以及带权周转时间
void calculateTime() {
	for (int i = 0; i < process_sum; i++) {
		if (process[i].tmp == 0) {
			process[i].TurnoverTime = 0;
			continue;
		}
		process[i].TurnoverTime = process[i].FinishTime - process[i].ComeTime;
	}
	for (int i = 0; i < process_sum; i++) {
		if (process[i].tmp == 0) {
			process[i].TurnoverTime = 0;
		}
		process[i].WeightTurnaroundTime = (double) process[i].TurnoverTime / process[i].tmp;
	}
}

//输出最后进程的信息以及结果
void output_final_information() {
	printf("进程名称\t进程到达时间\t进程服务时间\t进程结束时间\t进程响应时间\t进程等待时间\t进程周转时间\t进程带权周转时间\n");
	for(int i = 0; i < process_sum; i++){
		if(process[i].tmp == 0){
			printf("%-10s\t%-9d\t%-9d\t%-9d\t\n", process[i].name, process[i].ComeTime, process[i].tmp, process[i].FinishTime);
			//printf("%s\n", process[i].name);
			continue;
		}
		printf("%-10s\t%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%-18lf\n", process[i].name, process[i].ComeTime, process[i].tmp, process[i].FinishTime, process[i].xyTime, process[i].WaitTime, process[i].TurnoverTime, process[i].WeightTurnaroundTime);
	}
}

//每次结束一个时间片的时候就将当前所有的进程的状态输出一边
void output_after_queue(){
	printf("这是第%d个轮转之后进程的状态\n", ++times);
	printf("进程名称\t进程状态\n");
	for(int i = 0; i < process_sum; i++){
		printf("%s%18s\n", process[i].name, process[i].process_order);
	}
	for(int i = 0; i < 3; i++){
		printf("\n");
	}
}

//计算等待时间
void calculateWaitTime(){
	for(int i = 0; i < process_sum; i++){
		process[i].WaitTime = process[i]. FinishTime - process[i].ComeTime - process[i].tmp;
	}
}

int main(void) {
	memset(mapp, 0, sizeof(mapp));
	//queue_init();
	printf("这是多级反馈队列!\n");
	init();
	queue_init();
	output_init_information();
	//sort(process, process + process_sum, cmp1);     //根据到达时间进行排序
	sort_comeTime();
	process_first_begin();
	output_after_queue();
	process_second_begin();
	output_after_queue();
	//sort(process, process + process_sum, cmp2);   //根据输入顺序进行排序
	
	process_rr_FCFS();
	output_graph();
	calculateTime();
	calculateWaitTime();
	output_final_information();
}

/***************************************************************
 * 版权所有 (C) 2023, fu。保留所有权利。
 *
 * 文件名称：Parking management system.c
 * 摘    要：这是一个停车场管理系统，实现停车场管理及计费（每停车1个小时收费10块钱）。
 * 设计功能：
 * 			（1）车辆进入（判断有没有空车位，如果有，允许车辆进入，同时记录车位号，车牌号及辆进入时间）。
 * 			（2）车辆离开（按照车牌号或者车位号查找车辆，如果在车位，计费，同时让车辆离开，并将费用汇总）。
 * 			（3）查找空车位并显示车位号。
 * 			（4）查找车辆并显示车辆信息。
 * 			（5）显示所有在位车辆信息。
 * 			（6）显示缴费总金额。
 * 			（7）退出系统。
 * 			（8）设计一个菜单，至少具有上述操作要求的基本功能。
 *
 * 当前版本：1.2
 * 作    者：符文政，李志帆，于双淼
 * 完成日期：2023年5月1日
 ****************************************************************/


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <conio.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>

#define MAX_POSITION 20
#define HOURLY_RATE 10


// 定义车辆信息结构体
typedef struct car {
	wchar_t plate_num[10];  // 车牌号
	int position;        // 车位号
	time_t in_time;      // 进入时间
	time_t out_time;     // 离开时间
	double fee;          // 计费
	struct car* next;    // 指向下一辆车的指针
} Car;

// 初始化全局变量
Car* head = NULL;  // 头指针，指向第一辆车
Car* tail = NULL;  // 尾指针

double total_fee = 0; 

// 声明函数
void print_menu();
void add_car();
void remove_car_by_plate_num();
void remove_car_by_position();
void check_empty_positions();
void check_car_by_plate_num();
void check_all_cars();
void calculate_total_fee();
void write_file(Car* head, const char* filename);
Car* read_file(const char* filename);
void write_total_fee_to_file(const char* filename, float total_fee);
float read_total_fee_from_file(const char* filename);
void loading();
void SetSize(unsigned uCol,unsigned uLine);
void printchars();
void start();
void start_on();
void full();
void generate_report(Car* head, double total_fee);
void reset_list(Car** head);
int check_plate_num(wchar_t* plate_num);
int is_chinese_char(wchar_t c);
void flash ();
void fee_print(Car* p , double seconds);
void time_print(double seconds);

int main() {
	// 设置当前环境为中文UTF-8编码的locale
	_wsetlocale(LC_ALL, L"zh-CN.utf8");
	start_on();
	while (1) {
		print_menu();
		char choice = _getch();
		fflush(stdin);  // 清空输入缓冲区
		printf("\n");
		switch (choice) {
		case '1':
			add_car();
			break;
		case '2':
			remove_car_by_plate_num();
			break;
		case '3':
			remove_car_by_position();
			break;
		case '4':
			check_empty_positions();
			break;
		case '5':
			check_car_by_plate_num();
			break;
		case '6':
			check_all_cars();
			break;
		case '7':
			calculate_total_fee();
			break;
		case '8':
			generate_report(head, total_fee);
			break;
		case '9':
			write_file(head , "data.fuwenzheng");
			write_total_fee_to_file("fee.fuwenzheng", total_fee);
			printf("欢迎下次使用");
			getchar();
			return 0;
		case '0':
			reset_list(&head);
			break;
		default:
			printf("\n请输入合法的选项！\n");
		}
		printf("请按下任意键继续...");
		_getch();   // 调用 _getch() 函数等待用户输入任意键
		system("cls"); // 清屏
	}
	return 0;
}

// 打印菜单
void print_menu() {	
	printf("\n***欢迎使用停车场管理系统***\n");
	printf("1. 车辆进入\n");
	printf("2. 车辆离开(根据车牌号查找)\n");
	printf("3. 车辆离开(根据车位号查找)\n");
	printf("4. 查看空车位\n");
	printf("5. 查找车辆信息\n");
	printf("6. 查看所有在位车辆信息\n");
	printf("7. 显示缴费总金额\n");
	printf("8. 生成报表\n");
	printf("9. 退出系统\n");
	printf("0. 重置系统\n");
	printf("请选择操作（输入选项前的数字）：");
}

// 添加车辆
void add_car() {
	// 检查是否有空车位
	Car* p = head;
	while (p != NULL && p->position != MAX_POSITION) {
		p = p->next;
	}
	if (p != NULL && p->position == MAX_POSITION) {
		printf("对不起，停车场已满！\n");
		return;
	}
	
	int position;
	check_empty_positions();
	
	car_add:
	
	while (1) {
		printf("请输入要停放的车位号(输入0返回主菜单)：");
		int ret = scanf("%d", &position);
		flash ();		
		if(position == 0)
			return;
		if (ret != 1) {
			printf("输入错误，请重新输入！\n");
			// 清空输入缓冲区
			while (getchar() != '\n') continue;
		} else if (position < 1 || position > MAX_POSITION) {
			printf("输入的车位号不合法，请重新输入！\n");
		} else {
			break;
		}
	}
	
	// 检查所选车位号是否已经有车辆停放
	p = head;
	while (p != NULL) {
		if (p->position == position) {
			printf("该车位上已经有车辆停放了，请选择其他车位！\n");
			goto car_add;
		}
		p = p->next;
	}
	
	Car* new_car = (Car*)malloc(sizeof(Car));
	if (new_car == NULL) { // 检查内存分配是否成功
		perror("内存分配失败"); // 输出系统返回的错误信息
		printf("创建新车辆失败\n");
		return;
	}
	
	printf("请输入车牌号（中国普通轿车车牌号规范）:");
	wscanf(L"%ls", new_car->plate_num);
	flash ();
	
	// 判断车牌号是否合法
	if(check_plate_num(new_car->plate_num)!=0)
		return;
	
	// 检查车牌号是否重复
	p = head;
	while (p != NULL) {
		if (wcscmp(p->plate_num, new_car->plate_num) == 0) {
			printf("该车已经停在停车场了！\n");
			free(new_car);
			goto car_add;
		}
		p = p->next;
	}
	
	time(&new_car->in_time);
	
	// 插入指定车位号的链表中
	new_car->position = position;
	if (head == NULL || head->position > position) {
		printf("找到空车位：%d号车位\n", position);
		new_car->next = head;
		head = new_car;
	} else {
		p = head;
		while (p->next != NULL && p->next->position < position) {
			p = p->next;
		}
		printf("找到空车位：%d号车位\n", position);
		new_car->next = p->next;
		p->next = new_car;
	}
	printf("车辆：%ls 已经停在 %d 号车位！\n", new_car->plate_num, new_car->position);
}

// 根据车牌号删除车辆
void remove_car_by_plate_num() {
	if (head == NULL) {
		printf("停车场没有车辆！\n");
		return;
	}
	wchar_t plate_num[10];
	printf("请输入要查找的车牌号:");
	scanf("%9ls", plate_num);
	flash ();
	
	// 判断车牌号是否合法
	if(check_plate_num(plate_num)!=0)
		return;
	
	Car* p = head;
	Car* prev = NULL;
	while (p != NULL) {
		if (wcscmp(p->plate_num, plate_num) == 0) {
			time(&p->out_time);
			double seconds = difftime(p->out_time, p->in_time);
			p->fee = seconds / 3600 * HOURLY_RATE;
			
			fee_print(p , seconds);
			
			total_fee += p->fee;
			if (prev == NULL) {
				head = p->next;
			}
			else {
				prev->next = p->next;
			}
			free(p);
			return;
		}
		prev = p;
		p = p->next;
	}
	printf("未找到车牌号为%ls的车辆！\n", plate_num);
}

// 根据车位号删除车辆
void remove_car_by_position() {
	if (head == NULL) {
		printf("停车场没有车辆！\n");
		return;
	}
	int position;
	printf("请输入要查找的车位号:");
	
	int ret = scanf("%d", &position);
	flash ();
	
	//判断车位号是否合法
	if (ret != 1) {
		if (ret == EOF) {
			printf("输入错误或输入结束\n");
		} else {
			printf("输入错误，请重新输入！\n");
		}
		while ((ret=getchar()) != '\n' && ret != EOF);
		return;
	}
	if (position <= 0 || position > MAX_POSITION) { // 如果用户输入车位号不合法
		if (position <= 0) {
			printf("输入的车位号小于等于 0，请重新输入！\n");
			return;
		} else if (position > MAX_POSITION) {
			printf("输入的车位号超过了最大值，请重新输入！最大值为 %d\n", MAX_POSITION);
			return;
		}
	}
	
	Car* p = head;
	Car* prev = NULL;
	while (p != NULL) {
		if (p->position == position) {
			time(&p->out_time);
			double seconds = difftime(p->out_time, p->in_time);
			p->fee = seconds / 3600 * HOURLY_RATE;
			
			fee_print(p , seconds);
			
			total_fee += p->fee;
			if (prev == NULL) {
				head = p->next;
			} else {
				prev->next = p->next;
			}
			free(p);
			return;
		}
		prev = p;
		p = p->next;
	}
	printf("未找到车位号为%d的车辆！\n", position);
}

// 查找空车位并显示车位号
void check_empty_positions() {
	if (head == NULL) {
		if (MAX_POSITION == 0) {
			printf("停车场已满！\n");
		} else {
			printf("停车场空闲车位：\n");
			for (int i = 1; i <= MAX_POSITION; i++) {
				printf("%d ", i);
			}
			printf("\n");
		}
		return;
	}
	Car* p = head;
	int pos = 1;
	printf("停车场空闲车位：\n");
	while (p != NULL) {
		while (pos < p->position) {
			printf("%d ", pos);
			pos++;
		}
		pos++;
		p = p->next;
	}
	while (pos <= MAX_POSITION) {
		printf("%d ", pos);
		pos++;
	}
	printf("\n");
}

// 根据车牌号查找车辆并显示车辆信息
void check_car_by_plate_num() {
	if (head == NULL) {
		printf("停车场没有车辆信息！\n");
		return;
	}
	wchar_t plate_num[10];
	printf("请输入要查找的车牌号:");
	scanf("%ls", plate_num);
	flash ();
	
	// 判断车牌号是否合法
	if(check_plate_num(plate_num)!=0)
		return;
	
	Car* p = head;
	while (p != NULL) {
		if (wcscmp(p->plate_num, plate_num) == 0) {
			printf("车辆信息如下：\n");
			printf("车牌号：%ls\n", p->plate_num);
			printf("车位号：%d\n", p->position);
			printf("停车时间：%s", ctime(&p->in_time));
			if (p->out_time == 0) {
				printf("离开时间：%s", ctime(&p->out_time));
				printf("停车费用：%.2lf元\n", p->fee);
			} else {
				//车辆还在停车场
				time_t cur_time;
				time(&cur_time);
				double seconds = difftime(cur_time, p->in_time);
				double fee = seconds / 3600 * HOURLY_RATE;
				
				time_print(seconds);
				
				printf("当前停车费用：%.2lf元\n", fee);
			}
			return;
		}
		p = p->next;
	}
	printf("未找到车牌号为%ls的车辆！\n", plate_num);
}

// 显示所有在位车辆信息
void check_all_cars() {
	if (head == NULL) {
		printf("停车场没有车辆信息！\n");
		return;
	}
	printf("所有在位车辆信息如下：\n");
	Car* p = head;
	while (p != NULL) {
		printf("车牌号：%ls\n", p->plate_num);
		printf("车位号：%d\n", p->position);
		printf("停车时间：%s", ctime(&p->in_time));
		time_t cur_time;
		time(&cur_time);
		double seconds = difftime(cur_time, p->in_time);
		double fee = seconds / 3600 * HOURLY_RATE;
		
		time_print(seconds);
		
		printf("当前停车费用：%.2lf元\n", fee);
		printf("\n");
		p = p->next;
	}
}

// 计算缴费总金额
void calculate_total_fee() {
	printf("缴费总金额为：%.2lf元\n", total_fee);
}

// 打开文件并写入结构体链表
void write_file(Car* head, const char* filename) {
	FILE* fp = fopen(filename, "wb");
	if (fp == NULL) {
		perror("打开文件失败"); // 输出系统返回的错误信息
		printf("无法打开文件：%s\n", filename);
		return;
	}
	Car* p = head;
	while (p != NULL) {
		if (fwrite(p, sizeof(Car), 1, fp) != 1) { // 检查是否写入成功
			perror("写入文件失败"); // 输出系统返回的错误信息
			printf("向文件 %s 写入数据失败\n", filename);
			fclose(fp);
			return;
		}
		p = p->next;
	}
	if (ferror(fp)) { // 判断是否在写入文件时出现了错误
		perror("写入文件失败"); // 输出系统返回的错误信息
		printf("向文件 %s 写入数据失败\n", filename);
		fclose(fp);
		return;
	}
	fclose(fp);
}

// 从文件中读取结构体链表
Car* read_file(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {
		// 创建新文件并写入空结构体链表
		fp = fopen(filename, "wb");
		if (fp == NULL) {
			perror("创建文件失败"); // 输出系统返回的错误信息
			printf("无法创建文件：%s\n", filename);
			return NULL;
		}
		fclose(fp);
		// 重新打开文件
		fp = fopen(filename, "rb");
		if (fp == NULL) {
			perror("打开文件失败"); // 输出系统返回的错误信息
			printf("无法打开文件：%s\n", filename);
			return NULL;
		}
	}
	// 读取数据
	Car empty_car = {L"", 0, 0, 0, 0.0, NULL};
	Car* head = NULL;
	Car* tail = NULL;
	while (fread(&empty_car, sizeof(Car), 1, fp) > 0) {
		if (head == NULL) {
			head = (Car*) malloc(sizeof(Car));
			if (head == NULL) {
				perror("分配内存失败"); // 输出系统返回的错误信息
				printf("分配内存失败，无法读取文件中剩余的数据：%s\n", filename);
				return NULL;
			}
			memcpy(head, &empty_car, sizeof(Car));
			head->next = NULL;
			tail = head;
		} else {
			tail->next = (Car*) malloc(sizeof(Car));
			if (tail->next == NULL) {
				perror("分配内存失败"); // 输出系统返回的错误信息
				printf("分配内存失败，无法读取文件中剩余的数据：%s\n", filename);
				return NULL;
			}
			memcpy(tail->next, &empty_car, sizeof(Car));
			tail->next->next = NULL;
			tail = tail->next;
		}
	}
	if (ferror(fp)) { // 判断是否读取文件时发生了错误
		perror("读取文件失败"); // 输出系统返回的错误信息
		printf("从文件 %s 中读取数据失败\n", filename);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return head;
}

// 写total_fee
void write_total_fee_to_file(const char* filename, float total_fee) {
	FILE* fp = fopen(filename, "wb");
	if (fp == NULL) {
		perror("无法创建新文件"); // 输出系统报错信息
		printf("无法创建新文件：%s\n", filename);
		return;
	}
	size_t num_written = fwrite(&total_fee, sizeof(float), 1, fp); // 写入数据并检查是否成功写入
	if (num_written != 1) {
		perror("写入数据失败"); // 输出系统报错信息
		printf("写入数据失败，将会删除文件：%s\n", filename);
		fclose(fp);
		remove(filename); // 删除文件
		return;
	}
	fclose(fp);
}

//读total_fee
float read_total_fee_from_file(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {
		// 创建新文件并写入空数据
		fp = fopen(filename, "wb");
		if (fp == NULL) {
			perror("无法创建新文件"); // 输出系统报错信息
			printf("无法创建新文件：%s\n", filename);
			return -1.0;  // 返回 -1 表示出错
		}
		fclose(fp);
		return 0.0;  // 返回 0 代表文件为空
	}
	// 读取数据
	float total_fee;
	size_t num_read = fread(&total_fee, sizeof(float), 1, fp);
	if (num_read != 1) {
		if (feof(fp)) { // 判断是否是因为到达文件结尾而无法读取数据
			printf("已到达文件结尾，文件 %s 数据为空\n", filename);
		} else {
			perror("从文件中读取数据失败"); // 输出系统报错信息
			printf("从文件 %s 中读取数据失败\n", filename);
		}
		fclose(fp);
		return -1.0;  // 返回 -1 表示出错
	}
	fclose(fp);
	return total_fee;
}

// 加载
void loading() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		fprintf(stderr, "获取控制台窗口句柄和缓冲区信息失败\n");
		return;
	}
	
	COORD position = { 30, 20 };
	if (!SetConsoleCursorPosition(hConsole, position)) {
		fprintf(stderr, "设置光标位置失败\n");
		return;
	}
	if (!SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | BACKGROUND_GREEN)) {
		fprintf(stderr, "设置文本属性失败\n");
		return;
	}
	
	printf("加载中");
	
	int i = 0 ; 
	
	time_t start_time = time(NULL);
	while (i++<5) {
		printf(".");
		Sleep(500);
		time_t current_time = time(NULL);
		if (current_time - start_time > 10) { 
			fprintf(stderr, "等待超时，请检查系统资源\n");
			return;
		}
		if (_kbhit()) { 
			break;
		}
	}
	printf("\n加载完成!\n");
	Sleep(500);
	
	if (!SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition)) {
		fprintf(stderr, "恢复光标位置失败\n");
		return;
	}
	if (!SetConsoleTextAttribute(hConsole, csbi.wAttributes)) {
		fprintf(stderr, "恢复文本属性失败\n");
		return;
	}
	system("cls");
	
	HWND hWnd = GetConsoleWindow(); 
	if (!hWnd) {
		fprintf(stderr, "获取控制台窗口句柄失败\n");
		return;
	}
	if (!SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE)) {
		fprintf(stderr, "设置窗口位置失败\n");
		return;
	}
	// SetSize(500, 60); 
	
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); 
	if (!hOut) {
		fprintf(stderr, "获取标准输出流句柄失败\n");
		return;
	}
	CONSOLE_FONT_INFOEX font; 
	font.cbSize = sizeof(font);
	if (!GetCurrentConsoleFontEx(hOut, FALSE, &font)) {
		fprintf(stderr, "获取当前字体信息失败\n");
		return;
	}
	font.dwFontSize.X = 10; 
	font.dwFontSize.Y = 30; 
	if (!SetCurrentConsoleFontEx(hOut, FALSE, &font)) {
		fprintf(stderr, "设置字体大小失败\n");
		return;
	}
}

// 指定控制台窗口的大小 ，代表了控制台的长度是ucol个空格,宽度是uline个回车
void SetSize(unsigned uCol, unsigned uLine) {
	if (uCol <= 0 || uLine <= 0) {
		fprintf(stderr, "错误：列数和行数必须大于 0\n");
		return;
	}
	
	char cmd[64];
	sprintf(cmd, "mode con cols=%u lines=%u", uCol, uLine);
	int ret = system(cmd);
	if (ret != 0) {
		fprintf(stderr, "错误：设置控制台窗口大小失败\n");
	}
}

// 字符图
void printchars() {
	printf("　　　＃＃　　　＃＃＃　　　　　＃　　　　　　　＃＃＃　　　　　　＃　　　＃＃＃　＃＃＃＃＃＃＃　　＃　　　＃＃＃　　　＃＃＃　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　＃＃＃＃＃＃＃　　＃　　　　＃＃　　　＃＃＃　　　　＃\n");
	printf("　　　＃＃＃＃＃＃＃＃＃＃＃　　＃　　＃＃＃　　＃＃　　　　＃　　＃　　　＃＃　　＃＃＃＃＃＃＃　　＃　　＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃　＃　　　＃＃＃＃＃＃＃＃＃＃　＃　　＃＃＃＃＃＃＃＃＃＃＃＃　　＃　　　＃＃＃　　　　＃＃　　　　＃\n");
	printf("　　＃＃＃　　　　　　　　　　　＃　　＃＃＃＃＃＃＃＃＃＃＃＃　　＃　　　＃＃　　　　　＃＃＃＃　　＃　＃＃＃＃＃＃＃＃＃＃＃＃＃　　＃　＃＃＃＃＃　＃　＃＃　＃＃＃　＃　　＃＃＃　＃＃＃　＃＃　　　　＃　　　＃＃＃＃＃＃＃＃＃＃＃＃　＃\n");
	printf("　　＃＃＃　＃＃＃＃＃＃＃　　　＃　　＃＃＃＃＃＃＃＃＃＃＃＃　　＃　　　＃＃　　　　＃＃＃　　　　＃　　＃＃　＃＃＃＃＃　　＃＃　　＃　　　＃＃　　＃　＃＃　＃＃　　＃　　　　＃＃＃＃　＃＃＃＃　　　＃　　＃＃＃＃＃＃　＃＃　　　　　＃\n");
	printf("　＃＃＃＃　＃　　　　　＃　　　＃　　　　　＃＃　＃＃＃　　　　　＃　＃＃＃＃＃＃＃＃＃＃　　　　　＃　＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃　　　＃＃　　＃＃＃＃＃＃＃　　＃　　　＃＃＃＃＃＃＃＃　　　　　＃　＃＃＃＃＃＃　＃＃＃　＃　　　＃\n");
	printf("　＃＃＃＃　＃＃＃＃＃＃＃　　　＃　　　＃＃＃＃　＃＃　　　＃　　＃　　　＃＃　　＃＃＃＃＃＃＃＃＃＃　＃＃＃　　　　　　　　　＃＃　＃　＃　＃＃＃　＃　＃＃　＃＃　　＃　　　＃＃＃＃＃＃＃＃＃＃　　　＃　　＃＃＃＃＃　＃＃　＃＃＃　　＃\n");
	printf("　＃＃＃＃　　　　　　　　　　　＃　　　＃＃＃＃＃＃＃＃＃＃＃＃　＃　　　＃＃　　＃＃＃＃＃＃　＃＃＃　＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃　＃＃＃＃＃　＃　＃＃　＃＃＃　＃　　　＃＃＃＃＃　　＃＃＃＃　　＃　　　＃＃＃　＃＃＃＃＃＃＃＃　＃\n");
	printf("　　　＃＃＃＃＃＃＃＃＃＃＃＃　＃　　　＃＃＃＃＃＃＃＃＃＃＃＃　＃　　　＃＃　　　＃＃＃＃＃＃＃＃＃　　　　＃＃　　　　　＃＃　　　＃　　　＃＃　＃＃＃＃＃＃＃＃＃　＃　　＃＃＃＃＃＃＃＃＃＃＃＃＃　＃　　　＃＃　　＃＃＃＃＃＃＃＃　＃\n");
	printf("　　　＃＃＃＃　　　　＃＃　＃　＃　　＃　　　　　＃＃　　　　＃＃＃　　　＃＃＃＃＃＃＃＃＃＃＃＃＃＃　　　　＃＃＃＃＃＃＃＃＃　　　＃　　　＃＃　　　　＃＃　　　　　＃　　　＃＃＃＃＃＃＃　　　＃＃　＃　　＃＃＃＃＃＃＃＃＃＃＃＃＃　＃\n");
	printf("　　　＃＃　＃＃＃＃＃＃＃　　　＃　　＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃　　　＃＃＃＃＃＃＃＃＃　＃＃　＃　　　　＃＃　　　　　　　　　　＃　　　＃＃＃＃＃＃＃＃＃＃＃＃　＃　　　＃＃＃　　＃＃＃＃＃　　　＃　　＃＃＃＃　　＃＃　＃＃　　　＃\n");
	printf("　　　＃＃　　　　＃＃　　　　　＃　　＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃　＃＃＃＃＃＃＃＃＃＃＃　＃＃　＃　　　　＃＃＃＃＃＃＃＃＃＃　　＃　＃＃＃＃＃＃＃　＃＃　　＃＃　＃　　＃＃＃＃　　＃＃＃＃＃＃　　＃　　　　＃＃＃＃＃＃　＃＃　＃　＃\n");
	printf("　　　＃＃　　　　＃＃　　　　　＃　　　　　　　　＃＃＃　　　　　＃　　＃＃　　＃　＃＃＃　　＃＃　＃　　　　＃＃　　　　　＃＃　　　＃　＃＃＃　　　　　＃＃　　　　　＃　＃＃＃＃　＃＃＃＃　　＃＃＃　＃　　＃＃＃＃＃＃＃＃　＃＃＃＃＃＃\n");
	printf("　　　＃＃　　＃＃＃＃　　　　　＃　　　　　　　　＃＃＃　　　　　＃　　　　　　　＃＃＃　＃＃＃＃　＃　　　　＃＃＃＃＃＃＃＃＃＃　　＃　＃　　　＃＃＃＃＃＃＃＃＃＃　＃　　＃＃　　＃＃＃　　　　＃　　＃　　＃＃　　＃＃＃　　＃＃＃＃　＃\n");
	printf("　　　＃＃　　＃＃＃　　　　　　＃　　　　　　　　　＃　　　　　　＃　　　　　　　＃＃　　　＃＃　　＃　　　　＃＃　　　　　＃＃＃　　＃　　　　　　＃　　　　　　　＃　＃　　　　　　＃＃　　　　　　　　＃　　　　　　　＃　　　　　　　　＃\n");
	printf("　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃\n");
	printf("　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃　　　　　　　　　　　　　　　　＃\n");
	printf("＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃＃\n");
	printf("                                                                                                                                                                                                           河北农业大学                       \n");
	printf("                                                                                                                                                                                                                         计科    2204符文政   \n");
	printf("                                                                                                                                                                                                                         物联网  2201李志帆   \n");
	printf("                                                                                                                                                                                                                         大数据  2201于双淼   \n");
	printf("                                                                                                                                                                                                                                          制作\n");
}

// 显示字符图
void start() {
	HWND hWnd = GetConsoleWindow(); // 获取控制台窗口句柄
	if (!hWnd) {
		fprintf(stderr, "错误：获取控制台窗口句柄失败\n");
		return;
	}
	if (!SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE)) {
		fprintf(stderr, "错误：设置窗口位置失败\n");
		return;
	}
	SetSize(500, 60);
	
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // 获取标准输出流句柄
	if (!hOut) {
		fprintf(stderr, "错误：获取标准输出流句柄失败\n");
		return;
	}
	CONSOLE_FONT_INFOEX font; // 定义控制台字体信息结构体
	font.cbSize = sizeof(font); // 设置结构体大小
	if (!GetCurrentConsoleFontEx(hOut, FALSE, &font)) { // 获取当前字体信息
		fprintf(stderr, "错误：获取当前字体信息失败\n");
		return;
	}
	font.dwFontSize.X = 10; // 设置字体宽度
	font.dwFontSize.Y = 15; // 设置字体高度
	if (!SetCurrentConsoleFontEx(hOut, FALSE, &font)) { // 应用字体大小调整
		fprintf(stderr, "错误：设置字体大小失败\n");
		return;
	}
	
	printchars();  // 调用打印函数
	Sleep(500);
}

//全屏
void full() {
	HWND hWnd = GetConsoleWindow(); // 获取控制台窗口句柄
	if (!hWnd) {
		fprintf(stderr, "错误：获取控制台窗口句柄失败\n");
		return;
	}
	
	int screenWidth = GetSystemMetrics(SM_CXSCREEN); // 获取屏幕宽度
	int screenHeight = GetSystemMetrics(SM_CYSCREEN); // 获取屏幕高度
	if (screenWidth == 0 || screenHeight == 0) {
		fprintf(stderr, "错误：获取屏幕尺寸失败\n");
		return;
	}
	
	BOOL result = SetWindowPos(hWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_SHOWWINDOW); // 设置窗口位置和大小
	if (!result) {
		fprintf(stderr, "错误：设置窗口位置和大小失败\n");
		return;
	}
}

// 生成Excel格式的报表
void generate_report(Car* head, double total_fee) {
	
	// 创建csv文件
	FILE* fp = NULL;
	char filename[50] = "report.csv";
	bool overwrite = false;
	bool rename_file = false;
	
	if (fopen(filename, "r")) {
		// 如果文件已存在，则询问用户是否覆盖
		while (true) {
			printf("文件 %s 已存在. 是否覆盖? (输入'y'确认覆盖)\n", filename);
			char answer = _getch();
			fflush(stdin);  // 清空输入缓冲区
			if (answer == 'y' || answer == 'Y') {
				overwrite = true;
				break;
			}
			else {
				printf("取消写入\n");
				return;
			}
		}
	}
	
	// 打开或创建csv文件，检查是否成功
	if (!overwrite) {
		fp = fopen(rename_file ? filename : "report.csv", "w");
		if (fp == NULL) {
			printf("无法打开或创建报表文件.\n");
			return;
		}
	}
	else {
		fp = fopen("report.csv", "w");
		if (fp == NULL) {
			printf("无法打开或创建报表文件.\n");
			return;
		}
	}
	
	// 写入csv文件头
	fprintf(fp, "车牌号, 车位号, 停车时间\n");
	
	// 遍历链表，写入每个车辆的信息
	Car* current = head;
	if(current == NULL) {
		fprintf(fp, "没有停车记录\n");
		fclose(fp);
		printf("没有停车记录.\n");
		return;
	}
	while (current != NULL) {
		char in_time_str[20];
		strftime(in_time_str, 20, "%Y-%m-%d %H:%M:%S", localtime(&(current->in_time)));
		fprintf(fp, "%ls, %d, %s\n", current->plate_num, current->position, in_time_str);
		current = current->next;
	}
	
	// 写入总金额
	fprintf(fp, "总金额,,,,%lf\n", total_fee);
	
	// 关闭csv文件
	fclose(fp);
	
	if (overwrite) {
		printf("报表覆盖写入成功.\n");
	} else {
		printf("报表生成成功并保存为 %s.\n", rename_file ? filename : "report.csv");
	}
}

//重置
void reset_list(Car** head) {
	
	total_fee = 0; //总金额归零
	
	if (*head == NULL) {  // 链表为空，直接返回
		printf("系统已重置。\n");
		return;
	}
	Car* cur = (*head)->next;  // 指向第一个车辆节点
	while (cur != NULL) {
		Car* next = cur->next;
		free(cur);  // 释放当前节点内存
		cur = next;  // 移动到下一个车辆节点
	}
	(*head)->next = NULL;  // 将头节点指向null
	printf("系统已重置。\n");
	free(*head);  // 释放头节点内存
	*head = NULL;  // 将头节点指针设置为null
}

//车牌号合法
int check_plate_num(wchar_t* plate_num) {
	int len = wcslen(plate_num);
	if (len != 8) {
		if (len <= 7) {
			printf("车牌号长度过短，请输入7位车牌号！(第一位是中文）\n");
		}
		else {
			printf("车牌号长度过长，请输入7位车牌号！\n");
		}
		return 1;
	}
	else {
		int num_cnt = 0; // 记录数字数量
		int letter_cnt = 0; // 记录字母数量
		int chinese_cnt = 0; // 记录中文字符数量
		for (int i = 1; i < len; i++) {
			wchar_t c = plate_num[i];
			if (isdigit(c)) { // 判断是否是数字
				num_cnt++;
			}
			else if (isalpha(c)) { // 判断是否是字母
				letter_cnt++;
			}
			else if (is_chinese_char(c)) { // 判断是否是中文字符
				chinese_cnt++;
			}
		}
		
		if (num_cnt == 0 || letter_cnt == 0) {
			if (num_cnt == 0) {
				printf("车牌号中缺少数字，请重新输入！\n");
			}
			else {
				printf("车牌号中缺少字母，请重新输入！\n");
			}
			return 1;
		}
		else if (num_cnt + letter_cnt != 6) {
			printf("车牌号数字和字母的数量不正确，请重新输入！\n");
			return 1;
		}
		
		if (chinese_cnt > 1) {
			printf("车牌号中的中文字符过多，请重新输入！\n");
			return 1;
		}
		
		return 0;
	}
}

// 判断是否是中文字符
int is_chinese_char(wchar_t c) {
	return (c >= 0x4E00 && c <= 0x9FA5);
}

//刷新缓冲区
void flash(){
	wchar_t ch;
	while ((ch = getchar()) != '\n' && ch != EOF);
}

//停车时长分类
void fee_print(Car* p , double seconds)
{
	if (seconds < 60) {
		printf("车辆：%ls 已经离开，停留时间:%.0lf秒，停车费用:%.2lf元。\n", p->plate_num, seconds, p->fee);
	} else if (seconds >= 60 && seconds < 3600) {
		int minutes = (int)seconds / 60;
		int sec_remainder = (int)seconds % 60;
		if (sec_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d分，停车费用:%.2lf元。\n", p->plate_num, minutes, p->fee);
		} else {
			printf("车辆：%ls 已经离开，停留时间:%d分%d秒，停车费用:%.2lf元。\n", p->plate_num, minutes, sec_remainder, p->fee);
		}
	} 
	else if (seconds >= 3600 && seconds < 86400) {
		int hours = (int)seconds / 3600;
		int min_remainder = (int)((int)seconds % 3600) / 60;
		int sec_remainder = (int)((int)seconds % 3600) % 60;
		if (sec_remainder == 0 && min_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d小时，停车费用:%.2lf元。\n", p->plate_num, hours, p->fee);
		} else if (sec_remainder == 0 && min_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d小时%d分，停车费用:%.2lf元。\n", p->plate_num, hours, min_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d小时%d秒，停车费用:%.2lf元。\n", p->plate_num, hours, sec_remainder, p->fee);
		} else {
			printf("车辆：%ls 已经离开，停留时间:%d小时%d分%d秒，停车费用:%.2lf元。\n", p->plate_num, hours, min_remainder, sec_remainder, p->fee);
		}
	} 
	else if (seconds >= 86400 && seconds < 31536000) {
		int days = (int)seconds / 86400;
		int hours_remainder = (int)((int)seconds % 86400) / 3600;
		int min_remainder = (int)((int)seconds % 3600) / 60;
		int sec_remainder = (int)((int)seconds % 3600) % 60;
		if (sec_remainder == 0 && min_remainder == 0 && hours_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d天，停车费用:%.2lf元。\n", p->plate_num, days, p->fee);
		} else if (sec_remainder == 0 && min_remainder == 0 && hours_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d天%d小时，停车费用:%.2lf元。\n", p->plate_num, days, hours_remainder, p->fee);
		} else if (sec_remainder == 0 && min_remainder != 0 && hours_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d天%d分，停车费用:%.2lf元。\n", p->plate_num, days, min_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder == 0 && hours_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d天%d秒，停车费用:%.2lf元。\n", p->plate_num, days, sec_remainder, p->fee);
		} else if (sec_remainder == 0 && min_remainder != 0 && hours_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d天%d小时%d分，停车费用:%.2lf元。\n", p->plate_num, days, hours_remainder, min_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder == 0 && hours_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d天%d小时%d秒，停车费用:%.2lf元。\n", p->plate_num, days, hours_remainder, sec_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder != 0 && hours_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d天%d分%d秒，停车费用:%.2lf元。\n", p->plate_num, days, min_remainder, sec_remainder, p->fee);
		} else {
			printf("车辆：%ls 已经离开，停留时间:%d天%d小时%d分%d秒，停车费用:%.2lf元。\n", p->plate_num, days, hours_remainder, min_remainder, sec_remainder, p->fee);
		}
	} 
	else {
		int years = (int)seconds / 31536000;
		int days_remainder = (int)((int)seconds % 31536000) / 86400;
		int hours_remainder = (int)((int)seconds % 86400) / 3600;
		int min_remainder = (int)((int)seconds % 3600) / 60;
		int sec_remainder = (int)((int)seconds % 3600) % 60;
		if (sec_remainder == 0 && min_remainder == 0 && hours_remainder == 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年，停车费用:%.2lf元。\n", p->plate_num, years, p->fee);
		} else if (sec_remainder == 0 && min_remainder == 0 && hours_remainder == 0 && days_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d天，停车费用:%.2lf元。\n", p->plate_num, years, days_remainder, p->fee);
		} else if (sec_remainder == 0 && min_remainder == 0 && hours_remainder != 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d小时，停车费用:%.2lf元。\n", p->plate_num, years, hours_remainder, p->fee);
		} else if (sec_remainder == 0 && min_remainder != 0 && hours_remainder == 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d分，停车费用:%.2lf元。\n", p->plate_num, years, min_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder == 0 && hours_remainder == 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d秒，停车费用:%.2lf元。\n", p->plate_num, years, sec_remainder, p->fee);
		} else if (sec_remainder == 0 && min_remainder == 0 && hours_remainder != 0 && days_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d天%d小时，停车费用:%.2lf元。\n", p->plate_num, years, days_remainder, hours_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder == 0 && hours_remainder != 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d小时%d秒，停车费用:%.2lf元。\n", p->plate_num, years, hours_remainder, sec_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder != 0 && hours_remainder == 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d分%d秒，停车费用:%.2lf元。\n", p->plate_num, years, min_remainder, sec_remainder, p->fee);
		} else if (sec_remainder == 0 && min_remainder != 0 && hours_remainder == 0 && days_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d天%d分，停车费用:%.2lf元。\n", p->plate_num, years, days_remainder, min_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder == 0 && hours_remainder == 0 && days_remainder != 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d天%d秒，停车费用:%.2lf元。\n", p->plate_num, years, days_remainder, sec_remainder, p->fee);
		} else if (sec_remainder == 0 && min_remainder != 0 && hours_remainder != 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d小时%d分，停车费用:%.2lf元。\n", p->plate_num, years, hours_remainder, min_remainder, p->fee);
		} else if (sec_remainder != 0 && min_remainder != 0 && hours_remainder != 0 && days_remainder == 0) {
			printf("车辆：%ls 已经离开，停留时间:%d年%d小时%d分%d秒，停车费用:%.2lf元。\n", p->plate_num, years, hours_remainder, min_remainder, sec_remainder, p->fee);
		} else {
			printf("车辆：%ls 已经离开，停留时间:%d年%d天%d小时%d分%d秒，停车费用:%.2lf元。\n", p->plate_num, years, days_remainder, hours_remainder, min_remainder, sec_remainder, p->fee);
		}
	}
}

//停放时间
void time_print(double seconds){
	if (seconds < 60) {
		printf("已经停放时间：%.0lf秒\n", seconds);
	} 
	else if (seconds >= 60 && seconds < 3600) {
		int minutes = (int)(seconds / 60);
		int remainingSeconds = (int)(seconds - minutes * 60);
		if (remainingSeconds > 0) {
			printf("已经停放时间：%d分%d秒\n", minutes, remainingSeconds);
		} else {
			printf("已经停放时间：%d分\n", minutes);
		}
	} 
	else if (seconds >= 3600 && seconds < 86400) {
		int hours = (int)(seconds / 3600);
		int remainingMinutes = (int)((seconds - hours * 3600) / 60);
		int remainingSeconds = (int)(seconds - hours * 3600 - remainingMinutes * 60);
		if (remainingMinutes > 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d小时%d分\n", hours, remainingMinutes);
		} else if (remainingMinutes == 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d小时%d秒\n", hours, remainingSeconds);
		} else if (remainingMinutes > 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d小时%d分%d秒\n", hours, remainingMinutes, remainingSeconds);
		} else {
			printf("已经停放时间：%d小时\n", hours);
		}
	} 
	else if (seconds >= 86400 && seconds < 31536000) {
		int days = (int)(seconds / 86400);
		int remainingHours = (int)((seconds - days * 86400) / 3600);
		int remainingMinutes = (int)((seconds - days * 86400 - remainingHours * 3600) / 60);
		int remainingSeconds = (int)(seconds - days * 86400 - remainingHours * 3600 - remainingMinutes * 60);
		if (remainingHours > 0 && remainingMinutes == 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d天%d小时\n", days, remainingHours);
		} else if (remainingHours == 0 && remainingMinutes > 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d天%d分\n", days, remainingMinutes);
		} else if (remainingHours == 0 && remainingMinutes == 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d天%d秒\n", days, remainingSeconds);
		} else if (remainingHours > 0 && remainingMinutes > 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d天%d小时%d分\n", days, remainingHours, remainingMinutes);
		} else if (remainingHours > 0 && remainingMinutes == 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d天%d小时%d秒\n", days, remainingHours, remainingSeconds);
		} else if (remainingHours == 0 && remainingMinutes > 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d天%d分%d秒\n", days, remainingMinutes, remainingSeconds);
		} else if (remainingHours > 0 && remainingMinutes > 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d天%d小时%d分%d秒\n", days, remainingHours, remainingMinutes, remainingSeconds);
		} else {
			printf("已经停放时间：%d天\n", days);
		}
	}
	else {
		int years = (int)(seconds / 31536000);
		int remainingDays = (int)((seconds - years * 31536000) / 86400);
		int remainingHours = (int)((seconds - years * 31536000 - remainingDays * 86400) / 3600);
		int remainingMinutes = (int)((seconds - years * 31536000 - remainingDays * 86400 - remainingHours * 3600) / 60);
		int remainingSeconds = (int)(seconds - years * 31536000 - remainingDays * 86400 - remainingHours * 3600 - remainingMinutes * 60);
		if (remainingDays > 0 && remainingHours == 0 && remainingMinutes == 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d年%d天\n", years, remainingDays);
		} else if (remainingDays == 0 && remainingHours > 0 && remainingMinutes == 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d年%d小时\n", years, remainingHours);
		} else if (remainingDays == 0 && remainingHours == 0 && remainingMinutes > 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d年%d分\n", years, remainingMinutes);
		} else if (remainingDays == 0 && remainingHours == 0 && remainingMinutes == 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d年%d秒\n", years, remainingSeconds);
		} else if (remainingDays > 0 && remainingHours > 0 && remainingMinutes == 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d年%d天%d小时\n", years, remainingDays, remainingHours);
		} else if (remainingDays > 0 && remainingHours == 0 && remainingMinutes > 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d年%d天%d分\n", years, remainingDays, remainingMinutes);
		} else if (remainingDays > 0 && remainingHours == 0 && remainingMinutes == 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d年%d天%d秒\n", years, remainingDays, remainingSeconds);
		} else if (remainingDays == 0 && remainingHours > 0 && remainingMinutes > 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d年%d小时%d分\n", years, remainingHours, remainingMinutes);
		} else if (remainingDays == 0 && remainingHours > 0 && remainingMinutes == 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d年%d小时%d秒\n", years, remainingHours, remainingSeconds);
		} else if (remainingDays == 0 && remainingHours == 0 && remainingMinutes > 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d年%d分%d秒\n", years, remainingMinutes, remainingSeconds);
		} else if (remainingDays > 0 && remainingHours > 0 && remainingMinutes > 0 && remainingSeconds == 0) {
			printf("已经停放时间：%d年%d天%d小时%d分\n", years, remainingDays, remainingHours, remainingMinutes);
		} else if (remainingDays > 0 && remainingHours == 0 && remainingMinutes > 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d年%d天%d分%d秒\n", years, remainingDays, remainingMinutes, remainingSeconds);
		} else if (remainingDays == 0 && remainingHours > 0 && remainingMinutes > 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d年%d小时%d分%d秒\n", years, remainingHours, remainingMinutes, remainingSeconds);
		} else if (remainingDays > 0 && remainingHours > 0 && remainingMinutes == 0 && remainingSeconds > 0) {
			printf("已经停放时间：%d年%d天%d小时%d秒\n", years, remainingDays, remainingHours, remainingSeconds);
		} else {
			printf("已经停放时间：%d年%d天%d小时%d分%d秒\n", years, remainingDays, remainingHours, remainingMinutes, remainingSeconds);
		}
	}
}

//初始化
void start_on(){
	
	full();
	start();
	loading();
	full();
	head = read_file("data.fuwenzheng");
	total_fee = read_total_fee_from_file("fee.fuwenzheng");
	
}

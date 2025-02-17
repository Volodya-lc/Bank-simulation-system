#include<iostream>
using namespace std;
#include<stdlib.h>
#include<stdio.h> 
#include <windows.h> // Sleep
#include<time.h> //srand,rand
#define OVERFLOW false
#define ERROR false
#define OK true
typedef int Status;
typedef struct customer//定义顾客结构类型
{
	int number;//顾客编号
	long int money;//数额
	int typecase;//办理业务类型
	int continueTime;//办理时间
}CustomerType;
typedef struct Qnode {
	CustomerType cu;
	struct Qnode* next;
}LQNode, * QueuePtr;
typedef struct {
	QueuePtr front;
	QueuePtr rear;
	int len;
 }LinkQueue;//链队列类型
typedef struct event {
	/*定义事件类型*/
	int index;		/*标志变量，1代表到达事件，其余代表离开事件*/
	int time;		/*事件发生时间*/
	int No;			/*触发该事件的客户序号*/
}event;
 LinkQueue *InitQueue()
 {
	 LinkQueue* Q = (LinkQueue*)malloc(sizeof(LinkQueue));
	if (Q==NULL)
	{
		return NULL;
	}
	Q->front= NULL;
	Q->rear = NULL;
	Q->len = 0;
	
	return Q;
}
//入队列
Status EnQueue(LinkQueue* Q, CustomerType e)
{
	QueuePtr p;
	p = (QueuePtr)malloc(sizeof(LQNode));
	if (!p) {
		return OVERFLOW;
	}
	p->cu = e;
	p->next = NULL;
	if (NULL == Q->front) {
		Q->front = p;
		Q->rear = p;
		Q->len = 1;
	}
	else
	{
		Q->rear->next = p;
		Q->rear = p;
		Q->len++;
	}
	
	return OK;
}
//出队列 
Status DeQueue(LinkQueue* Q, CustomerType* e)
{
	QueuePtr p;
	if (Q->front == NULL)
	{
		return ERROR;
	}
	p = Q->front;
	*e = p->cu;
	Q->front = p->next;
	if (Q->rear == p)
	{
		Q->rear = NULL;
	}
	free(p);
	Q->len--;
	
	return OK;
}
Status InsertQueue(LinkQueue* q, LinkQueue* e)
{
	if (e == NULL || e->front == NULL)return ERROR;
	if (q == NULL || q->front == NULL || q->rear == NULL)
	{
		q->front = e->front;
		q->rear = e->rear;
	}
	else {
		e->rear->next = q->front;
		q->front = e->front;
	}
	return OK;
}
Status DestoryQueue(LinkQueue *Q) {
	/*队列的销毁操作*/
	if (Q == NULL)	return ERROR;
	QueuePtr p=NULL, q=NULL;
	p = Q->front;



	while (p != Q->rear) {
		q = p->next;
		free(p);
		p = q;
	}
	free(Q->rear);
	return OK;
}
typedef struct LNode {	/*定义链表节点*/
	event data;
	struct LNode* next;
}Lnode, * Link;

typedef struct LinkL {    /*定义链表*/
	Link head, tail;
	int len;
}*LinkList;

Status InitLink(LinkList& L) {    /*初始化链表操作*/
	L = (LinkList)malloc(sizeof(LNode));
	if (L == NULL)   return NULL;
	L->head = NULL;
	L->tail = NULL;
	L->len = 0;
	
}

Status ClearLink(LinkList& L) {    /*链表的清空操作*/
	if (L == NULL)  return ERROR;
	Link p = L->head;
	Link k = L->head->next;
	while (p != L->tail)
	{
		free(p);
		p = k;
		if (k != L->tail)   k = k->next;
	}
	free(p);
	L->head = NULL;
	L->tail = NULL;
	L->len = 0;
	
	return OK;
}

Status DestoryLink(LinkList L) {     /*链表的销毁操作*/
	if (L == NULL)   return ERROR;
	ClearLink(L);
	free(L);
	
	return OK;
}


Status InsertLink(LinkList L, event e) {    /*链表的尾部插入操作*/
	if (L == NULL)   return ERROR;
	Link p = (Link)malloc(sizeof(LNode));
	if (p == NULL)  return ERROR;
	p->data = e;
	p->next = NULL;
	if (L->head == NULL) {
		L->head = p;
		L->tail = p;
		L->len = 1;
	}
	else {
		L->tail->next = p;
		L->tail = p;
		L->len++;
	}
	
	return OK;
}
int total;//一天开始银行总额
int maxmoney;//取钱最大数额
int nextTime = 0;//下一个客户的到达时间
int nextHandletime = 0;//下一个客户的处理时间
LinkQueue *doing, *waiting;//两个队列
LinkList eventTab=NULL;//事件表
int num = 1;//客户标号
CustomerType* leave=NULL;
CustomerType* waitfor=NULL;

CustomerType * creatCustomer(int max_timetrade)//对顾客赋值
{
	CustomerType* cut;
	cut = (CustomerType*)malloc(sizeof(CustomerType));
	if (cut == NULL) return 0;
	cut->continueTime = (rand() % max_timetrade) + 1;
	if(maxmoney!=0)
	cut->money = (rand() % (2 * maxmoney)) - maxmoney;//客户在取款到存款之间随机取值
	cut->number = num;
	num++;
	return cut;
}
Status freeCustomer(CustomerType* cus)//释放客户节点
{
	if (cus == NULL) return ERROR;
	free(cus);
	return OK;
}
CustomerType* Arrive(int i, int maxgap, int maxtrade)
{
	if (nextTime == i)
	{
		cout << "当前时间是：" << i<<" 分 "<<endl;
		cout<<"当前银行总额为："<<total<<" 元 "<<endl; 
		nextTime = nextTime + (rand() % maxgap) + 1;
		CustomerType* p = NULL;
		p = creatCustomer(maxtrade);
		event* e;
		e = (event*)malloc(sizeof(event));
		e->index = 1;//代表到达
		e->No = p->number;
		e->time = i;
		InsertLink(eventTab, *e);
		if (nextHandletime <=i)//如果当前时间已经超过下一个用户操作时间
		{
			if (-(p->money) <= total)
			{
				nextHandletime = p->continueTime + i;
				total = total + p->money;
				EnQueue(doing, *p);
				cout << "第 " << p->number <<" 位客户到达  "<< "  客户办理需要时间：" << p->continueTime<<" 分钟  " ; 
		    if(p->money>0)
		    cout<< "  客户存款/还款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl << endl;
		    else
		      cout<< "  客户取款/借款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl << endl;
			}
			else
			{
				cout << "第 " << p->number <<" 位客户到达  "<< "  客户办理需要时间：" << p->continueTime<<" 分钟  " ;
				cout<< "  客户取款/借款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl << endl;
				cout << "银行余额不足，入等待办理队列" << endl<<endl;
				EnQueue(waiting, *p);
			}
		}
		else {
			if (nextHandletime == 0) {
				if (-(p->money) <= total)
				{
					nextHandletime = p->continueTime + i;
					total = total + p->money;
					EnQueue(doing, *p);
					cout << "第 " << p->number <<" 位客户到达  "<< "  客户办理需要时间：" << p->continueTime<<" 分钟  " ; 
		           if(p->money>0)
		            cout<< "  客户存款/还款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl << endl;
		         else
		            cout<< "  客户取款/借款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl << endl;
				}
				else
				{
					cout << "第 " << p->number <<" 位客户到达  "<< "  客户办理需要时间：" << p->continueTime<<" 分钟  " ;
					cout<< "  客户取款/借款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl << endl;
					cout << "银行余额不足，入等待办理队列" << endl<<endl;
					EnQueue(waiting, *p);
				}
			}
			else 
			{
				   
				    cout << "第 " << p->number <<" 位客户到达  "<< "  客户办理需要时间：" << p->continueTime<<" 分钟  " ; 
		         if(p->money>0)
		           cout<< "  客户存款/还款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl;
		           else
		            cout<< "  客户取款/借款：  " << p->money<<" 元   此时银行余额： "<<total <<" 元 "<< endl;
					cout<<"因前面还有客户正在服务，所以该客户排入队伍"<<endl<<endl; 
					cout<<nextHandletime-i<<" 分钟后，"<<"第 "<<p->number<<" 位客户办理业务"<<endl<<endl; 
					 EnQueue(doing, *p);
			}
	}
		return p;
		
	}
}
Status Leave(CustomerType* e, int i)//客户离开事件
{
	event* p = (event*)malloc(sizeof(event));
	p->index = 0;
	p->No = e->number;
	p->time = i;
	InsertLink(eventTab, *p);
	return OK;
}
void copeCase(int i)
{
	if (nextHandletime == i)
	{
		DeQueue(doing, leave);
		int shengyu = total - leave->money;//刚刚离开的人操作之前的余额
		
		Leave(leave, i);
		if (((leave->money) > 0) && waiting->front != NULL) {
			int index = 0;
			LinkQueue *e=InitQueue();//可以被满足的客户组成的队列
			int linshi = total;//临时金额
			while (linshi > shengyu && waiting->front != NULL) {//接待了第一个客户
				DeQueue(waiting, waitfor);//检查等待队列
				if (-(waitfor->money) <= linshi) {//等待用户的需求可以满足
					cout <<"当前第 "<<waitfor->number<< " 位用户需求可以被满足" << endl;
					EnQueue(e, *waitfor);//入可满足队列
					linshi = linshi + waitfor->money;
				}
				else {//不能满足要求，重新入第二个队列
				    cout <<"当前第 "<<waitfor->number<< " 位用户需求仍不可以被满足，重新排队" << endl;
					EnQueue(waiting, *waitfor);
					
				}
				index++;
				if (index >= waiting->len) break;
			}
			InsertQueue(doing, e);//把可以满足的人插入到第一个队列
			if (doing->front != NULL)
			{
				while (doing->front != NULL && -(doing->front->cu.money )> total) {
					DeQueue(doing, leave);
					EnQueue(waiting, *leave);
				}
				if (doing->front != NULL)
				{
					nextHandletime = doing->front->cu.continueTime + nextHandletime;
					total = total + doing->front->cu.money;
				}
			}
		}
		else {//刚刚的用户是取钱或借钱
			if (doing->front != NULL)
			{
				while ((doing)->front != NULL && -((doing)->front->cu.money) > total)
				{	/*无法满足的用户进入等待队列*/
					cout << "第"<<doing->front->cu.number<<"位客户的需要银行余额无法满足，入等待队列"<<endl<<endl;
					DeQueue(doing, leave);
					EnQueue(waiting, *leave);
				}
				if ((doing)->front != NULL) {
					/*处理队列不为空*/
					nextHandletime = (doing)->front->cu.continueTime + nextHandletime;
					total = total + (doing)->front->cu.money;
				}
			}
		}
	}
}
int short1;
double average(LinkList l, double final)
{
	double sum = 0, a, b;
	int nmuu = 0;
	Lnode* p = l->head;
	for (int i = 1;i <= l->len;i++)
	{
		while (p != NULL)
		{
			if (p->data.No == i)
			{
				if (p->data.index == 1) {
					a = p->data.time;
					nmuu = nmuu + 1;
				}
				if (p->data.index == 0)b = p->data.time;
			}
			p = p->next;
		}
		p = l->head;	 
		if (b == -1.0) b = final;
		sum = sum + b - a;
		if(i==l->len)
		{
			
			short1=b-a;
		}
		b = -1.0;
	}
	sum=sum-short1*(l->len-nmuu);
	cout <<  "   全部用户数量:   " << nmuu  << endl;
	cout <<  "   总共逗留时间：  " << sum<<" 分钟"<<endl;
	cout <<  "   平均逗留时间：  " << sum / nmuu <<" 分钟"<< endl;
	return sum / nmuu;
}

Status LinkTraverse(LinkList L) {			/*链表的遍历操作*/
	if (L == NULL)  return ERROR;
	Link p = L->head;
	cout<<"\n       =================     客户到达离开记录表     ==================\n";
	while (p != NULL) {
		cout<<"\t时间： "<<p->data.time<<"分   \t客户序号为： " <<p->data.No<<"   \t客户状态为：  " ; 
		if (p->data.index == 1) {
			cout<<"到达 " <<endl;
		}
		else {
	    cout<<"离开"<<endl; 
		}
		p = p->next;
	}
	return OK;
}
int main()
{
	cout << "\n\n\n     ===========================银行业务模拟系统==========================" << endl << endl;
	cout << "\t\t\t\t1.进入模拟" << endl << endl;
	cout << "\t\t\t\t0.退出系统" << endl << endl;
	cout << "     =====================================================================" << endl;
	cout << "请选择：";
	int n;
	cin >> n;

	if (n == 0) { exit(0); }
	else if (n != 0 && n != 1)
	{
		cout << "输入数字错误，系统自动退出中";
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		exit(0);
	}
	else
	{
		cout << "进入银行模拟系统中";
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		system("cls");
		int closetime;
		cout << "\n\n\n     ===========================银行业务模拟系统==========================" << endl << endl;
		cout << "\t\t\t    银行营业时间为（分钟）：   ";cin >> closetime;
		cout << "\t\t\t    银行最开始总额为（元）： ";cin >> total;
		Sleep(100);
		cout << "银行当前总额为：" << total <<" 元 "<< endl;
		Sleep(100);
		leave = (CustomerType*)malloc(sizeof(CustomerType));
		waitfor= (CustomerType*)malloc(sizeof(CustomerType));
		waiting = InitQueue();
		doing = InitQueue();
		InitLink(eventTab);
		srand((unsigned int)time(NULL));
		int maxT, maxG;
		cout << "\t\t\t    客户最大交易金额（元）：   ";cin >> maxmoney;
		Sleep(100);
	
		cout << "\t\t\t    客户最大交易时间（分钟）：   ";cin >> maxT;Sleep(100);
			while(1){
			if(maxT>closetime) 
		  {
			cout<<"\t交易时间超过银行营业时间，无法实现交易，请重新输入！"<<endl;
			cin>>maxT; 
		  }
		  else
		  break;
		}
		cout << "\t\t\t    两位客户最大到达时间间隔（分钟）：   ";cin >> maxG;Sleep(100);
		while(1){
			if(maxG>closetime) 
		  {
			cout<<"\t到达间隔时间超过银行营业时间，无法实现交易，请重新输入！"<<endl;
			cin>>maxG; 
		  }
		  else
		  break;
		 }
		cout << "\t\t\t    银行开始营业" ;
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << ".";
		Sleep(700);
		cout << "\n\n\n===============================================================================  \n\n\n" << endl;
		Sleep(100);
		for (int i = 0;i <closetime;i++)
		{
			Arrive(i, maxG, maxT);
			copeCase(i);
			Sleep(50);
		}
		LinkTraverse(eventTab);//遍历事件表，输出客户状态 
		cout<<endl<<endl; 
		average(eventTab,closetime);//计算平均客户逗留时间 
			DestoryQueue(waiting);//释放空间 
	        DestoryQueue(doing);
	        DestoryLink(eventTab);
	        freeCustomer(waitfor);
	        freeCustomer(leave);
	}
	
	return 0;
}




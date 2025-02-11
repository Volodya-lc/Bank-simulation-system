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
typedef struct customer//����˿ͽṹ����
{
	int number;//�˿ͱ��
	long int money;//����
	int typecase;//����ҵ������
	int continueTime;//����ʱ��
}CustomerType;
typedef struct Qnode {
	CustomerType cu;
	struct Qnode* next;
}LQNode, * QueuePtr;
typedef struct {
	QueuePtr front;
	QueuePtr rear;
	int len;
 }LinkQueue;//����������
typedef struct event {
	/*�����¼�����*/
	int index;		/*��־������1�������¼�����������뿪�¼�*/
	int time;		/*�¼�����ʱ��*/
	int No;			/*�������¼��Ŀͻ����*/
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
//�����
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
//������ 
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
	/*���е����ٲ���*/
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
typedef struct LNode {	/*��������ڵ�*/
	event data;
	struct LNode* next;
}Lnode, * Link;

typedef struct LinkL {    /*��������*/
	Link head, tail;
	int len;
}*LinkList;

Status InitLink(LinkList& L) {    /*��ʼ���������*/
	L = (LinkList)malloc(sizeof(LNode));
	if (L == NULL)   return NULL;
	L->head = NULL;
	L->tail = NULL;
	L->len = 0;
	
}

Status ClearLink(LinkList& L) {    /*�������ղ���*/
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

Status DestoryLink(LinkList L) {     /*��������ٲ���*/
	if (L == NULL)   return ERROR;
	ClearLink(L);
	free(L);
	
	return OK;
}


Status InsertLink(LinkList L, event e) {    /*�����β���������*/
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
int total;//һ�쿪ʼ�����ܶ�
int maxmoney;//ȡǮ�������
int nextTime = 0;//��һ���ͻ��ĵ���ʱ��
int nextHandletime = 0;//��һ���ͻ��Ĵ���ʱ��
LinkQueue *doing, *waiting;//��������
LinkList eventTab=NULL;//�¼���
int num = 1;//�ͻ����
CustomerType* leave=NULL;
CustomerType* waitfor=NULL;

CustomerType * creatCustomer(int max_timetrade)//�Թ˿͸�ֵ
{
	CustomerType* cut;
	cut = (CustomerType*)malloc(sizeof(CustomerType));
	if (cut == NULL) return 0;
	cut->continueTime = (rand() % max_timetrade) + 1;
	if(maxmoney!=0)
	cut->money = (rand() % (2 * maxmoney)) - maxmoney;//�ͻ���ȡ����֮�����ȡֵ
	cut->number = num;
	num++;
	return cut;
}
Status freeCustomer(CustomerType* cus)//�ͷſͻ��ڵ�
{
	if (cus == NULL) return ERROR;
	free(cus);
	return OK;
}
CustomerType* Arrive(int i, int maxgap, int maxtrade)
{
	if (nextTime == i)
	{
		cout << "��ǰʱ���ǣ�" << i<<" �� "<<endl;
		cout<<"��ǰ�����ܶ�Ϊ��"<<total<<" Ԫ "<<endl; 
		nextTime = nextTime + (rand() % maxgap) + 1;
		CustomerType* p = NULL;
		p = creatCustomer(maxtrade);
		event* e;
		e = (event*)malloc(sizeof(event));
		e->index = 1;//������
		e->No = p->number;
		e->time = i;
		InsertLink(eventTab, *e);
		if (nextHandletime <=i)//�����ǰʱ���Ѿ�������һ���û�����ʱ��
		{
			if (-(p->money) <= total)
			{
				nextHandletime = p->continueTime + i;
				total = total + p->money;
				EnQueue(doing, *p);
				cout << "�� " << p->number <<" λ�ͻ�����  "<< "  �ͻ�������Ҫʱ�䣺" << p->continueTime<<" ����  " ; 
		    if(p->money>0)
		    cout<< "  �ͻ����/���  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl << endl;
		    else
		      cout<< "  �ͻ�ȡ��/��  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl << endl;
			}
			else
			{
				cout << "�� " << p->number <<" λ�ͻ�����  "<< "  �ͻ�������Ҫʱ�䣺" << p->continueTime<<" ����  " ;
				cout<< "  �ͻ�ȡ��/��  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl << endl;
				cout << "�������㣬��ȴ��������" << endl<<endl;
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
					cout << "�� " << p->number <<" λ�ͻ�����  "<< "  �ͻ�������Ҫʱ�䣺" << p->continueTime<<" ����  " ; 
		           if(p->money>0)
		            cout<< "  �ͻ����/���  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl << endl;
		         else
		            cout<< "  �ͻ�ȡ��/��  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl << endl;
				}
				else
				{
					cout << "�� " << p->number <<" λ�ͻ�����  "<< "  �ͻ�������Ҫʱ�䣺" << p->continueTime<<" ����  " ;
					cout<< "  �ͻ�ȡ��/��  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl << endl;
					cout << "�������㣬��ȴ��������" << endl<<endl;
					EnQueue(waiting, *p);
				}
			}
			else 
			{
				   
				    cout << "�� " << p->number <<" λ�ͻ�����  "<< "  �ͻ�������Ҫʱ�䣺" << p->continueTime<<" ����  " ; 
		         if(p->money>0)
		           cout<< "  �ͻ����/���  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl;
		           else
		            cout<< "  �ͻ�ȡ��/��  " << p->money<<" Ԫ   ��ʱ������ "<<total <<" Ԫ "<< endl;
					cout<<"��ǰ�滹�пͻ����ڷ������Ըÿͻ��������"<<endl<<endl; 
					cout<<nextHandletime-i<<" ���Ӻ�"<<"�� "<<p->number<<" λ�ͻ�����ҵ��"<<endl<<endl; 
					 EnQueue(doing, *p);
			}
	}
		return p;
		
	}
}
Status Leave(CustomerType* e, int i)//�ͻ��뿪�¼�
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
		int shengyu = total - leave->money;//�ո��뿪���˲���֮ǰ�����
		
		Leave(leave, i);
		if (((leave->money) > 0) && waiting->front != NULL) {
			int index = 0;
			LinkQueue *e=InitQueue();//���Ա�����Ŀͻ���ɵĶ���
			int linshi = total;//��ʱ���
			while (linshi > shengyu && waiting->front != NULL) {//�Ӵ��˵�һ���ͻ�
				DeQueue(waiting, waitfor);//���ȴ�����
				if (-(waitfor->money) <= linshi) {//�ȴ��û��������������
					cout <<"��ǰ�� "<<waitfor->number<< " λ�û�������Ա�����" << endl;
					EnQueue(e, *waitfor);//����������
					linshi = linshi + waitfor->money;
				}
				else {//��������Ҫ��������ڶ�������
				    cout <<"��ǰ�� "<<waitfor->number<< " λ�û������Բ����Ա����㣬�����Ŷ�" << endl;
					EnQueue(waiting, *waitfor);
					
				}
				index++;
				if (index >= waiting->len) break;
			}
			InsertQueue(doing, e);//�ѿ���������˲��뵽��һ������
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
		else {//�ոյ��û���ȡǮ���Ǯ
			if (doing->front != NULL)
			{
				while ((doing)->front != NULL && -((doing)->front->cu.money) > total)
				{	/*�޷�������û�����ȴ�����*/
					cout << "��"<<doing->front->cu.number<<"λ�ͻ�����Ҫ��������޷����㣬��ȴ�����"<<endl<<endl;
					DeQueue(doing, leave);
					EnQueue(waiting, *leave);
				}
				if ((doing)->front != NULL) {
					/*������в�Ϊ��*/
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
	cout <<  "   ȫ���û�����:   " << nmuu  << endl;
	cout <<  "   �ܹ�����ʱ�䣺  " << sum<<" ����"<<endl;
	cout <<  "   ƽ������ʱ�䣺  " << sum / nmuu <<" ����"<< endl;
	return sum / nmuu;
}

Status LinkTraverse(LinkList L) {			/*����ı�������*/
	if (L == NULL)  return ERROR;
	Link p = L->head;
	cout<<"\n       =================     �ͻ������뿪��¼��     ==================\n";
	while (p != NULL) {
		cout<<"\tʱ�䣺 "<<p->data.time<<"��   \t�ͻ����Ϊ�� " <<p->data.No<<"   \t�ͻ�״̬Ϊ��  " ; 
		if (p->data.index == 1) {
			cout<<"���� " <<endl;
		}
		else {
	    cout<<"�뿪"<<endl; 
		}
		p = p->next;
	}
	return OK;
}
int main()
{
	cout << "\n\n\n     ===========================����ҵ��ģ��ϵͳ==========================" << endl << endl;
	cout << "\t\t\t\t1.����ģ��" << endl << endl;
	cout << "\t\t\t\t0.�˳�ϵͳ" << endl << endl;
	cout << "     =====================================================================" << endl;
	cout << "��ѡ��";
	int n;
	cin >> n;

	if (n == 0) { exit(0); }
	else if (n != 0 && n != 1)
	{
		cout << "�������ִ���ϵͳ�Զ��˳���";
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
		cout << "��������ģ��ϵͳ��";
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
		cout << "\n\n\n     ===========================����ҵ��ģ��ϵͳ==========================" << endl << endl;
		cout << "\t\t\t    ����Ӫҵʱ��Ϊ�����ӣ���   ";cin >> closetime;
		cout << "\t\t\t    �����ʼ�ܶ�Ϊ��Ԫ���� ";cin >> total;
		Sleep(100);
		cout << "���е�ǰ�ܶ�Ϊ��" << total <<" Ԫ "<< endl;
		Sleep(100);
		leave = (CustomerType*)malloc(sizeof(CustomerType));
		waitfor= (CustomerType*)malloc(sizeof(CustomerType));
		waiting = InitQueue();
		doing = InitQueue();
		InitLink(eventTab);
		srand((unsigned int)time(NULL));
		int maxT, maxG;
		cout << "\t\t\t    �ͻ�����׽�Ԫ����   ";cin >> maxmoney;
		Sleep(100);
	
		cout << "\t\t\t    �ͻ������ʱ�䣨���ӣ���   ";cin >> maxT;Sleep(100);
			while(1){
			if(maxT>closetime) 
		  {
			cout<<"\t����ʱ�䳬������Ӫҵʱ�䣬�޷�ʵ�ֽ��ף����������룡"<<endl;
			cin>>maxT; 
		  }
		  else
		  break;
		}
		cout << "\t\t\t    ��λ�ͻ���󵽴�ʱ���������ӣ���   ";cin >> maxG;Sleep(100);
		while(1){
			if(maxG>closetime) 
		  {
			cout<<"\t������ʱ�䳬������Ӫҵʱ�䣬�޷�ʵ�ֽ��ף����������룡"<<endl;
			cin>>maxG; 
		  }
		  else
		  break;
		 }
		cout << "\t\t\t    ���п�ʼӪҵ" ;
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
		LinkTraverse(eventTab);//�����¼�������ͻ�״̬ 
		cout<<endl<<endl; 
		average(eventTab,closetime);//����ƽ���ͻ�����ʱ�� 
			DestoryQueue(waiting);//�ͷſռ� 
	        DestoryQueue(doing);
	        DestoryLink(eventTab);
	        freeCustomer(waitfor);
	        freeCustomer(leave);
	}
	
	return 0;
}




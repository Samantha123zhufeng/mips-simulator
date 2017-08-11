#include<iostream>
#include<fstream>
#include<string>
#include<memory.h>
#include<map>
#include<vector>
#include<cstdio>
#include<cstring>
#include"memory.h"
#include"parser.h"
#include"instruction.h"

using namespace std;

/*内存*/
char mem[MAXN];
int heap_top;
/*label的存储*/
map<string, int>text_label;
map<string, int>data_label;
/*寄存器的存储 lo 32 hi 33*/
int reg[34];
/*指令的存储*/
vector<instruction*> ins_vec;
int ins_top;
instruction *plat[5];//5级流水

					 /*动态分支预测*/
int predict_cnt;
int wrong_cnt;

void shut_down(int val);
typedef unsigned long long ull;

int main(int argc, char *argv[])
{
	ifstream source;
	source.open(argv[1]);
	parser par(source, cin, cout);
	par.parse();
	shut_down(0);
}

void shut_down(int val)
{
	vector<instruction*>::iterator it = ins_vec.begin();
	while (it != ins_vec.end())
	{
		delete *(it++);
	}
	exit(val);
}

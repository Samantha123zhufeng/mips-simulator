#pragma once
#pragma once
#ifndef _memory_h
#define _memory_h

#include<vector>
#include<map>
#include<cstdio>

using namespace std;

const int MAXN = 1000005;
const int MAXL = 205;

class instruction;

//!开外部变量
/*内存*/
extern char mem[MAXN];
extern int heap_top;
/*label的存储*/
extern map<string, int>text_label;
extern map<string, int>data_label;
/*寄存器的存储 lo 32 hi 33*/
extern int reg[34];
/*指令的存储*/
extern vector<instruction*> ins_vec;
extern int ins_top;
extern instruction *plat[5];//5级流水

							/*动态分支预测*/
extern int predict_cnt;
extern int wrong_cnt;


#endif


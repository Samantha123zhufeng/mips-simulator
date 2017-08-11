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

//!���ⲿ����
/*�ڴ�*/
extern char mem[MAXN];
extern int heap_top;
/*label�Ĵ洢*/
extern map<string, int>text_label;
extern map<string, int>data_label;
/*�Ĵ����Ĵ洢 lo 32 hi 33*/
extern int reg[34];
/*ָ��Ĵ洢*/
extern vector<instruction*> ins_vec;
extern int ins_top;
extern instruction *plat[5];//5����ˮ

							/*��̬��֧Ԥ��*/
extern int predict_cnt;
extern int wrong_cnt;


#endif


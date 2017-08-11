#pragma once
#ifndef _parser_h
#define _parser_h

#include<thread>
#include<fstream>
#include<iostream>
#include"memory.h"
#include"instruction.h"

using namespace std;

const int MOD = 256;
const int S = 8;

class parser
{
public:
	ifstream &src;
	istream &is;
	ostream &os;

	int reg_cnt[34];
	int ins_vec_sz;
	int pre_cnt[MOD][S];
	int status;
	bool predict[MOD][1 << S];

	parser(ifstream &_src, istream &_is, ostream &_os);
	void parse();
	void read_in();

	void write_back();
	void memory_access();
	void execute();
	void data_prepare();
	void instruction_fetch();

	void run();
};

#endif

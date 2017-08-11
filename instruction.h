#pragma once
#pragma once
#ifndef _instruction_h
#define _instruction_h

#include<vector>
#include<string>
#include<map>
#include<cstring>
#include<cstdio>
#include"memory.h"
#include"other.h"

using namespace std;

extern void shut_down(int val);

class instruction
{
public:
	vector<int>reg_to_read;
	vector<int>reg_to_write;
	int line;
	int jump_type;//0-not jump;1-jump;2-branch;

	instruction() :jump_type(0)
	{
		reg_to_read.clear();
		reg_to_write.clear();
	}

	virtual instruction* fetch()
	{
		return new instruction(*this);
	}
	virtual void data_prepare() {}
	virtual void execute() {}
	virtual void memory_access() {}
	virtual void write_back() {}

	virtual ~instruction() {}

};

class load :public instruction
{
public:
	string address;
	int rdset, rsrc;
	int val, pos, offset;

	load(const string &_rdset, const string &_address) :
		instruction(), rsrc(-1), address(_address), val(0), pos(-1), offset(0)
	{
		rdset = string_to_reg(_rdset);
		reg_to_write.push_back(rdset);
		if (data_label.find(address) != data_label.end())
		{
			pos = data_label[address];
			return;
		}

		int a = address.find('(');
		int b = address.find(')');

		offset = string_to_int(address.substr(0, a));
		rsrc = string_to_reg(address.substr(a + 1, b - a - 1));
		reg_to_read.push_back(rsrc);
	}

	virtual instruction* fetch()
	{
		return new load(*this);
	}

	virtual void data_prepare()
	{
		if (rsrc != -1)
		{
			pos = reg[rsrc];
		}
	}

	virtual void execute()
	{
		pos += offset;
	}
	virtual void write_back()
	{
		reg[rdset] = val;
	}
};

class store :public instruction
{
public:
	string address;
	int rdset, rsrc;
	int val, pos, offset;

	store(const string &_rdset, const string &_address) :
		instruction(), rsrc(-1), address(_address), val(0), pos(-1), offset(0)
	{
		rdset = string_to_reg(_rdset);
		reg_to_read.push_back(rdset);//??
		if (data_label.find(address) != data_label.end())
		{
			pos = data_label[address];
			return;
		}
		int a = address.find('(');
		int b = address.find(')');

		offset = string_to_int(address.substr(0, a));
		rsrc = string_to_reg(address.substr(a + 1, b - a - 1));
		reg_to_read.push_back(rsrc);
	}
	virtual instruction* fetch()
	{
		return new store(*this);
	}
	virtual void data_prepare()
	{
		val = reg[rdset];
		if (rsrc != -1)
		{
			pos = reg[rsrc];
		}
	}
	virtual void execute()
	{
		pos += offset;
	}
};

class calculate :public instruction
{
public:
	int rdset, rsrc1, rsrc2;
	int imm1, imm2, res;

	calculate(const string &_rdset, const string &_rsrc1, const string &_rsrc2) :instruction()
	{
		rdset = string_to_reg(_rdset);
		rsrc1 = string_to_reg(_rsrc1);
		rsrc2 = string_to_reg(_rsrc2);

		reg_to_write.push_back(rdset);

		if (rsrc1 == -1)
		{
			imm1 = string_to_int(_rsrc1);
		}
		else
		{
			reg_to_read.push_back(rsrc1);
		}
		if (rsrc2 == -1)
		{
			imm2 = string_to_int(_rsrc2);
		}
		else
		{
			reg_to_read.push_back(rsrc2);
		}
	}

	virtual instruction* fetch()
	{
		return new calculate(*this);
	}

	virtual void data_prepare()
	{
		if (rsrc1 != -1)
		{
			imm1 = reg[rsrc1];
		}
		if (rsrc2 != -1)
		{
			imm2 = reg[rsrc2];
		}
	}

	virtual void write_back()
	{
		reg[rdset] = res;
	}
};

class assign :public instruction
{
public:
	int rdset, rsrc, imm;

	assign(const string &_rdset, const string &_rsrc) : instruction()
	{
		rdset = string_to_reg(_rdset);
		rsrc = string_to_reg(_rsrc);
		reg_to_write.push_back(rdset);
		if (rsrc == -1)
		{
			imm = string_to_int(_rsrc);
		}
		else
		{
			reg_to_read.push_back(rsrc);
		}
	}
	virtual instruction* fetch()
	{
		return new assign(*this);
	}
	virtual void data_prepare()
	{
		if (rsrc != -1) imm = reg[rsrc];
	}
	virtual void write_back()
	{
		reg[rdset] = imm;
	}
};
/*b,j,jr*/
class jump :public instruction
{
public:
	int rsrc;
	int pos;

	jump(const string &label) :instruction(), rsrc(-1), pos(-1)
	{
		if (text_label.find(label) != text_label.end())
		{
			pos = text_label[label];
		}
		else
		{
			rsrc = string_to_reg(label);
			reg_to_read.push_back(rsrc);
		}
		jump_type = 1;
	}
	virtual instruction* fetch()
	{
		return new jump(*this);
	}
	virtual void data_prepare()
	{
		if (rsrc != -1)
		{
			pos = reg[rsrc];
		}
		ins_top = pos;
	}
};
/*jal,jalr*/
class jal :public instruction
{
public:
	int rsrc;
	int pos;
	int val;
	jal(const string &label) :instruction(), rsrc(-1), pos(-1), val(0)
	{
		if (text_label.find(label) != text_label.end())
		{
			pos = text_label[label];
		}
		else
		{
			rsrc = string_to_reg(label);
			reg_to_read.push_back(rsrc);
		}
		jump_type = 1;
		reg_to_write.push_back(31);
	}
	virtual instruction* fetch()
	{
		return new jal(*this);
	}
	virtual void data_prepare()
	{
		if (rsrc != -1) pos = reg[rsrc];
		val = ins_top;
		ins_top = pos;
	}
	virtual void write_back()
	{
		reg[31] = val;
	}

};

class branch :public instruction
{
public:
	int rsrc1, rsrc2;
	int imm1, imm2;
	int pos;
	bool judge, predict;

	branch(const string &_rsrc1, const string &_rsrc2, const string &label) : instruction(), judge(false)
	{
		rsrc1 = string_to_reg(_rsrc1);
		rsrc2 = string_to_reg(_rsrc2);
		if (rsrc1 == -1)
		{
			imm1 = string_to_int(_rsrc1);
		}
		else
		{
			reg_to_read.push_back(rsrc1);
		}
		if (rsrc2 == -1)
		{
			imm2 = string_to_int(_rsrc2);
		}
		else
		{
			reg_to_read.push_back(rsrc2);
		}
		pos = text_label[label];
		jump_type = 2;
	}
	virtual instruction* fetch()
	{
		return new branch(*this);
	}
	virtual void data_prepare()
	{
		if (rsrc1 != -1) imm1 = reg[rsrc1];
		if (rsrc2 != -1) imm2 = reg[rsrc2];
		ins_top = pos;
	}
};

class syscall : public instruction
{
public:
	istream &is;
	ostream &os;
	int type, val_a0, val_a1, res;
	string str;

	syscall(istream &_is, ostream &_os) : instruction(), is(_is), os(_os)
	{
		str = "";
		reg_to_read.push_back(2);
		reg_to_read.push_back(4);
		reg_to_read.push_back(5);
		reg_to_write.push_back(2);
	}
	virtual instruction* fetch()
	{
		return new syscall(*this);
	}
	virtual void data_prepare()
	{
		type = reg[2]; // $v0
		switch (type)
		{
		case 1: case 4: case 9: case 17:
			val_a0 = reg[4]; break; // $a0
		case 8:
			val_a0 = reg[4]; // $a0
			val_a1 = reg[5]; // $a1
			break;
		}
	}
	virtual void execute()
	{
		str = "";
		switch (type)
		{
		case 1:
			os << val_a0; break;
		case 5:
			is >> res; break;
		case 8:
			is >> str; break;
		case 10:
			shut_down(0); break;
		case 17:
			shut_down(val_a0); break;
		}
	}
	virtual void memory_access()
	{
		int i;
		switch (type)
		{
		case 4:
			i = val_a0;
			while (mem[i])
			{
				os << mem[i++];
			}
			break;
		case 8:
			int l = str.length();
			i = 0;
			while (i < l)
			{
				mem[val_a0 + i] = str[i];
				++i;
			}
			break;
		}
	}
	virtual void write_back()
	{
		switch (type)
		{
		case 5:
			reg[2] = res; break;
		case 9:
			reg[2] = heap_top;
			heap_top += val_a0;
			break;
		}
	}
};

//´Óload¼Ì³Ð
class la :public load
{
public:
	la(const string &ph1, const string &ph2) : load(ph1, ph2) {}
	virtual instruction* fetch()
	{
		return new la(*this);
	}
	virtual void write_back()
	{
		reg[rdset] = pos;
	}
};

class lb :public load
{
public:
	lb(const string &ph1, const string &ph2) : load(ph1, ph2) {}
	virtual instruction* fetch()
	{
		return new lb(*this);
	}
	virtual void memory_access()
	{
		memcpy(&val, mem + pos, 1);
	}
};

class lh :public load
{
public:
	lh(const string &ph1, const string &ph2) : load(ph1, ph2) {}
	virtual instruction* fetch()
	{
		return new lh(*this);
	}
	virtual void memory_access()
	{
		memcpy(&val, mem + pos, 2);
	}
};

class lw :public load
{
public:
	lw(const string &ph1, const string &ph2) : load(ph1, ph2) {}
	virtual instruction* fetch()
	{
		return new lw(*this);
	}
	virtual void memory_access()
	{
		memcpy(&val, mem + pos, 4);
	}
};

//´Óstore¼Ì³Ð
class sb :public store
{
public:
	sb(const string &ph1, const string &ph2) : store(ph1, ph2) {}
	virtual instruction* fetch()
	{
		return new sb(*this);
	}
	virtual void memory_access()
	{
		memcpy(mem + pos, &val, 1);
	}
};

class sh :public store
{
public:
	sh(const string &ph1, const string &ph2) : store(ph1, ph2) {}
	virtual instruction* fetch()
	{
		return new sh(*this);
	}
	virtual void memory_access()
	{
		memcpy(mem + pos, &val, 2);
	}
};

class sw :public store
{
public:
	sw(const string &ph1, const string &ph2) : store(ph1, ph2) {}
	virtual instruction* fetch()
	{
		return new sw(*this);
	}
	virtual void memory_access()
	{
		memcpy(mem + pos, &val, 4);
	}
};

//´Ócalculate¼Ì³Ð
class add :public calculate
{
public:
	bool unsign;

	add(const string &ph1, const string &ph2, const string &ph3, bool _unsign) : calculate(ph1, ph2, ph3), unsign(_unsign) {}
	virtual instruction* fetch()
	{
		return new add(*this);
	}
	virtual void execute()
	{
		res = imm1 + imm2;
	}

};

class sub :public calculate
{
public:
	bool unsign;

	sub(const string &ph1, const string &ph2, const string &ph3, bool _unsign) : calculate(ph1, ph2, ph3), unsign(_unsign) {}
	virtual instruction* fetch()
	{
		return new sub(*this);
	}
	virtual void execute()
	{
		res = imm1 - imm2;
	}
};

class mul :public calculate
{
public:
	int para;
	bool unsign;
	long long llres;

	mul(const string &ph1, const string &ph2, const string &ph3, bool _unsign) : calculate(ph1, ph2, ph3), para(ph3 == "" ? 2 : 3), unsign(_unsign)
	{
		if (para == 2)
		{
			reg_to_read.push_back(rdset);
			reg_to_write.push_back(32);
			reg_to_write.push_back(33);
		}
	}
	virtual instruction* fetch()
	{
		return new mul(*this);
	}
	virtual void data_prepare()
	{
		if (para == 2)
		{
			imm2 = imm1;
			imm1 = reg[rdset];
			if (rsrc1 != -1) imm2 = reg[rsrc1];
		}
		else
		{
			if (rsrc1 != -1) imm1 = reg[rsrc1];
			if (rsrc2 != -1) imm2 = reg[rsrc2];
		}
	}
	virtual void execute()
	{
		if (unsign)
		{
			llres = (unsigned long long)1 * (unsigned int)(imm1) * (unsigned int)(imm2);
		}
		else
		{
			llres = 1LL * imm1 * imm2;
		}
	}
	virtual void write_back()
	{
		if (para == 3)
		{
			reg[rdset] = llres;
		}
		else
		{
			reg[32] = llres;
			reg[33] = (llres >> 32);
		}
	}
};

class __div :public calculate
{
public:
	int para, q, r;
	bool unsign;

	__div(const string &ph1, const string &ph2, const string &ph3, bool _unsign) : calculate(ph1, ph2, ph3), para(ph3 == "" ? 2 : 3), unsign(_unsign)
	{
		if (para == 2)
		{
			reg_to_read.push_back(rdset);
			reg_to_write.push_back(32);
			reg_to_write.push_back(33);
		}
	}
	virtual instruction* fetch()
	{
		return new __div(*this);
	}
	virtual void data_prepare()
	{
		if (para == 2)
		{
			imm2 = imm1;
			imm1 = reg[rdset];
			if (rsrc1 != -1) imm2 = reg[rsrc1];
		}
		else
		{
			if (rsrc1 != -1) imm1 = reg[rsrc1];
			if (rsrc2 != -1) imm2 = reg[rsrc2];
		}
	}
	virtual void execute()
	{
		if (unsign)
		{
			q = (unsigned int)(imm1) / (unsigned int)(imm2);
		}
		else
		{
			q = imm1 / imm2;
		}
		if (unsign)
		{
			r = (unsigned int)(imm1) % (unsigned int)(imm2);
		}
		else
		{
			r = imm1 % imm2;
		}
	}
	virtual void write_back()
	{
		if (para == 3)
		{
			reg[rdset] = q;
		}
		else
		{
			reg[32] = q;
			reg[33] = r;
		}
	}
};

class __xor :public calculate
{
public:
	bool unsign;

	__xor(const string &ph1, const string &ph2, const string &ph3, bool _unsign) : calculate(ph1, ph2, ph3), unsign(_unsign) {}
	virtual instruction* fetch()
	{
		return new __xor(*this);
	}
	virtual void execute()
	{
		res = imm1 ^ imm2;
	}
};

class neg :public calculate
{
public:
	bool unsign;

	neg(const string &ph1, const string &ph2, bool _unsign) : calculate(ph1, ph2, ""), unsign(_unsign) {}
	virtual instruction* fetch()
	{
		return new neg(*this);
	}
	virtual void execute()
	{
		res = -imm1;
	}
};

class rem :public calculate
{
public:
	bool unsign;

	rem(const string &ph1, const string &ph2, const string &ph3, bool _unsign) : calculate(ph1, ph2, ph3), unsign(_unsign) {}
	virtual instruction* fetch()
	{
		return new rem(*this);
	}
	virtual void execute()
	{
		if (unsign) res = (unsigned int)imm1 % (unsigned int)imm2;
		else res = imm1 % imm2;
	}
};

class seq :public calculate
{
public:
	seq(const string &ph1, const string &ph2, const string &ph3) : calculate(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new seq(*this);
	}
	virtual void execute()
	{
		res = imm1 == imm2;
	}
};

class sge :public calculate
{
public:
	sge(const string &ph1, const string &ph2, const string &ph3) : calculate(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new sge(*this);
	}
	virtual void execute()
	{
		res = imm1 >= imm2;
	}
};

class sgt :public calculate
{
public:
	sgt(const string &ph1, const string &ph2, const string &ph3) : calculate(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new sgt(*this);
	}
	virtual void execute()
	{
		res = imm1 > imm2;
	}
};

class sle :public calculate
{
public:
	sle(const string &ph1, const string &ph2, const string &ph3) : calculate(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new sle(*this);
	}
	virtual void execute()
	{
		res = imm1 <= imm2;
	}
};

class slt :public calculate
{
public:
	slt(const string &ph1, const string &ph2, const string &ph3) : calculate(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new slt(*this);
	}
	virtual void execute()
	{
		res = imm1 < imm2;
	}
};

class sne :public calculate
{
public:
	sne(const string &ph1, const string &ph2, const string &ph3) : calculate(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new sne(*this);
	}
	virtual void execute()
	{
		res = imm1 != imm2;
	}
};

//assign

class mfhi : public assign
{
public:
	mfhi(const string &_rdset) : assign(_rdset, "$hi") {}
};

class mflo : public assign
{
public:
	mflo(const string &_rdset) : assign(_rdset, "$lo") {}
};

//´Óbranch ¼Ì³Ð
class beq :public branch
{
public:
	beq(const string &ph1, const string  &ph2, const string &ph3) : branch(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new beq(*this);
	}
	virtual void execute()
	{
		judge = imm1 == imm2;
	}
};

class bne :public branch
{
public:
	bne(const string &ph1, const string  &ph2, const string &ph3) : branch(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new bne(*this);
	}
	virtual void execute()
	{
		judge = imm1 != imm2;
	}
};

class bge :public branch
{
public:
	bge(const string &ph1, const string  &ph2, const string &ph3) : branch(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new bge(*this);
	}
	virtual void execute()
	{
		judge = imm1 >= imm2;
	}
};

class ble :public branch
{
public:
	ble(const string &ph1, const string  &ph2, const string &ph3) : branch(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new ble(*this);
	}
	virtual void execute()
	{
		judge = imm1 <= imm2;
	}
};

class bgt :public branch
{
public:
	bgt(const string &ph1, const string  &ph2, const string &ph3) : branch(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new bgt(*this);
	}
	virtual void execute()
	{
		judge = imm1 > imm2;
	}
};

class blt :public branch
{
public:
	blt(const string &ph1, const string  &ph2, const string &ph3) : branch(ph1, ph2, ph3) {}
	virtual instruction* fetch()
	{
		return new blt(*this);
	}
	virtual void execute()
	{
		judge = imm1 < imm2;
	}
};

#endif 
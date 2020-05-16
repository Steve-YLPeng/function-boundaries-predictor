#ifndef CLASS_H_INCLUDED
#define CLASS_H_INCLUDED
#include <iostream>
#include<iomanip>
#include<fstream>
#include<map>
#include<vector>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<sstream>
#include <stdint.h>

using namespace std;

map<uint64_t,int> Count;
map<short,string> OPtable;


class instr{
public:
    //uint32_t addr;
    short OPcode;
    string OPname;
    string raw_byte;
    string disassembly;
    void print(ostream &fout, uint32_t addr){
        fout<<hex<<addr<<'\t'<<raw_byte<<'\t'<<disassembly<<endl;
    }
};

class nodeInfo{
public:
    uint64_t values;
    short getOP(){
        short op = 0;
        op = (values>>7*8);
        return op;
    }
    static uint64_t getValue(instr &i){
        uint64_t v = 0;
        uint64_t op = i.OPcode;
        switch(i.OPcode){
        //ADD
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
                op = 0x01;
                break;
        //PUSH
            case 0x06:
            case 0x0e:
            case 0x16:
            case 0x1e:
                op = 0x06;
                break;
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            //case 0x54:
            //case 0x55:
            //case 0x56:
            //case 0x57:
                op = 0x53;
                break;
        //POP
            case 0x07:
            case 0x17:
            case 0x1f:
                op = 0x07;
                break;
            case 0x58:
            case 0x59:
            case 0x5a:
            case 0x5b:
            //case 0x5c:
            //case 0x5d:
            //case 0x5e:
            //case 0x5f:
                op = 0x59;
                break;
        //MOV
            case 0xb0:
            case 0xb1:
            case 0xb2:
            case 0xb3:
                op = 0xb0;
                break;
            case 0xb8:
            case 0xb9:
            case 0xba:
            case 0xbb:
                op = 0xb8;
                break;
        }
        v &= 0x0011111111111111;
        v |= (op<<8*7);
        //v = i.OPcode;
        return v;
    }
};

class func{
public:
    //char* id;
    uint32_t begin;
    uint32_t end;
    short size;
    //char* type;
    //char* local;
    //char* ver;
    //char* idx;
    string name;
    //vector<int> instr_list;
    void print(ostream &fout, uint32_t addr){
        fout<<hex<<addr<<"\t+ "<<dec<<size<<" \t"<<name<<endl;
    }
};


class node{
public:
    int positive;
    int negative;
    nodeInfo value;
    //vector<node*> children;
    map<uint64_t, node*> children;

    node():  positive(0), negative(0){
        value.values = -1;
    }
    node(uint64_t op,int tar){
        value.values = op;
        positive = (tar==0 ? 0:1);
        negative = (tar==0 ? 1:0);
    }
    node* predict(instr &i, bool &quit){
        uint64_t op = nodeInfo::getValue(i);
        map<uint64_t, node*>::iterator next = children.find(op);
        if(next != children.end()){ //find
            node* child = next->second;
            return child;
        }
        else{ //not find
            quit = true;
            return this;
        }
    }
    node* insert(instr &i, int tar){
        uint64_t op = nodeInfo::getValue(i);
        map<uint64_t, node*>::iterator next = children.find(op);
        if(next != children.end()){
            //if((*next)->value == op){
                node* child = next->second;
                if(tar==0)
                    (child)->negative++;
                else
                    (child)->positive++;
                return child;
            //}
        }
        if(tar==1){
            node* new_node = new node(op,tar);
            //children.push_back(new_node);
            children[op] = new_node;
            return new_node;
        }
        else
            return NULL;
    }
    void prun(){
        for(map<uint64_t, node*>::iterator it=children.begin(); it!=children.end(); ){
            int p = it->second->positive;
            int n = it->second->negative;
            if(p == 0){
                children.erase(it++);
            }
            else if(n == 0){
                map<uint64_t, node*>::iterator c = it->second->children.begin();
                while(c != it->second->children.end()){
                    it->second->children.erase(c++);
                }
                ++it;
            }
            else{
                it->second->prun();
                ++it;
            }
        }
    }
    uint64_t size(){
        uint64_t sum = 1;
        for(map<uint64_t, node*>::iterator it=children.begin(); it!=children.end(); ++it){
            sum += it->second->size();
        }
        return sum;
    }
    void print(ostream &fout,int d){
        if(d>0 && positive<50)return;
        for(int i=0;i<d;++i)fout<<" | ";
        fout<<"["<<d<<"] ";
        fout<<"s:"<<size()<<"/c:"<<children.size()<<"  ";
        fout<<OPtable[value.getOP()]<<" "<<hex<<value.values<<dec<<"  ";
        fout<<positive<<"/"<<negative<<" ("<<fixed<<setprecision(2)<<positive*100.0/(negative)<<"%)  ";
        fout<<endl;

        for(map<uint64_t, node*>::iterator it=children.begin(); it!=children.end(); ++it)
            it->second->print(fout,d+1);
    }
};


#endif // CLASS_H_INCLUDED

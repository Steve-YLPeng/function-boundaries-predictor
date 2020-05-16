#include "class.h"

using namespace std;

int main()
{
    /*read all programs name*/
    fstream f_pname, fout;
    vector<string> programs;
    f_pname.open("program_name", ios::in);
    fout.open("log", ios::out);
    string str;
    while(f_pname>>str){
        programs.push_back(str);
    }
    int func_count=0;
    int instr_count=0;
    int max_depth = 10;
    int max_opttran = 2;
    int max_opttest = 2;
    node* pfx_tree = new node(-1,0);
    node* pfx_endtree = new node(-1,0);
    map<string,map<uint32_t, func> >prom_func;
    map<string,map<uint32_t, instr> >prom_instr;
    OPtable[-1] = "root";

    cout<<"training...\n";
    for(vector<string>::iterator it=programs.begin(); it!=programs.end(); ++it){
        string prom_name = *it;
        for(int opt_level = 0; opt_level <= max_opttran; ++opt_level){
            char fname[25];
            string s;

            /**start parsing disassemly**/
            sprintf(fname, "o%d/%s_o%d_asm", opt_level, prom_name.c_str(), opt_level);
            //cout<<fname<<endl;
            map<uint32_t, instr> instructions;
            fstream fin_asm;
            fin_asm.open(fname, ios::in);
            if(!fin_asm)cout<<"Fail to open file: "<<fname<<endl;
            uint32_t last_addr;
            while(getline(fin_asm,s)){
                stringstream ss(s);
                string tok;
                if(ss.peek()==' '){
                    instr temp_instr;
                    uint32_t addr;
                    ss>>hex>>addr;
                    if(addr==0)break;
                    ss.ignore(2);
                    getline(ss,temp_instr.raw_byte,'\t');
                    getline(ss,temp_instr.disassembly);
                    //cout<<fname<<"\t"<<hex<<addr<<endl;
                    //temp_instr.print();
                    if((temp_instr.disassembly).size()==0){
                        instructions[last_addr].raw_byte += (temp_instr.raw_byte);
                    }
                    else{
                        stringstream ss_byte(temp_instr.raw_byte);
                        stringstream ss_asm(temp_instr.disassembly);
                        ss_byte>>hex>>temp_instr.OPcode;
                        ss_asm>>temp_instr.OPname;
                        instructions[addr] = temp_instr;
                        last_addr = addr;
                    }
                }
            }
            /**start parsing symtab**/
            sprintf(fname, "o%d/%s_o%d_sym", opt_level, prom_name.c_str(), opt_level);
            cout<<fname<<endl;
            map<uint32_t, func> functions, function_end;
            fstream fin_sym;
            fin_sym.open(fname, ios::in);
            if(!fin_sym)cout<<"Fail to open file: "<<fname<<endl;
            int symtab_loc=2;
            while(getline(fin_sym,s)){
                stringstream ss(s);
                string tok;
                ss>>tok;
                if(tok=="0:"){
                    symtab_loc--;
                }
                if(symtab_loc==0){
                    uint32_t addr, addr_end;
                    func temp_func;
                    ss>>hex>>addr;
                    ss>>dec>>temp_func.size;
                    ss>>tok;
                    if(tok!="FUNC" || addr==0 || temp_func.size==0){
                        continue;
                    }
                    ss>>tok;
                    ss>>tok;
                    ss>>tok;
                    ss>>temp_func.name;
                    map<uint32_t, instr>::iterator ctx = instructions.find(addr);
                    while(ctx->first < addr+temp_func.size){
                        addr_end = ctx->first;
                        ++ctx;
                    }
                    temp_func.begin = addr;
                    temp_func.end = addr_end;
                    if(functions.find(addr)==functions.end()){
                        functions[addr] = temp_func;
                        function_end[addr_end] = temp_func;
                    }
                    else{
                        //functions[addr].print(fout,addr);
                        //temp_func.print(fout,addr);
                    }

                }
            }
            //cout<<functions.size()<<endl;
            //cout<<function_end.size()<<endl;
            func_count+=functions.size();
            instr_count += instructions.size();
            //cout<<instructions.size()<<endl;

            /** for each function **/
            /**PFX tree**/
            for(map<uint32_t, func>::iterator it=functions.begin(); it!=functions.end(); ++it){
                uint32_t fun_start = it->first;
                short size = (it->second).size;
                uint32_t fun_end = fun_start + size;
                //(it->second).print(fout,fun_start);
                map<uint32_t, instr>::iterator ctx = instructions.find(fun_start);
                if(ctx == instructions.end()) cout<<"err"<<endl;

                node* current = pfx_tree;
                for(int d=0; d<max_depth; ++d){
                    /*add op table*/
                    OPtable[(ctx->second).OPcode] = (ctx->second).OPname;
                    /*insert prefix node*/
                    if(ctx!=instructions.end() && ctx->first < fun_end){
                        //(ctx->second).print(fout, ctx->first);
                        current = current->insert(ctx->second, 1);
                        ++ctx;
                    }
                    else break;
                }
            }
            /**PFX End tree**/
            for(map<uint32_t, func>::iterator it=function_end.begin(); it!=function_end.end(); ++it){
                uint32_t fun_end = (it->second).end;
                short size = (it->second).size;
                uint32_t fun_start = (it->second).begin;
                map<uint32_t, instr>::iterator ctx = instructions.find(fun_end);
                if(ctx == instructions.end()) cout<<"err"<<endl;

                node* current = pfx_endtree;
                for(int d=0; d<max_depth; ++d){
                    /*add op table*/
                    OPtable[(ctx->second).OPcode] = (ctx->second).OPname;
                    /*insert prefix node*/
                    current = current->insert(ctx->second, 1);
                    if(ctx==instructions.begin() || ctx->first <= fun_start)break;
                    --ctx;
                }

            }

            sprintf(fname, "o%d/%s_o%d", opt_level, prom_name.c_str(), opt_level);
            prom_instr[fname] = instructions;
            //prom_func[fname] = functions;
        }
    }

    cout<<"func_count: "<<func_count<<endl;
    cout<<"instr_count: "<<instr_count<<endl;
    cout<<"node_count: "<<pfx_tree->size()<<endl;

    /**update negative nodes**/
    cout<<"updating weight...\n";
    for(vector<string>::iterator it=programs.begin(); it!=programs.end(); ++it){
        string prom_name = *it;
        cout<<prom_name<<endl;
        for(int opt_level = 0; opt_level <= max_opttran; ++opt_level){
            char fname[25];
            sprintf(fname, "o%d/%s_o%d", opt_level, prom_name.c_str(), opt_level);
            map<uint32_t, instr> *instructions = &prom_instr[fname];

            map<uint32_t, instr>::iterator seq_start = instructions->begin();
            map<uint32_t, instr>::iterator ctx, ctx2;
            while(seq_start != instructions->end()){
                ctx = ctx2 = seq_start;
                node* current = pfx_tree;
                for(int d=0; d<max_depth; ++d){
                    current = current->insert(ctx->second, 0);
                    ++ctx;
                    if(ctx==instructions->end() || current == NULL) break;
                }
                current = pfx_endtree;
                for(int d=0; d<max_depth; ++d){
                    current = current->insert(ctx->second, 0);
                    if(ctx==instructions->begin() || current == NULL) break;
                    --ctx;
                }
                ++seq_start;
            }
        }
    }

    pfx_tree->prun();
    pfx_tree->print(fout,0);
    pfx_endtree->prun();
    pfx_endtree->print(fout,0);

    f_pname.close();

    /**testing**/
    cout<<"testing...\n";
    double threshold = 0.5;
    int TP[2][9]={{0}},TN[2][9]={{0}},FP[2][9]={{0}},FN[2][9]={{0}};
    vector<string> test;
    f_pname.open("test_name", ios::in);
    while(f_pname>>str){
        test.push_back(str);
    }
    for(vector<string>::iterator it=test.begin(); it!=test.end(); ++it){
        string prom_name = *it;
        for(int opt_level = 0; opt_level <= max_opttest; ++opt_level){
            char fname[25];
            string s;

            /**start parsing disassemly**/
            sprintf(fname, "o%d/%s_o%d_asm", opt_level, prom_name.c_str(), opt_level);
            map<uint32_t, instr> instructions;
            fstream fin_asm;
            fin_asm.open(fname, ios::in);
            if(!fin_asm)cout<<"Fail to open file: "<<fname<<endl;
            uint32_t last_addr;
            while(getline(fin_asm,s)){
                stringstream ss(s);
                string tok;
                if(ss.peek()==' '){
                    instr temp_instr;
                    uint32_t addr;
                    ss>>hex>>addr;
                    if(addr==0)break;
                    ss.ignore(2);
                    getline(ss,temp_instr.raw_byte,'\t');
                    getline(ss,temp_instr.disassembly);
                    if((temp_instr.disassembly).size()==0){
                        instructions[last_addr].raw_byte += (temp_instr.raw_byte);
                    }
                    else{
                        stringstream ss_byte(temp_instr.raw_byte);
                        stringstream ss_asm(temp_instr.disassembly);
                        ss_byte>>hex>>temp_instr.OPcode;
                        ss_asm>>temp_instr.OPname;
                        instructions[addr] = temp_instr;
                        last_addr = addr;
                    }
                }
            }
            /**start parsing symtab**/
            sprintf(fname, "o%d/%s_o%d_sym", opt_level, prom_name.c_str(), opt_level);
            cout<<fname<<endl;
            map<uint32_t, func> functions, function_end;
            fstream fin_sym, fout_addr;
            fin_sym.open(fname, ios::in);
            if(!fin_sym)cout<<"Fail to open file: "<<fname<<endl;
            sprintf(fname, "o%d/%s_o%d_out", opt_level, prom_name.c_str(), opt_level);
            //fout_addr.open(fname, ios::out);
            int symtab_loc=2;
            while(getline(fin_sym,s)){
                stringstream ss(s);
                string tok;
                ss>>tok;
                if(tok=="0:"){
                    symtab_loc--;
                }
                if(symtab_loc==0){
                    uint32_t addr, addr_end;
                    func temp_func;
                    ss>>hex>>addr;
                    ss>>dec>>temp_func.size;
                    ss>>tok;
                    if(tok!="FUNC" || addr==0 || temp_func.size==0){
                        continue;
                    }
                    ss>>tok;
                    ss>>tok;
                    ss>>tok;
                    ss>>temp_func.name;
                    map<uint32_t, instr>::iterator ctx = instructions.find(addr);
                    while(ctx->first < addr+temp_func.size){
                        addr_end = ctx->first;
                        ++ctx;
                    }
                    if(functions.find(addr)==functions.end()){
                        functions[addr] = temp_func;
                        function_end[addr_end] = temp_func;
                    }
                }
            }
            /**testing**/
            map<uint32_t, instr>::iterator seq_start = instructions.begin();
            map<uint32_t, instr>::iterator ctx = seq_start;
            while(seq_start != instructions.end()){
                /**test tree1**/
                ctx = seq_start;
                node* current = pfx_tree;
                bool quit = false;
                bool o_positive;
                bool d_positive;
                for(int d=0; d<max_depth; ++d){
                    if(ctx!=instructions.end() && !quit){
                        current = current->predict(ctx->second,quit);
                        ++ctx;
                    }
                    else break;
                }
                //check
                if(functions.find(seq_start->first) == functions.end())
                    d_positive=false;
                else
                    d_positive=true;
                /*for each threshold*/
                for(int i=0;i<9;++i){
                    threshold = 0.1 + i*0.1;
                    double weight = (double)current->positive/current->negative;
                    if(weight > threshold){
                        o_positive=true;
                        //fout_addr<<ctx->first<<" "<<threshold<<" 0\n";
                    }

                    else
                        o_positive=false;

                    if(d_positive){ //positive
                        if(o_positive) TP[0][i]++;
                        else FN[0][i]++;
                    }
                    else{ //negative
                        if(o_positive) FP[0][i]++;
                        else TN[0][i]++;
                    }
                }
                /**test end tree**/
                ctx = seq_start;
                current = pfx_endtree;
                quit = false;
                for(int d=0; d<max_depth; ++d){
                    current = current->predict(ctx->second,quit);
                    if(ctx==instructions.begin() || quit) break;
                    --ctx;
                }
                //check
                if(function_end.find(seq_start->first) == function_end.end())
                    d_positive=false;
                else
                    d_positive=true;
                for(int i=0;i<9;++i){
                    threshold = 0.1 + i*0.1;
                    double weight = (double)current->positive/current->negative;
                    if(weight > threshold){
                        o_positive=true;
                        //fout_addr<<ctx->first<<" "<<threshold<<" 1\n";
                    }
                    else
                        o_positive=false;

                    if(d_positive){ //positive
                        if(o_positive) TP[1][i]++;
                        else FN[1][i]++;
                    }
                    else{ //negative
                        if(o_positive) FP[1][i]++;
                        else TN[1][i]++;
                    }
                }

                ++seq_start;
            }
            //cout<<TP<<" "<<TN<<" "<<FP<<" "<<FN<<endl;
            //cout<<"Precision: "<<(double)TP/(TP+FP)<<"  Recall: "<<(double)TP/(TP+FN)<<endl;
        }
    }
    fout<<"\nFStart Tree node_count: "<<pfx_tree->size()<<endl;
    fout<<"FEnd Tree node_count: "<<pfx_endtree->size()<<endl;
    for(int j=0;j<2;++j){
        if(j==0)
            fout<<"\nFStart prdiction result\n";
        else
            fout<<"FEND prdiction result\n";
        for(int i=0;i<9;++i){
            //cout<<TP[j][i]+TN[j][i]+FP[j][i]+FN[j][i]<<endl;
            fout<<"threshold="<<0.1+i*0.1<<"  TP:"<<TP[j][i]<<" TN:"<<TN[j][i]<<" FP:"<<FP[j][i]<<" FN:"<<FN[j][i]<<endl;
            fout<<"Precision: "<<(double)TP[j][i]/(TP[j][i]+FP[j][i])*100<<"  Recall: "<<(double)TP[j][i]/(TP[j][i]+FN[j][i])*100<<endl;
        }
        fout<<endl;
    }
    fout<<"OPcode mapping:\n";
    for(map<short,string>::iterator it=OPtable.begin(); it!=OPtable.end(); ++it){
        fout<<hex<<it->first<<": "<<it->second<<endl;
    }
    return 0;
}

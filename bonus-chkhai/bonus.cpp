#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;

const int size_of_stack = 20000;
char stack[size_of_stack];
int PC = 0;
int SP = size_of_stack;  
unordered_map<string, int> function_names;
unordered_map<string, int> registers;
unordered_map<string, string> reg_convert;

void init_reg_convert(){
    reg_convert["zero"] = "x0";
    reg_convert["ra"] = "x1";
    reg_convert["sp"] = "x2";
    reg_convert["gp"] = "x3";
    reg_convert["tp"] = "x4";

    reg_convert["t0"] = "x5";
    reg_convert["t1"] = "x6";
    reg_convert["t2"] = "x7";
    reg_convert["t3"] = "x28"; 
    reg_convert["t4"] = "x29"; 
    reg_convert["t5"] = "x30";
    reg_convert["t6"] = "x31"; 
    
    reg_convert["s0"] = "x8";
    reg_convert["s1"] = "x9";
    reg_convert["s2"] = "x18";
    reg_convert["s3"] = "x19"; 
    reg_convert["s4"] = "x20"; 
    reg_convert["s5"] = "x21";
    reg_convert["s6"] = "x22"; 
    reg_convert["s7"] = "x23"; 
    reg_convert["s8"] = "x24"; 
    reg_convert["s9"] = "x25";
    reg_convert["s10"] = "x26";
    reg_convert["s11"] = "x27"; 

    reg_convert["a0"] = "x10";
    reg_convert["a1"] = "x11";
    reg_convert["a2"] = "x12";
    reg_convert["a3"] = "x13"; 
    reg_convert["a4"] = "x14"; 
    reg_convert["a5"] = "x15";
    reg_convert["a6"] = "x16"; 
    reg_convert["a7"] = "x17"; 
}
void init_registers() {
    for(int i = 0; i <= 31; i++) {
        registers["x" + to_string(i)] = 0;
    }
    registers["x2"] = SP;
}

vector<string> tokenize(string s){
    vector<string> res;
    string tk;
    istringstream stream(s);
    while (stream >> tk) {
        res.push_back(tk);
    }
    return res;
}

void execute_line(vector<string> tokens){
    if (tokens[0].back() == ':') return;

    string op = tokens[0];
    for (char &c : op) c = tolower(c);

    for(int i = 1; i < tokens.size(); i++){
        if(reg_convert.find(tokens[i]) != reg_convert.end()) tokens[i] = reg_convert[tokens[i]];
    }

    if(op == "addi"){
        registers[tokens[1]] = registers[tokens[2]] + stoi(tokens[3]);
    }else if(op == "add"){
        registers[tokens[1]] = registers[tokens[2]] + registers[tokens[3]];
    }else if(op == "sub"){
        registers[tokens[1]] = registers[tokens[2]] - registers[tokens[3]];
    }else if(op == "mul"){
        registers[tokens[1]] = registers[tokens[2]] * registers[tokens[3]];
    }else if(op == "div"){
        registers[tokens[1]] = registers[tokens[2]] / registers[tokens[3]];
    }else if(op == "beq"){
        if(registers[tokens[1]] == registers[tokens[2]]) PC = function_names[tokens[3]] - 1;
    }else if(op == "bne"){
        if(registers[tokens[1]] != registers[tokens[2]]) PC = function_names[tokens[3]] - 1;
    }else if(op == "blt"){
        if(registers[tokens[1]] < registers[tokens[2]]) PC = function_names[tokens[3]] - 1;
    }else if(op == "ble"){
        if(registers[tokens[1]] <= registers[tokens[2]]) PC = function_names[tokens[3]] - 1;
    }else if(op == "bgt"){
        if(registers[tokens[1]] > registers[tokens[2]]) PC = function_names[tokens[3]] - 1;
    }else if(op == "bge"){
        if(registers[tokens[1]] >= registers[tokens[2]]) PC = function_names[tokens[3]] - 1;
    }else if(op == "j"){
        PC = function_names[tokens[1]] + 1;
    }else if(op == "call"){
        registers["x1"] = PC + 1;
        PC = function_names[tokens[1]] - 1;
    }else if(op == "li"){
        registers[tokens[1]] = stoi(tokens[2]);
    }else if(op == "sw"){
        int offset = tokens[2][0] - '0';
        size_t start_pos = tokens[2].find('(');
        string reg = tokens[2].substr(start_pos, tokens[2].size() - start_pos - 2); //retrieve reg
        int address = registers[reg] + offset;  // base + offset
    
        int tmp = registers[tokens[1]];
        memcpy(&stack[address], &tmp, sizeof(int));  // Store 4 bytes
    }else if(op == "sh"){
        int offset = tokens[2][0] - '0';
        size_t start_pos = tokens[2].find('(');
        string reg = tokens[2].substr(start_pos, tokens[2].size() - start_pos - 2); //retrieve reg
        int address = registers[reg] + offset;  // base + offset
    
        short tmp = registers[tokens[1]];
        memcpy(&stack[address], &tmp, sizeof(short));  // Store 2 bytes
    }else if(op == "sb"){
        int offset = tokens[2][0] - '0';
        size_t start_pos = tokens[2].find('(');
        string reg = tokens[2].substr(start_pos, tokens[2].size() - start_pos - 2); //retrieve reg
        int address = registers[reg] + offset;  // base + offset
    
        char tmp = registers[tokens[1]];
        memcpy(&stack[address], &tmp, sizeof(char));  // Store 1 bytes
    }else if(op == "lw"){
        int offset = tokens[2][0] - '0';
        size_t start_pos = tokens[2].find('(');
        string reg = tokens[2].substr(start_pos, tokens[2].size() - start_pos - 2); //retrieve reg
        int address = registers[reg] + offset;  // base + offset
    
        int tmp;
        memcpy(&tmp, &stack[address], sizeof(int));  // Load 4 bytes
        registers[tokens[1]] = tmp;
    }else if(op == "lh"){
        int offset = tokens[2][0] - '0';
        size_t start_pos = tokens[2].find('(');
        string reg = tokens[2].substr(start_pos, tokens[2].size() - start_pos - 2); //retrieve reg
        int address = registers[reg] + offset;  // base + offset
    
        int tmp;
        memcpy(&tmp, &stack[address], sizeof(short));  // Load 2 bytes
        registers[tokens[1]] = tmp;
    }else if(op == "lb"){
        int offset = tokens[2][0] - '0';
        size_t start_pos = tokens[2].find('(');
        string reg = tokens[2].substr(start_pos, tokens[2].size() - start_pos - 2); //retrieve reg
        int address = registers[reg] + offset;  // base + offset
    
        int tmp;
        memcpy(&tmp, &stack[address], sizeof(char));  // Load 1 byte
        registers[tokens[1]] = tmp;
    }else if(op == "ecall"){
        if(registers["x10"] == 1) cout << registers["x11"] << endl;
    }else if(op == "mv"){
        registers[tokens[1]] = registers[tokens[2]];
    }else if(op == "ret"){
        PC = registers["x1"];
    }
}


void execute_commands(vector<string>& line_of_commands){
    while(PC < line_of_commands.size()){
        string curr = line_of_commands[PC];
        vector<string> tokens = tokenize(curr);
        if (!tokens.empty()) execute_line(tokens);
        PC += 1;
    }
}


void read_file(string s){
    ifstream file(s);
    if (!file) {
        cout << "Error opening file " << s << endl;
        return;
    }
    vector<string> line_of_commands;
    string line;
    while(getline(file, line)){
        // cout << line << endl;
        if(line.find(":") != string::npos){
            int idx = line.find(":");
            string func = line.substr(0, idx);
            function_names[func] = line_of_commands.size();
        }
        line_of_commands.push_back(line);
    }
    file.close();
    PC = 0;
    if(function_names.find("main") != function_names.end()) PC = function_names["main"];
    execute_commands(line_of_commands);
}


int main(){
    init_reg_convert();
    init_registers();
    vector<string> files{"data/test1.txt", "data/test2.txt", "data/test3.txt", 
        "data/test4.txt","data/test5.txt", "data/test6.txt"};
    for(string s : files){
        cout << "Starting running tests for " << s << "!" <<endl;
        read_file(s);
        cout << "File read successfully!" << endl;
    }
    return 0;
}
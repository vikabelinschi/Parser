#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>




using namespace std;

struct word
{
    int token;
    string name;
};

enum tokens
{
    tok_keywords = -1,
    tok_operators = -2,
    tok_delimiters = -3,
    tok_identifire = -4,
    tok_numbers = -5,
    tok_dataType = -6,
    tok_string = -7
};
vector<string> keywords = {"function", "return", "for",  "while", "if", "else", "start", "end"};
vector<string> dataType = {"int", "string", "float"};
vector<string> delimiters = {"(",")","{","}",",",";","="};
vector<string> operators = {"+","-","/","*","<",">"};
struct Edges
{
    int src;
    int dest;
    
};
struct Node
{
    int state;
    vector<Edges> edges;
};



namespace fun {
    vector<word> tokens2;
    
    static word currentToken;
    int index = -1;
    static word nextToken(){
        index++;
        return currentToken = tokens2[index];
    }
    
    class Abstexpression{
        public:
        virtual ~Abstexpression(){}
    };
    
    class VariableDecAST : public Abstexpression{
        public:
        string type;
        string name;
        string value;
        
        VariableDecAST(const string &type, const string &name) : type(type), name(name) {cout<<"Variable declared"<<endl;}
        
        string getName(){ return name; }
        string getType(){ return type; }
    };
    
    vector<VariableDecAST> newVariables;
    
    class FunctionDefAST : public Abstexpression{
        public:
        string name;
        string type;
        vector<Abstexpression> body;
        vector<VariableDecAST> args;
        
        FunctionDefAST(const string &name, const string &type, vector<VariableDecAST> args) : name(name), type(type), args(move(args)){cout<<"Function Declared"<<endl;}
        
        string getName(){ return name; }
        vector<VariableDecAST> getArgs(){ return args; }
    };
    
    vector<FunctionDefAST> newFunctions;
    
    class NumberExprAST : public Abstexpression{
        public:
        
        virtual double value() = 0;
    };
    
    class DoubleExprAST : public NumberExprAST{
        double nrValue;
        
        public:
        DoubleExprAST(double nrValue) : nrValue(nrValue){}
        
        double value(){ return nrValue; }
    };
    
    class BinaryOpASt : public NumberExprAST{
        char op;
        NumberExprAST *left;
        NumberExprAST *right;
        
        public:
        BinaryOpASt(char op, NumberExprAST *left, NumberExprAST *right) : op(op), left(move(left)), right(move(right)){}
        
        double value(){
            switch (op)
            {
                case '+': return left->value() + right->value();
                case '-': return left->value() - right->value();
                case '*': return left->value() * right->value();
                case '/': return left->value() / right->value();
                case '>': return left->value() > right->value();
                case '<': return left->value() < right->value();
                default :
                cout<<"InvalidOperatorException"<<endl;
                exit(1);
                return 1;
            }
        }
    };
}

string readFIle()
{
    string myText;
    string line;
    ifstream myfile("input.txt");
    if(myfile.is_open())
    {
        while (getline(myfile, line))
        {
            if (line == "") continue;
            myText += line + " ";
        }
        myfile.close();
    }
    return myText;
}

vector<string> makeWords()
{
    int i = 0;
    vector<string> tmpVec;
    string tmp;
    while (i<readFIle().length())
    {
        if(ispunct(readFIle()[i]))
        {
            tmpVec.push_back(tmp);
            tmpVec.push_back(string(1, readFIle()[i]));
            tmp.clear();
            goto inc;
        }else if(isspace(readFIle()[i]))
        {
            tmpVec.push_back(tmp);
            tmp.clear();
            goto inc;
        }
        tmp+=readFIle()[i];
    inc:i++;
    }
    return tmpVec;
}

vector<word> Tokenvec()
{
    int i = 0;
    vector<word> Tokens;
    vector<string> tmpVec = makeWords();
    while(i < tmpVec.size())
    {
        for(int kl = 0; kl < dataType.size(); kl++){
            if(tmpVec[i]==dataType[kl]){
                Tokens.push_back({tok_dataType, tmpVec[i]});
                goto out;
            }
        }
        
        for(int j = 0; j < keywords.size(); j++)
        {
            if(tmpVec[i]==keywords[j])
            {
                Tokens.push_back({tok_keywords,tmpVec[i]});
                goto out;
            }
        }
        
        for(int k = 0; k < delimiters.size(); k++)
        {
            if(tmpVec[i]==delimiters[k])
            {
                Tokens.push_back({tok_delimiters,tmpVec[i]});
                goto out;
            }
        }
        
        for(int y = 0; y < operators.size(); y++)
        {
            if(tmpVec[i]==operators[y])
            {
                Tokens.push_back({tok_operators,tmpVec[i]});
                goto out;
            }
        }
        
        if(tmpVec[i][0] >= 60 && tmpVec[i][0] <=90 || tmpVec[i][0] >= 97 && tmpVec[i][0] <=122){
            Tokens.push_back({tok_identifire,tmpVec[i]});
        }
        
        for(int z = 0; z < tmpVec[i].length(); z++){
            if(!(tmpVec[i][z] >=48 && tmpVec[i][z] <=57 || tmpVec[i][z]==46)) break;
            else if(z == tmpVec[i].length()-1)
            Tokens.push_back({tok_numbers,tmpVec[i]});
        }
    out: i++;
    }
    return Tokens;
}



void binaryOperations(fun::DoubleExprAST left,int name);
void functionCall();

fun::VariableDecAST variableDeclaration(){
    string type = fun::currentToken.name;
    string name;
    
    fun::nextToken();
    
    if (fun::currentToken.token == tokens::tok_identifire)
    name = fun::currentToken.name;
    else
    {
        cout << "Identifier expected at variable declaration" << endl;
        exit(1);
    }
    fun::nextToken();
    if(fun::currentToken.name.compare(";") == 0){
        fun::VariableDecAST var(type, name);
        fun::newVariables.push_back(var);
        fun::nextToken();
        return var;
    }else
    {
        cout<<"';' is missing "<<endl;
        exit(1);
    }
}

void functionDeclaration(){
    string name;
    string type;
    vector<fun::VariableDecAST> args;
    
    fun::nextToken();
    if(fun::currentToken.token != tokens::tok_dataType){
        cout<<"Return type is missing "<<endl;
        return;
    }
    type = fun::currentToken.name;
    fun::nextToken();
    if(fun::currentToken.token != tokens::tok_identifire){
        cout<<"Invalid function name ' "<<fun::currentToken.name<<" '"<<endl;
        return;
    }
    name = fun::currentToken.name;
    fun::nextToken();
    if(fun::currentToken.name.compare("(") !=0){
        cout<<"'( )' are missing"<<endl;
        return;
    }
    fun::nextToken();
    while(fun::currentToken.name.compare(")") != 0){
        args.push_back(variableDeclaration());
    }
    fun::FunctionDefAST func(name, type, args);
    fun::newFunctions.push_back(func);
    fun::nextToken();
    if (fun::currentToken.name.compare("{") !=0 ){
        cout<<"' { ' is missing"<<endl;
        exit(1);
    }
    fun::nextToken();
    while (fun::currentToken.name.compare("}") != 0){
        if (fun::currentToken.token == tok_dataType)
        variableDeclaration();
        if (fun::currentToken.name.compare("fun") == 0)
        functionDeclaration();
        if (fun::currentToken.token == tok_identifire)
        functionCall();
    }
    fun::nextToken();
}

void variableAssign(string name){
    string type;
    int i;
    fun::DoubleExprAST* tmp;
    
    for (int j = 0; j < fun::newVariables.size(); j++){
        if (name.compare(fun::newVariables[j].name) == 0){
            i = j;
            type = fun::newVariables[j].type;
            break;
        }
        if(j == fun::newVariables.size()-1){
            cout<<"Undeclared varaiable "<<name<<endl;
            exit(1);
        }
    }
    
    
    if (fun::currentToken.name.compare("=") != 0){
        cout<<"' = ' expected"<<endl;
        exit(1);
    }
    fun::nextToken();
    if(count(fun::currentToken.name.begin(), fun::currentToken.name.end(), '.') == 0 && type == "int"){
        fun::newVariables[i].value = fun::currentToken.name;
        fun::DoubleExprAST left4bOP(stod(fun::currentToken.name));
        tmp = &left4bOP;
        fun::nextToken();
    }
    else if(count(fun::currentToken.name.begin(), fun::currentToken.name.end(), '.') == 1 && type == "float"){
        fun::newVariables[i].value = fun::currentToken.name;
        fun::DoubleExprAST left4bOP(stod(fun::currentToken.name));
        tmp = &left4bOP;
        fun::nextToken();
    }
    else if(type == "string"){
        fun::newVariables[i].value = fun::currentToken.name;
        fun::nextToken();
    }else {
        cout<<"Invalid data type for variable : "<<name<<endl;
        exit(1);
    }
    if(fun::currentToken.name.compare(";") == 0)
    fun::nextToken();
    else if (fun::currentToken.token == tok_operators) {
        binaryOperations(*tmp,i);
        return;
    }else{
        cout<<"' ; ' is missing "<<endl;
        exit(1);
    }
    
}

void parser(){
    fun::nextToken();
    while(fun::currentToken.token != -8){
        functionDeclaration();
    }
}

void functionCall(){
    string name;
    int argsNr = 0;
    int i;
    
    name = fun::currentToken.name;
    fun::nextToken();
    if (fun::currentToken.name.compare("(") != 0){
        variableAssign(name);
        return;
    }
    for (int j = 0; j < fun::newFunctions.size(); j++){
        if (name.compare(fun::newFunctions[j].name) == 0){
            i = j;
            break;
        }
        if (j == fun::newFunctions.size()-1){
            cout<<"Unknown function :"<<name<<endl;
            exit(1);
        }
    }
    fun::nextToken();
    while (fun::currentToken.name.compare(")") != 0){
        if (fun::currentToken.name.compare(",") == 0)
        fun::nextToken();
        if (fun::currentToken.token == tok_identifire){
            for (int j = 0; j < fun::newVariables.size(); j++){
                if (fun::newVariables[j].name.compare(fun::currentToken.name) == 0){
                    if (fun::newVariables[j].type == fun::newFunctions[i].args[argsNr].type)
                    break;
                    cout<<"Parameter-"<<fun::currentToken.name<<" incompatible data type"<<endl;
                    exit(1);
                }
            }
            argsNr++;
            fun::nextToken();
        }else if(fun::currentToken.token == tok_numbers){
            if(count(fun::currentToken.name.begin(), fun::currentToken.name.end(), '.') == 0 && fun::newFunctions[i].args[argsNr].type == "int"){
                argsNr++;
                cout<<"int"<<endl;
                fun::nextToken();
            }
            else if(count(fun::currentToken.name.begin(), fun::currentToken.name.end(), '.') == 1 && fun::newFunctions[i].args[argsNr].type == "float"){
                argsNr++;
                cout<<"float"<<endl;
                fun::nextToken();
            }
            else {
                cout<<"Invaid type of parameter->"<<fun::currentToken.name<<endl;
                exit(1);
            }
        }else if(fun::currentToken.token == tok_string){
            if (fun::newFunctions[i].args[argsNr].type == "string"){
                argsNr++;
                fun::nextToken();
            }else {
                cout<<"Invaid type of parameter-"<<fun::currentToken.name<<endl;
                exit(1);
            }
        }
    }
    fun::nextToken();
    if (fun::currentToken.name.compare(";") != 0){
        cout<<"' ; ' is missing"<<endl;
        exit(1);
    }
    cout<<"Function Call"<<endl;
    fun::nextToken();
}

void binaryOperations(fun::DoubleExprAST left,int name){
    int op = 0, opera = 1;
    double finalResult = 0;
    double resultM = 0;
    double resultD = 0;
    vector<string> right;
    right.push_back(to_string(left.value()));
    while(fun::currentToken.name.compare(";") != 0){
        if (fun::currentToken.token == tok_operators){
            right.push_back(fun::currentToken.name);
            op++;
        }
        else if (fun::currentToken.token == tok_numbers){
            right.push_back(fun::currentToken.name);
            opera++;
        }
        else
        {
            cout<<"Unknown symbol in expression "<<fun::currentToken.name<<endl;
            exit(1);
        }
        fun::nextToken();
    }
    if (op != opera-1){
        cout<<"Invalid expression"<<endl;
        exit(1);
    }
    while (right.size() != 1){
        auto itmult = find(right.begin(), right.end(), "*");
        auto itdiv = find(right.begin(), right.end(), "/");
        if (itmult != right.end()) {
            int i = distance(right.begin(), itmult);
            fun::DoubleExprAST node1(stod(right[i - 1]));
            fun::DoubleExprAST node2(stod(right[i + 1]));
            fun::BinaryOpASt binnode('*', &node1, &node2);
            resultM = binnode.value();
            right.erase(right.begin() + i - 1, right.begin() + i + 2);
            right.insert(right.begin() + i - 1, to_string(resultM));
        }else if (itdiv != right.end()) {
            int i = distance(right.begin(), itdiv);
            fun::DoubleExprAST node1(stod(right[i - 1]));
            fun::DoubleExprAST node2(stod(right[i + 1]));
            fun::BinaryOpASt binnode('/', &node1, &node2);
            resultD = binnode.value();
            right.erase(right.begin() + i - 1, right.begin() + i + 2);
            right.insert(right.begin() + i - 1, to_string(resultD));
        }else {
            fun::DoubleExprAST node1(stod(right[0]));
            fun::DoubleExprAST node2(stod(right[2]));
            fun::BinaryOpASt binnode(right[1][0], &node1, &node2);
            finalResult = binnode.value();
            right.erase(right.begin(), right.begin() + 3);
            right.insert(right.begin(), to_string(finalResult));
        }
    }
    fun::nextToken();
    fun::newVariables[name].value = finalResult;
    cout<<fun::newVariables[name].name<<" = "<<finalResult<<fun::newVariables[name].value<<endl;
}


int main () {
    vector<word> Tokens = Tokenvec();
    word last;
    last.name = "EOF";
    last.token = -8;
    Tokens.push_back(last);
    fun::tokens2 = Tokens;
    parser();
    return 0;
}

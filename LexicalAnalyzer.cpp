#include<iostream>
#include <fstream>
#include<vector>
#include<cstring>

using namespace std;

const static char *sFilePath = "input.txt";
const static char *dFilePath = "output.txt";

struct Token
{
    char *code;
    int value;
    int id;
};
//关键字
vector<char*> Keywords = {"and","and_eq","asm","auto","bitand","bitor","bool","break",
"case","catch","char","class","compl","const","const_cast","continue","default",
"delete","do","double","dynamic_cast","else","enum","explicit","export","extern",
"false","float","for","friend","goto","if","inline","int","long","mutable","namespace",
"new","not","not_eq","operator","or","or_eq","private","protected","public","register",
"reinterpret_cast","return","short","signed","sizeof","static","static_cast","struct",
"switch","template","this","throw","true","try","typedef","typeid","typename","union",
"unsigned","using","virtual","void","volatile","wchar_t","while","xor","xor_eq"};
//界限符
vector<char*> Delimiters = {"{","}","(",")","[","]",",",";","\"","'","::"};
//运算符
vector<char*> Operators = {"=","+","-","*","/","%","^","!",">","<",">=","<=","==","!=","<<",">>",
	".","++","--","&","&&","|","||"};
//符号集
vector<char> Symbol = {'{','}','(',')','[',']',',',';','"','\'',':','=','+','-','*',
    '/','%','^','!','>','<','=','!','&','|'};
vector<char*> ID;
vector<char *> Num;
vector<char *> Error;
vector<char *> Type = {"Keywords", "Delimiters", "Operators", "Error","ID", "Num"};
bool IsLetter(char ch)
{
    if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z'))
        return 1;
    else
        return 0;
}
bool IsDigit(char ch)
{
    if(ch>='0'&&ch<='9')
        return 1;
    else
        return 0;
}
int IsSymbol(char ch)
{
    for (int i = 0; i < Symbol.size();i++)
    {
        if(Symbol[i]==ch)
            return 1;
    }
    return 0;    
}
int IsKeyWords(char* str)
{
    for (int i = 0; i < Keywords.size();i++)
    {
        if(!strcmp(Keywords[i],str))
            return i;
    }
    return -1;
}
int IsDelimiters(char* str)
{
    for (int i = 0; i < Delimiters.size();i++)
    {
        if(!strcmp(Delimiters[i],str))
            return i;
    }
    return -1;
}
int IsOperators(char* str)
{
    for (int i = 0; i < Operators.size();i++)
    {
        if(!strcmp(Operators[i],str))
            return i;
    }
    return -1;
}
int CheckID(char *strToken)
{
    int i = 0;
    while(i<ID.size())
    {
        if(!strcmp(strToken,ID[i]))
            return i;
        i++;
    }
    char *tmp = new char[20];
    strcpy_s(tmp,20,strToken);
    ID.push_back(tmp);
    return ID.size()-1;
}
int CheckNum(char *strToken)
{
    int i = 0;
    while(i<Num.size())
    {
        if(!strcmp(strToken,Num[i]))
            return i;
        i++;
    }
    char *tmp = new char[20];
    strcpy_s(tmp,20,strToken);
    Num.push_back(tmp);
    return Num.size()-1;
}
int CheckError(char *strToken)
{
    int i = 0;
    while(i<Error.size())
    {
        if(!strcmp(strToken,Error[i]))
            return i;
        i++;
    }
    char *tmp = new char[20];
    strcpy_s(tmp,20,strToken);
    Error.push_back(tmp);
    return Error.size()-1;
}


void analysis(FILE* stream)
{
	ofstream outfile(dFilePath,ios::out|ios::app);  
	if(!outfile){
	    cout << "Fail to write in" << dFilePath << endl;
		exit(-1);
	}

    vector<Token> TokenStream = {};
    char ch;
    char str[20];
    int pstr=0;
    int state = 0;
    int value;
    while((ch = fgetc(stream)) != EOF)
    {
        if(ch==' '||ch=='\n'||ch=='\t'||(IsSymbol(ch)&&state!=5&&state!=0)||(state==5&&(IsDigit(ch)||IsLetter(ch)))||state==6)
        {
            Token tmp;
            str[pstr] = '\0';
            switch(state)
                {
                    case 0:  //跳过空格
                        break;
                    case 1:  //为数字（整数）
                    case 4:   //是数字（小数）
                        value = CheckNum(str);
                        tmp.value = value;
                        tmp.code = Num[value];
                        tmp.id = 5;
                        TokenStream.push_back(tmp);
                        str[pstr] = '\0';
                        pstr = 0;
                        state = 0;
                        break;
                    case 2:   //为纯字符
                        value = IsKeyWords(str);
                        if(value ==-1)
                        {
                            value = CheckID(str);
                            tmp.value = value;
                            tmp.code = ID[value];
                            tmp.id = 4;
                            TokenStream.push_back(tmp);                            
                        }
                        else
                        {
                            tmp.value = value;
                            tmp.code = Keywords[value];
                            tmp.id = 0;
                            TokenStream.push_back(tmp);
                        }
                        str[pstr] = '/0';
                        pstr = 0;
                        state = 0;
                        break;
                    case 3:    //有数字和字符且第一个为字符
                        value = CheckID(str);
                        tmp.value = value;
                        tmp.code = ID[value];
                        tmp.id = 4;
                        TokenStream.push_back(tmp); 
                        str[pstr] = '/0';
                        pstr = 0;
                        state = 0;
                        break;
                    case 5:
                    case 6:
                        value = IsDelimiters(str);
                        if(value!=-1)
                        {
                            tmp.value = value;
                            tmp.code = Delimiters[value];
                            tmp.id = 1;
                            TokenStream.push_back(tmp); 
                            str[pstr] = '/0';
                            pstr = 0;
                            state = 0;
                            break;
                        }
                        else 
                        {
                            value = IsOperators(str);
                            if(value!=-1)
                            {
                                tmp.value = value;
                                tmp.code = Operators[value];
                                tmp.id = 2;
                                TokenStream.push_back(tmp); 
                                str[pstr] = '/0';
                                pstr = 0;
                                state = 0;
                                break;
                            }
                            else
                            {
                                state = 100;
                            }
                        }
                    case 100:
                        value = CheckError(str);
                        tmp.value = value;
                        tmp.code = Error[value];
                        tmp.id = 3;
                        TokenStream.push_back(tmp);
                        str[pstr] = '/0';
                        pstr = 0;
                        state = 0;
                        break;
                }
        }
        switch(state)
        {
            case 0: //之前没有读入
                if(IsLetter(ch))
                {
                    state = 2;
                    str[pstr++] = ch;
                }
                else if(IsDigit(ch))
                {
                    state = 1;
                    str[pstr++] = ch;
                }
                else if (IsSymbol(ch))
                {
                    state = 5;
                    str[pstr++] = ch;
                }
                break;
            case 1: //之前读入全为数字
                if(IsDigit(ch))
                {
                    state = 1;
                    str[pstr++] = ch;
                }
                else if(ch == '.')
                {
                    state = 4;
                    str[pstr++] = ch;
                }
                else
                {
                    str[pstr++] = ch;
                    state = 100;
                }
                break;
            case 2:   //之前读入全为字符
                if(IsDigit(ch))
                {
                    state = 3;
                    str[pstr++] = ch;
                }  
                else if(IsLetter(ch))
                {
                    state = 2;
                    str[pstr++] = ch;
                }  
                else
                {
                    str[pstr++] = ch;
                    state = 100;
                }            
                break;
            case 3:  //之前读入有字符和数字且第一个为字符
                if(IsLetter(ch))
                {
                    state = 3;
                    str[pstr++] = ch;
                }  
                else
                {
                    str[pstr++] = ch;
                    state = 100;
                }            
                break;
            case 4:  //之前读入过一个小数点
                if(IsDigit(ch))
                {
                    state = 4;
                    str[pstr++] = ch;
                }
                else
                {
                    str[pstr++] = ch;
                    state = 100;
                }                
                break;
            case 5:  //读入过一个符号
                str[pstr++] = ch;
                state = 6;
                break;
            case 6:
                str[pstr++] = ch;
                state = 6;
                break;            

            case 100: //内容出现错误
                str[pstr++] = ch;
                state = 100;            
                break;
        }
    }
    for (int i = 0; i < TokenStream.size();i++)
    {
        cout << "\t< "  << TokenStream[i].code << " , "<<Type[TokenStream[i].id] <<" , "<< TokenStream[i].value << " >" <<endl;
		outfile << "\t< "  << TokenStream[i].code << " , "<<Type[TokenStream[i].id] <<" , "<< TokenStream[i].value << " >" <<endl;        
    }
}

int main()
{
    FILE * stream;
	errno_t err;
	cout << "Read Stream of characters from file " << sFilePath << endl;
	while(1){
		if((err  = fopen_s( &stream, sFilePath, "r" )) ==0)
			break;
		else cout<<"Error！Can't open the file!";
	}
    cout << "\nSource File：" << endl;
	char ch;
	while((ch = fgetc(stream)) != EOF) 
		cout << ch;
	cout << endl << endl;
	ofstream outfile(dFilePath,ios::out);  
	 if(!outfile){
		cout << "Fail to write in" << dFilePath << endl;
		exit(-1);
	 }

    fseek(stream,0L,SEEK_SET);
	cout << ">>>>>>>>>>>Lexical Analysis<<<<<<<<<<<"
		<< endl << endl;

	outfile << ">>>>>>>>>>>Lexical Analysis<<<<<<<<<<<"
		<< endl << endl;

	outfile.close();
    analysis(stream);
    fclose(stream);
    return 0;
}


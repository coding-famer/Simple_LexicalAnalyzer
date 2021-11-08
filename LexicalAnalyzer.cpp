#include<iostream>
#include <fstream>
#include<vector>

using namespace std;

const static char *sFilePath = "input.txt";
const static char *dFilePath = "output.txt";

struct Token
{
    char *code;
    int value;
    int id;
};
#define nKeywords 74  //关键字数量
#define nDelimiters 11  //界限符数量
#define nOperators 23  // 运算符数量
//关键字
vector<char*> Keywords = {"and","and_eq","asm","auto","bitand","bitor","bool","break",
"case","catch","char","class","compl","const","const_cast","continue","default",
"delete","do","double","dynamic_cast","else","enum","explicit","export","extern",
"false","float","for","friend","goto","if","inline","int","long","mutable","namespace",
"new","not","not_eq","operator","or","or_eq","private","protected","public","register",
"reinterpret_cast","return","short","signed","sizeof","static","static_cast","struct",
"switch","template","this","throw","true","try","typedef","typeid","typename","union",
"unsigned","using","virtual","void","volatile","wchar_t","while","xor","xor_eq"};//keywords
//界限符
vector<char*> Delimiters = {"{","}","(",")","[","]",",",";","\"","'","::"};//delimiters
//运算符
vector<char*> Operators = {"=","+","-","*","/","%","^","!",">","<",">=","<=","==","!=","<<",">>",
	".","++","--","&","&&","|","||"};//operators
vector<char*> ID;
vector<char *> Num;

bool IsLetter(char ch)
{
    if((ch>='a'&&ch<='z')||ch>='A'&&ch<='z')
        return 1;
    else
        return 0;
}
bool IsDigit(char ch)
{
    if(ch>='0'&&ch<=9)
        return 1;
    else
        return 0;
}
int IsReservedWords(char* str)
{
    for (int i = 0; i < nKeywords;i++)
    {
        if(!strcmp(Keywords[i],str))
            return i;
    }
    for (int i = 0; i < nDelimiters;i++)
    {
        if(!strcmp(Delimiters[i],str))
            return nKeywords + i;
    }
    for (int i = 0; i < nOperators;i++)
    {
        if(!strcmp(Operators[i],str))
            return nKeywords + nDelimiters + i;
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
    ID.push_back(strToken);
    return ID.size();
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
    Num.push_back(strToken);
    return Num.size();
}

void analysis(FILE* stream)
{
	ofstream outfile(dFilePath,ios::out|ios::app);  
	if(!outfile){
	    cout << "Fail to write in" << dFilePath << endl;
		exit(-1);
	}

    vector<Token> TokenStream;
    char ch;
    char str[20];
    int pstr=0;
    int state = 0;
    int oldstate = 0;
    int value;
    while((ch = fgetc(stream)) != EOF)
    {
        switch(state)
        {
            case 0: //之前没有读入
                if(ch==' '||ch=='\n'||ch=='\t')
                {
                    oldstate = state;
                    state = 10;
                }
                else if(IsLetter(ch)!=-1)
                {
                    state = 2;
                    str[pstr++] = ch;
                }
                else if(IsDigit(ch)!=-1)
                {
                    state = 1;
                    str[pstr++] = ch;
                }
                break;
            case 1: //之前读入全为数字
                if(ch==' '||ch=='\n'||ch=='\t')
                {
                    oldstate = state;
                    state = 10;
                }
                else if(IsDigit(ch)!=-1)
                {
                    state = 1;
                    str[pstr++] = ch;
                }
                else
                {
                    state = 100;
                }
                break;
            case 2:   //之前读入全为字符
                if(ch==' '||ch=='\n'||ch=='\t')
                {
                    oldstate = state;
                    state = 10;
                }
                else if(IsDigit(ch)!=-1)
                {
                    state = 3;
                    str[pstr++] = ch;
                }  
                else if(IsLetter(ch)!=-1)
                {
                    state = 2;
                    str[pstr++] = ch;
                }  
                else
                {
                    state = 100;
                }            
                break;
            case 3:  //之前读入有字符和数字且第一个为字符s
                break;
            case 10:  //读到空格回车等
                str[pstr] = '\0';
                switch(oldstate)
                {
                    case 0:
                        break;
                    case 1:
                        value = CheckID(str);
                        Token tmp;
                        tmp.value = value;
                        tmp.code = str;
                        tmp.id = 0;
                        TokenStream.push_back(tmp);
                        pstr = 0;
                        state = 0;
                        oldstate = 0;
                        break;
                    case 2:
                }
                break;
            case 100: //内容出现错误
                while(ch!=' '||ch!='\t'||ch!='\n')
                    str[pstr++] = fgetc(stream);
                Token tmp;
                tmp.value = -1;
                tmp.code = str;
                tmp.id = 0;
                TokenStream.push_back(tmp);
                pstr = 0;
                state = 0;
                oldstate = 0;               

                break;
        }
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
		<<"\nnote：KW = key word, ID = identification, "
		<<"\nNUM = number, DM = delimiter, OP = operator." 
		<< endl << endl;

	outfile << ">>>>>>>>>>>Lexical Analysis<<<<<<<<<<<"
		<<"\nnote：KW = key word, ID = identification, "
		<<"\nNUM = number, DM = delimiter, OP = operator." 
		<< endl << endl;

	outfile.close();
    analysis(stream);
    fclose(stream);
    return 0;
}


                // else if(IsLetter(ch)!=-1)
                //     {
                //         while(IsDigit(ch)||IsLetter(ch))
                //         {
                //             str[pstr++] = ch;
                //             ch = fgetc(stream);
                //         }
                //         fseek(stream, -1L, SEEK_CUR);
                //         str[pstr] = '\0';
                //         int value = IsReservedWords(str);
                //     }
                // else if(IsDigit(ch)!=-1)
                // {
                    
                // }
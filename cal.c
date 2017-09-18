#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Include local header files

// TODO: Use all unused parameters and remove this temporary definition
// Helps eliminating warning C4100: 'error': unreferenced formal parameter
#define UNUSED_PARAMETER(name) (void)name

/******************************Preface*********************************/
//1. 除了原件框架以外的全局变量或者自定义函数放在Private区域内；
//2. 对已定义的函数进行修改时，在函数之前写出函数的修改时间和功能，有变动的单独说明；
//3. 尽力规范编程，以锻炼协调编程为主要目的；
//4. 有任何需求或问题写在Questions区域内进行讨论和研究；
//5. 类型的定义尽量写清注释；
//6. 变量命名方式以方便阅读为主要目的，编码风格协调一致；
/*******************************END*************************************/


/***************************Questions***********************************/
//1.在对动态分配变量line的使用需要小心，防止由于更改‘\n’字符而导致内存泄露
/****************************END****************************************/


/**********************Private parameters*******************************/
#define TRUE 0
#define FALSE 1

// Question :so what your meaning to built this enum ?
typedef enum
{
	NORMAL,
	END_LINE,
	END_FILE,
	NO_MEMORY,
}statut_t;
statut_t Statut_File = NORMAL;



/*************************END*******************************************/


//////////////////////////////////////////////////////////////////////////////
// Dummy calc module

// TODO: Move to a separate module 'calc'
typedef enum
{
  ERR_OK,
  ERR_NOT_ENOUGH_MEMORY,
  // TODO: Add your own error codes
} error_t;
error_t error = ERR_OK;

// TODO: Move to a separate module 'calc'
char const* GetErrorString(error_t error)
{
  // TODO: Find the corresponding error description
  UNUSED_PARAMETER(error);
  return "";
}

// 2017/9/18
//	根据给定操作符和操作数求解计算结果
double OP(double op1,double op2,char op)  
{  
    double res = 0;  
    
    switch(op)
    {
        case '+': res = op1 + op2; break;
        case '-': res = op1 - op2; break;
        case '*': res = op1 * op2; break;
        case '/': res = op1 / op2; break;
        case '^': res =   pow(op1, op2); break;
        case 'A': res =   sin(op2); break;
        case 'B': res =   cos(op2); break;
        case 'C': res =   tan(op2); break;
        case 'D': res = 1/tan(op2); break;
        case 'F': res =  asin(op2); break;
        case 'G': res =  acos(op2); break;
        case 'H': res =  atan(op2); break;
        case 'I': res =  sqrt(op2); break;
        case 'J': res =   log(op2); break;
        case 'K': res = floor(op2); break;
        case 'L': res =  ceil(op2); break;
        
    }
    return res;  
}  
  
// 2017/9/18
// 当前不具备错误码输入输出功能。
// 函数功能：　	将常规中缀表达式转换为逆波兰（后缀）表达式
// 输入参数：	s 	包含中缀表达式的字符串,其中所有函数使用 funName 中对应的大写字母表示，且不含有任何其他字母。
// 输出参数：	output	包含逆波兰表达式的字符串,各元素间以空格作为间隔符。
void Polish(char *s, char *output, error_t* error)          //将一个中缀串转换为后缀串
{  
    unsigned int outLen = 0;  
    unsigned int top;           //stack count
    char *stack = (char*)malloc( strlen(s) * sizeof(char));
    int i;
    memset(output,'\0',sizeof output);  //输出串  
    while(*s != '\0')               //1）  
    {  
        if (isDigit(s))              
        {
            output[outLen++] = *s;        //3）假如是操作数，把它添加到输出串中。  
            while( *(s+1) !='\0' && isDigit( s +1 ))
            {  
                output[outLen++] = *( s+1 );  
                ++s;  
            }  
            output[outLen++] = ' ';     //空格隔开  
        }
        if (*s=='(' || isSymFunction(*s))                  //4）假如是闭括号，将它压栈。  
        {  
            ++top;  
            stack[top] = *s;  
        }  
        while (isOperator(*s) )        //5)如果是运算符，执行算法对应操作；  
        {  
            if (top==0 || stack[top]=='(' || priority(*s) > priority( *(stack+top))) //空栈||或者栈顶为)||新来的元素优先级更高  
            {  
                ++top;  
                stack[top] = *s;  
                break;  
            }  
            else  
            {  
                output[outLen++] = stack[top];  
                output[outLen++] = ' ';  
                --top;  
            }  
        }  
        if (*s==')')                  //6）假如是开括号，栈中运算符逐个出栈并输出，直到遇到闭括号。闭括号出栈并丢弃。  
        {  
            while (stack[top]!='(')  
            {  
                output[outLen++] = stack[top];  
                output[outLen++] = ' ';  
                --top;  
            }  
            --top;  // 此时stack[top]==')',跳过)  
        }
        s ++;  
        //7）假如输入还未完毕，跳转到步骤2。  
    }  
    while (top!=0)                      //8）假如输入完毕，栈中剩余的所有操作符出栈并加到输出串中。  
    {  
        output[outLen++] = stack[top];  
        output[outLen++] = ' ';  
        --top;  
	}
	free(stack); 
}  

// 2017/9/18
// 添加的已完成的calculate 函数 
// 当前不具备错误码输入输出功能。

// 功能： 求解逆波兰表达式计算结果
// 输入：expr	:逆波兰表达式字符串
// 		error :错误码输出码指针
// 输出：		表达式计算结果
// TODO: Move to a separate module 'calc'
double Calculate(char const* expr, error_t* error)	
{
	double result;
	// TODO: Replace with a computational algorithm subdivided into modules/functions
	char* dst = (char*)malloc( 20 * sizeof(char));  
	double* cSt1 = (double*)malloc( strlen(expr) * sizeof(double));   	//波兰式需要用两个栈，逆波兰式只需要一个栈  
	unsigned int top1=0;  
	
	while ( *expr )  
	{  
		printf( "\ns = %s ", expr);
		if (*expr != ' ')  
		{  
			sscanf(expr,"%s",dst);  
			// printf("\n, %d ",dst,isDigit(dst));
			if (isDigit(dst))  
			{  
				++top1;  
				cSt1[top1] = atof(dst);     //进栈  
			}  
			else if(isOperator(*dst))
			{  
				
				printf("\n %f %c %f=",cSt1[top1-1], dst[0], cSt1[top1]);
				cSt1[top1-1] = OP( cSt1[top1-1], cSt1[top1], dst[0]); 
				printf("%f",cSt1[top1-1]); 
				--top1;     //操作数栈：出栈俩，进栈一 
			}
			else if(isSymFunction(*dst))
			{
				printf("\n %c %f=", dst[0], cSt1[top1]);
				cSt1[top1] = OP( 0, cSt1[top1], dst[0]); 
				printf("%f",cSt1[top1]); 
			}
			while (*expr && *expr != ' ')  
			{
				// printf("p = %p\n",expr);
				++expr;
			}  
		}
		++expr;  
	}
	result = cSt1[1];
	
	free(dst);
	free(cSt1);
		
	if (error != NULL)
    	*error = ERR_OK;

	return result;
}

//////////////////////////////////////////////////////////////////////////////
// UI functions

//parameters: FILE, sizeofline
//function: To read the line of messages from the stdin
//return： The first address of the messages which be read
//the types of errors are included at here: 
//date: 2017-9-17 02:45:35
//需要决策的设计：
//在这里需要对内存不足，内存溢出，行结尾，文件结尾，以及正常状态进行判断，如前述，是否使用同一标识符表示？
//一种方法是设计一个状态变量类型，在需要用到的函数体内创建相应的状态变量，否则单一使用一个全局变量来进行判断，其数量不足以满足判断需求。
//为了解决上述问题，是否取消全局变量而只使用相应状态变量类型来创建需要的变量。
//该功能实现并不困难，关键先统一表示法。需要自行设计。
char* ReadLine(FILE* in, int* len)
{
  char* line = (char*)malloc(2*sizeof(char));
  int temp;

  *len = 0;
  if(line == NULL)
  {
	printf("ERROR: Memory Not Enough \n");
	free(line);
	return NULL;
  }

  do{
	temp= fgetc(in);

	if(line == NULL)
	{
		printf("ERROR: Memory Not Enough \n");
		free(line);
		return NULL;
	}
//考虑如果读到文件结尾后，在进行一次读写会不会出问题
	if(temp == EOF || temp == 26)
	{   
//遇到的文件为空文件
		if(*len == 0){
			if(line != NULL)
				free(line);
			return NULL;//这种情况可以直接结束返回
		}
		else
		{
			Statut_File = END_FILE;//将结尾换成换行符，并更改状态，执行完赋值操作，然后结束返回
			temp = '\n';
		}
	}
//插入到字符串操作
	else
	{
		line[*len] = (char)temp;
		(*len)++;
		line =(char*)realloc(line, ((*len)+1)*sizeof(char));
		if(line == NULL)
		{
			printf("ERROR: Memory Not Enough \n");
			free(line);
			return NULL;
		}
	}


  }while(temp != '\n');

  line[*len] = '\0';
  return line;
}

//parameters: string
//function: whether need to calculate
//return： True or False
//the types of errors are included at here: 
//
//在完成读功能后，参考设计需求和其他程序进行设计
//15行以内解决
//需要判断内容：是否含有非法参数，括号数量是否正好，是否以算术符结尾，
//这里需要讨论能否对sin cos等功能进行判断以及判断方法
int NeedCalculate(char const* line, int len)
{
	int brk = 0, counter = 0, spesym = TRUE;
	char lastch = TRUE, statut = TRUE;

	if(line[counter] == '\0')//空行
		return 0;
//检查
	do
	{
		if(line[0] == '/' && line[1] == '/')//注释行
		{
			printf("%s", line);
			return 0;
		}
//是否有特殊操作符
		if (!((line[counter] == '\a') || (line[counter] == '\b') || (line[counter] == '\t') || (line[counter] == '\n') || \
			(line[counter] == '\v') || (line[counter] == '\r') || (line[counter] == '\f')))
			spesym = FALSE;
//判断结尾是否为数字
		if(line[counter]>47 && line[counter]<58)
			lastch = TRUE;
		else if(line[counter]!='\n' && line[counter]!=' ' && line[counter] != '(' && line[counter] != ')')
			lastch = FALSE;
//判断括号数量是否吻合
		if(line[counter] == '(')
			brk++;
		if(line[counter] == ')')
			brk--;
//是否有连续的操作符
//sin
		if(line[counter] == 's'){
			if( line[counter+1] == 'i' && line[counter+2] == 'n' && ((line[counter+3]>47 &&line[counter]<58) || line[counter+3]==' ' || line[counter+3] == '(')){
				if(line[counter+3] == ' '){//判断sin   5类似的格式
					int i = 4;
					statut = FALSE;
					do{//跳过空格
						if(line[counter+i] != ' ' )
							i++;
						else if((line[counter+i] >47&&line[counter+i]<58)||line[counter+i] == '(' ){counter += i;statut = TRUE;break;}
						else{printf("Error symbol\n");return FALSE;} //这里需要改
					}while(line[counter+i] != '\n' && line[counter+i] != '\0');
					if(statut == FALSE){lastch = FALSE;break;}//判断以sin     结尾的格式

				}
			}
		}
//cos
		if(line[counter] == 'c'){
			if( line[counter+1] == 'o' && line[counter+2] == 's' && ((line[counter+3]>47 &&line[counter]<58) || line[counter+3]==' ' || line[counter+3] == '(')){
				if(line[counter+3] == ' '){//判断sin   5类似的格式
					int i = 4;
					statut = FALSE;
					do{//跳过空格
						if(line[counter+i] != ' ' )
							i++;
						else if((line[counter+i] >47&&line[counter+i]<58)||line[counter+i] == '(' ){counter += i;statut = TRUE;break;}
						else{printf("Error symbol\n");return FALSE;} //这里需要改
					}while(line[counter+i] != '\n' && line[counter+i] != '\0');
					if(statut == FALSE){lastch = FALSE;break;}//判断以sin     结尾的格式

				}
			}
		}
//先测试这两个可以用的话，在复制
	}while(line[++counter] != '\0');
//长度错误
	if(counter != len){printf("Lenth Error\n");return 0;}
//括号数量错误
	if(brk != 0 || lastch == FALSE || spesym == FALSE){printf("Wrong expression\n");return 0;}

    return 1;
}

error_t ReportError(error_t error)
{
  printf("ERROR: %s\n", GetErrorString(error));
  return error;
}



void ProcessLine(char const* line)
{
  double result;
  error_t lastError = ERR_OK;

  printf("%s == ", line);
  result = Calculate(line, &lastError);
  if (lastError == ERR_OK)
    printf("%lg\n", result);
  else
    ReportError(lastError);
}

int main(int argc, char const* argv[])
{
  	char* line = "100%200*lnlg(asin6^2)/(3asincos2)";

	ProcessLine(line);
	free(line);

	// Clean up
	if (in != stdin)
		fclose(in);
	return 0;
}
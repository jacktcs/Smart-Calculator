#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

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
//7. 把NeedCalculate中对括号和字母判断的命令注释掉了，在后面的fun2sym中可以完成同样的功能，
//      然后把表达式的打印部分代码做了相应的调整。
//8. 解决了如果空行是\n的情况导致的bug
//9. 解决了free的bug，原因是memset函数如果是对一个内容随机的字符串进行清空，可能会超出字符串的空间范围
/*******************************END*************************************/


/***************************Questions***********************************/
//1.在对动态分配变量line的使用需要小心，防止由于更改‘\n’字符而导致内存泄露
//2.free（） 总是会导致内存溢出，网上说可能是修改了malloc的指针导致的，
//  但是经过检查，出错的free对应的指针都没有被修改过，
//  因此考虑是不是编译器自动添加了free函数，导致其被重复调用而出错，
//  暂时将free注释掉，忽略该问题
//4. 尽量把可以不用动态变量的地方替换掉，并且在进行指针操作是谨慎的检查
//5. 可以的话吧函数传参时候的变量名尽量改成一致的，不会造成误解。

/****************************END****************************************/


/**********************Private parameters*******************************/
#define TRUE 0
#define FALSE 1


// Question :so what your meaning to built this enum ?
//629行，主函数判断文件结尾时候需要用，因为识别到函数到结尾了，Read之后跳出循环还需要在进行最后一次的处理
typedef enum
{
	NORMAL,
	END_LINE,
	END_FILE,
	NO_MEMORY,              //question 这里为什么要放no memory ? 岂不是跟 error_t 打架了 ？
}statut_t;//所以最开始时候说是不是要把状态变量的参数和报错的合并，因为有些交集，多用一个变量会更麻烦
statut_t Statut_File = NORMAL;

// TODO: Move to a separate module 'calc'
typedef enum
{
  ERR_OK,
  ERR_NOT_ENOUGH_MEMORY,
  ERR_WRONG_EXPRESSION,
  ERR_OTHER,            // add this statue to represent all other error that we didn't give
  
  // TODO: Add your own error codes
} error_t;

// TODO: Move to a separate module 'calc'
char const* GetErrorString(error_t error)
{
    char *errorStr;
    // TODO: Find the corresponding error description
    switch(error)
    {
        case ERR_NOT_ENOUGH_MEMORY:
            errorStr = "ERR_NOT_ENOUGH_MEMORY";
            break;
        case ERR_WRONG_EXPRESSION:
            errorStr = "ERR_WRONG_EXPRESSION";
            break;
        case ERR_OTHER:
            errorStr = "Unkown Error";
            break;      
		case ERR_OK:
		default:
			errorStr = "No Error";
			break;

    }
    return errorStr;
}

void ReportError(error_t error)
{
  printf("ERROR: %s\n", GetErrorString(error));
//   return error;
}


/*************************END*******************************************/

int isFunction(char* op);
int isSymFunction(char op);

//判断运算符级别函数；其中* /的级别为2，+ -的级别为1；  
// 此处如果返回0 应当报错
int priority(char op)               
{  
    if (op=='+' || op=='-')  
        return 1;  
    if (op=='*' || op=='/')  
        return 2;  
    if (op == '^')
		return 3;
    if (isSymFunction(op))
        return 4;
    if (op == '~')
		return 5;
    else  
        return 0;  
}  

int isNumber(char *op)
{
    return (*op >='0' && *op<='9');
}


int isConstantNum(char *op)
{
    return (    ((*op =='p') && (*(op+1) =='i')) 
               || (*op =='e')
           );
}

int isDigit(char *op)
{
    return  ((*op =='.'&&isNumber( op+1 ))      //小数，且点号后面有数字
            || isNumber( op )                   //数字
            || isConstantNum( op )              //数学常数: 自然对数e 以及圆周率pi
            );
}

int isAlphabet(char op)  // 字母
{
    return (op >='a' && op<='z');
}
//判断输入串中的字符是不是操作符，如果是返回 1 
int isOperator(char op)                
{  
    return (op=='+' || op=='-' || op=='*' || op=='/' || op=='^');  
}  

//在此定义了改程序支持的所有数学函数，及其代号
#define FUNMAX 20   //程序支持的所有数学函数个数
typedef struct
{
    char name[7];
    char sym;
} funName2Sym;
funName2Sym funName[FUNMAX] = {   "sin",      'A',    //1
                            "cos",      'B',    //2
                            "tg",       'C',    //3
                            "tan",      'C',    //3
                            "ctg",      'D',    //4
                            "cot",      'D',    //4
                            "ctan",     'D',    //4
                            "asin",     'F',    //5
                            "arcsin",   'F',    //5
                            "acos",     'G',    //6
                            "arccos",   'G',    //6
                            "arctg",    'H',    //7
                            "arctan",   'H',    //7
                            "atan",     'H',    //7
                            "sqrt",     'I',    //8
                            "ln",       'J',    //9
                            "log",      'J',    //9
                            "lg",       'J',    //9
                            "floor",    'K',    //10
                            "ceil",     'L'     //11
};

int isFunction(char* op)
{
	int i=0;
	for( ;i<FUNMAX;i++)
        if( op == strstr(op,funName[i].name))
            return i+1;
    return 0;
}

int isSymFunction(char op)
{
	int i=0;
    for(; i<FUNMAX;i++)
        if( op == funName[i].sym)
            return i+1;
    return 0;
}

char *strlwrt(char *str)
{
    char *orign =str;
    for (; *str!='\0'; str++,orign++)
        *orign = (char)tolower(*str);
    return orign;
}


// 将字符串中的数学函数转换为对应的字母代号
void fun2sym(char *expr, error_t *error)
{
    int tmp ;
    int i;
    strlwrt(expr);  // 将字符串中所有字母转换为小写字母
    while (*expr)
    {
        tmp = isFunction(expr)-1;
        // 这个写法有点儿逆天，但是暂时没找到更好的优化方案
        if(tmp>-1) //if the first several alphabet consist of the function name
        {
            // replace it with its sym 
            *expr = funName[tmp].sym;
            for( i =strlen(funName[tmp].name); i>1; i--)
                *(++expr) = ' ';
        }
        else if(isAlphabet(*expr) && *expr != 'e')
        {
			*error = ERR_WRONG_EXPRESSION;
			return;
        }
        ++expr;
    }
}


//	根据给定操作符和操作数求解计算结果
double OP(double op1,double op2,char op, error_t *error)  
{  
    double res = 0;  
    
    switch(op)
    {
        case '+': res = op1 + op2; break;
        case '-': res = op1 - op2; break;
        case '*': res = op1 * op2; break;
        case '/': res = op1 / op2; break;
        case '~': res =      -op2; break;
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
        default: *error = ERR_OTHER;
    }
    return res;  
}  

// 函数功能：    字符串复制，并去除空格
// 输入参数：	输入字符串
// 输出参数：	输出字符串
void copyExpr(char *a, char *b)
{
    while(*b != '\0')
    {
        if (*b != ' ') 
            *a++ = *b;
        b++;
    }
}
// 函数功能：    区分表达式中的负号，将其转换为~
// 输入参数：	表达式字符串
// 输出参数：	经过转换的字符串
void minus2negative(char *expr)
{
    int i=0;
    
    if (expr[i] == '-') expr[i++] = '~';
    while( expr[i] )
    {
        if ( expr[i] == '-' )
            if ( !isDigit( &expr[i-1]) && expr[i-1]!=')' )
                expr[i] = '~';
        ++i;
    }
}

// 当前不具备错误码输入输出功能。
// 函数功能：　	将常规中缀表达式转换为逆波兰（后缀）表达式
// 输入参数：	s 	包含中缀表达式的字符串,其中所有函数使用 funName 中对应的大写字母表示，且不含有任何其他字母。
// 输出参数：	output	包含逆波兰表达式的字符串,各元素间以空格作为间隔符。
void Polish (char const*s, char *output, error_t* error)
{  
    unsigned int outLen = 0,i=0;  
    unsigned int top = 0;           //stack count

    char *stack = (char*)malloc( 2*strlen(s) * sizeof(char));
    char *expr_in = (char*)malloc( 2*strlen(s) * sizeof(char));
	//printf("\nstack len == %d", 2 * strlen(s) * sizeof(char));
    // no memory error test 
    if (stack == NULL || expr_in == NULL )
    {
        // printf("ERROR: Memory Not Enough \n");
        *error = ERR_NOT_ENOUGH_MEMORY;
        free(stack);
        free(expr_in);
        return;
    }
    copyExpr(expr_in,s);
    minus2negative(expr_in);
    fun2sym(expr_in, error);
    
    if(*error != ERR_OK)
        return;
    
        //如果output申请时，空间中不包含\0，那么计算的时候很可能会超出指定的容量最终报错
	//memset(output,'\0',strlen(output) *sizeof (char));  //输出串  
    while(*(expr_in+i))               //1）  
    {  
		//printf("\ntop = %d", top);
		if (isDigit(expr_in+i))              
        {
            output[outLen++] = *(expr_in+i);        //3）假如是操作数，把它添加到输出串中。  
            while( *(expr_in+i+1) !='\0' && isDigit( expr_in+i +1 ))
            {  
                output[outLen++] = *( expr_in+i+1 );  
                ++i;  
            }  
            output[outLen++] = ' ';     //空格隔开  
        }
        if (*(expr_in+i)=='(' || isSymFunction(*(expr_in+i)) || *(expr_in+i) == '~')   //4）假如是闭括号，将它压栈。  
        {  
            ++top;  
            stack[top] = *(expr_in+i);  
        }  
        while (isOperator(*(expr_in+i)) )        //5)如果是运算符，执行算法对应操作；  
        {  
            if (top==0 || stack[top]=='(' || priority(*(expr_in+i)) > priority( *(stack+top))) //空栈||或者栈顶为)||新来的元素优先级更高  
            {  
                ++top;  
                stack[top] = *(expr_in+i);  
                break;  
            }  
            else  
            {  
                output[outLen++] = stack[top];  
                output[outLen++] = ' ';  
                --top;  
            }  
        }  
        if (*(expr_in+i)==')')                  //6）假如是开括号，栈中运算符逐个出栈并输出，直到遇到闭括号。闭括号出栈并丢弃。  
        {  
            while (stack[top]!='(')  
            {  
                output[outLen++] = stack[top];  
                output[outLen++] = ' ';  
                --top;  
            }  
            --top;  // 此时stack[top]==')',跳过)  
        }
        ++ i;  
        //7）假如输入还未完毕，跳转到步骤2。  
    }  
    while (top!=0)                      //8）假如输入完毕，栈中剩余的所有操作符出栈并加到输出串中。  
    {  
        output[outLen++] = stack[top];  
        output[outLen++] = ' ';  
        --top;  
	}
    output[outLen] = '\0';
    free(stack); 
    stack = NULL;
    free(expr_in); 
    expr_in = NULL;
}  




//////////////////////////////////////////////////////////////////////////////
// Dummy calc module

// 功能： 求解逆波兰表达式计算结果
// 输入：expr	:逆波兰表达式字符串
// 		error :错误码输出码指针
// 输出：		表达式计算结果
// TODO: Move to a separate module 'calc'
double Calculate(char const* expr_in, error_t* error)
{
	double result = 0;
	// TODO: Replace with a computational algorithm subdivided into modules/functions
    char *expr= (char*)malloc(2*strlen(expr_in) * sizeof(char));
    char *dst = (char*)malloc( 20 * sizeof(char));  
    double *cSt = (double*)malloc( 2*sizeof(double)*strlen(expr_in));   	//波兰式需要用两个栈，逆波兰式只需要一个栈  
    unsigned int top=0,i=0;  
    
    // no memory error test 
    if (expr == NULL || dst == NULL ||cSt == NULL )
    {
        // printf("ERROR: Memory Not Enough \n");
        *error = ERR_NOT_ENOUGH_MEMORY;
        free(expr);
        free(dst);
        free(cSt);
        return 0;
    }
    Polish (expr_in, expr, error);
    // if the polish function gives error status
    if (*error != ERR_OK)
        return 0;
	
	while ( *(expr+i) )  
	{  
		//printf( "\ns = %s ", (expr+i));
		if (*(expr+i) != ' ')  
		{  
			sscanf((expr+i),"%s",dst);  
			if (isDigit(dst))  
			{  
				++top;  
				cSt[top] = atof(dst);     //进栈  
			}  
			else if(isOperator(*dst))
			{  
				
				// printf("\n %f %c %f=",cSt[top-1], dst[0], cSt[top]);
				cSt[top-1] = OP( cSt[top-1], cSt[top], dst[0], error); 
				// printf("%f",cSt[top-1]); 
				--top;     //操作数栈：出栈俩，进栈一 
			}
			else if(isSymFunction(*dst)  || *dst=='~')
			{
				// printf("\n %c %f=", dst[0], cSt[top]);
				cSt[top] = OP( 0, cSt[top], dst[0], error); 
				// printf("%f",cSt[top]); 
			}
			while (*(expr+i) && *(expr+i) != ' ')  
			    ++i;
		}
		++i;  
	}

    result = cSt[1];
    free(cSt);
    cSt = NULL;
    free(dst);
    dst = NULL;
    free(expr);
    expr = NULL;
	// what that code means ?
	 if (error == NULL)
     	*error = ERR_OK;

	return result;
}

//////////////////////////////////////////////////////////////////////////////

// 功能： 从标准输入读取字符
// 输入：in： 文件指针
// 		 *len： 字符串长度指针
// 输出：		返回动态分配后字符串首地址
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
        if(*len == 0)
        {
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

// 功能： 初步判断计算需要
// 输入：line: 字符串
// 		len：字符串长度 
// 输出：是否需要计算
int NeedCalculate(char const* line, int len)
{
	int counter = 0, spesym = TRUE;
	char lastch = TRUE;
    
    // remove the spaces
    while(line[counter] == ' ')
        counter++; 
    
    if(line[counter] == '\0' || line[counter] == '\n')//空行
    {  
        printf("%s", line);        
        return 0;
    }
//检查
	do
	{
        //个别情况下，输入文件会在这里直接换行，因此空行也可能是\n
        if(line[0] == '/' && line[1] == '/')//注释行
		{
			printf("%s", line);
			return 0;
		}
//是否有特殊操作符
		if ((line[counter] == '\a') || (line[counter] == '\b') || (line[counter] == '\t') || \
			(line[counter] == '\v') || (line[counter] == '\r') || (line[counter] == '\f'))
			spesym = FALSE;
//判断结尾是否为数字
		 if(line[counter]>47 && line[counter]<58)
		 	lastch = TRUE;
		 else if(line[counter]!='\n' && line[counter]!=' ' && line[counter] != '(' && line[counter] != ')')
		 	lastch = FALSE;
	}while(line[++counter] != '\0');

//长度错误
	if(counter != len){printf("Lenth Error\n");return 0;}
//括号数量错误
	 if(spesym == FALSE){printf("Wrong expression\n");return 0;}
	 if(lastch == FALSE){printf("Wrong expression\n");return 0;}

    return 1;
}





void ProcessLine(char const* line)
{
    double result = 0;
    error_t lastError = ERR_OK;
    // error_t *lastError_t = &lastError;
	int i = 0;
	
	char *expr_in = (char*)malloc( 2*strlen(line) * sizeof(char));
	 // no memory error test 
    if (expr_in == NULL )
    {
        lastError = ERR_NOT_ENOUGH_MEMORY;    
        ReportError(lastError);
        free(expr_in);
        return;
    }
    strcpy(expr_in,line);


	i = strlen(expr_in);
	expr_in[i -1] = '\0';

    
    //printf("%s", expr_in);        
    
    result = Calculate(line, &lastError);
        
    if (lastError == ERR_OK)
    {
        printf("%s == ", expr_in);        
        printf("%lg\n", result);
    }
    else
        ReportError(lastError);
    
	free(expr_in); //这里有问题需要解决

}

int main(int argc, char const* argv[])
{
    FILE* in = stdin;
    char* line = NULL;

    //Private part
    int len = 0;
    int* len_p = &len;
    

    // Choose an input source
    if (argc == 1 )
    {
        in = stdin;
    }
    else if  ((in = fopen(argv[1], "r")) == NULL)
    {
        printf("ERROR: Cannot open file '%s'", argv[1]);
        return -1;
    }
    
    // Process the data line by line
    while ((line = ReadLine(in, len_p)) != NULL || Statut_File == END_FILE)
    {
        
        if(NeedCalculate(line, len))
            ProcessLine(line);

        free(line);
    }
    //结尾处最后一行的处理
     if(Statut_File == END_FILE)
         if(NeedCalculate(line, len))
             ProcessLine(line);
     free(line);

    // Clean up
    if (in != stdin)
        fclose(in);
    return 0;
}
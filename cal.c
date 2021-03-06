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
//5. 可以的话吧函数传参时候的变量名尽量改成一致的，不会造成误解。
//7. 表达式中有特殊符号，会被跳过 i.e. ||||
//8. 1+( == 2   ????
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
  ERR_DIV_BY_ZERO,
  ERR_INCORRECT_PARAMETERS,
  ERR_TOO_MANY_PARAMETERS,
  ERR_NOT_NUMBER_RESULT,
  ERR_INFINIT_RESULT,
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
        case ERR_OK:		
            errorStr = "No Error";
            break;
        case ERR_NOT_ENOUGH_MEMORY:
            errorStr = "ERROR: ERR_NOT_ENOUGH_MEMORY";
            break;
        case ERR_WRONG_EXPRESSION:
            errorStr = "ERROR: ERR_WRONG_EXPRESSION";
            break;
        case ERR_DIV_BY_ZERO:    
            errorStr = "ERROR: ERR_DIV_BY_ZERO";
            break;
        case ERR_INCORRECT_PARAMETERS:    
            errorStr = "ERROR: ERR_INCORRECT_PARAMETERS";
            break;
        case ERR_TOO_MANY_PARAMETERS:    
            errorStr = "ERROR: ERR_TOO_MANY_PARAMETERS";
            break;
        case ERR_NOT_NUMBER_RESULT:
            errorStr = "ERROR: ERR_NOT_NUMBER_RESULT";
            break;
        case ERR_INFINIT_RESULT:
            errorStr = "ERROR: ERR_INFINIT_RESULT";
            break;
        case ERR_OTHER:
        default:
            errorStr = "ERROR: Unkown Error";
            break;      
        
    }
    return errorStr;
}

void ReportError(error_t error)
{
  printf("%s\n", GetErrorString(error));
//   return error;
}


/*************************END*******************************************/

int isFunction(char* op);
int isSymFunction(char op);

// 此处如果返回0 应当报错
int priority(char op)               
{  
    if (op=='+' || op=='-')  
        return 1;  
    if (op=='*' || op=='/')  
        return 2;  
    if (op== '~')  
        return 3;  
    if (op == '^')
		return 4;
    if (isSymFunction(op))
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
                || ((*(op-1) =='p') && (*op =='i')) 
                || ((*op =='e') && !isNumber(op+1))
           );
}

int isDigit(char *op)
{
    return  ((*op =='.'&&isNumber( op+1 ))      //小数，且点号后面有数字
			|| ((*op =='e') && isNumber(op+1))  //数字
            || ((*op =='e') && ((*(op+1) =='-') || (*(op+1) =='~')) && isNumber(op+2))  //负指数数字
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
    return (op=='+' || op=='-' || op=='*' || op=='/' || op=='^' );  
}  

int isInt(double op)  // 字母
{
    return ( op == (int)op );
}
//在此定义了改程序支持的所有数学函数，及其代号
#define FUNMAX 20   //程序支持的所有数学函数个数
typedef struct
{
    char name[10];
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
    while (*expr)
    {
        tmp = isFunction(expr)-1;
        //printf( "tmp = %d \n", tmp);
		// 这个写法有点儿逆天，但是暂时没找到更好的优化方案
        if(tmp>-1) //if the first several alphabet consist of the function name
        {
            int strl = strlen(funName[tmp].name);
            i=strl;
            while( expr[i]==' ' ) i++;
            if( expr[i] == '(' && i != strl)
            {
                *error = ERR_WRONG_EXPRESSION;
                return;
            }
            else
            {
                // replace it with its sym 
                *expr = funName[tmp].sym;
                for( i = strl ; i>1; i--)
                    *(++expr) = ' ';
            }
        }
        else if (!isDigit(expr) && isAlphabet(*expr))
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
        case '/': 
                if(op2 == 0.0 )
                {
                    *error = ERR_DIV_BY_ZERO;
                    return 0;
                }
                else
                    res = op1 / op2; 
                    break;
        case '~': res =      -op2; break;
        case '^': res =   pow(op1, op2); break;
        case 'A': res =   sin(op2); break;
        case 'B': res =   cos(op2); break;
        case 'C': res =   tan(op2); break;
        case 'D': res = 1/tan(op2); break;
        case 'F': 
                if ( op2>1.0 || op2<-1.0 )
                {
                    *error = ERR_INCORRECT_PARAMETERS;
                    return 0;
                }
                else
                    res =  asin(op2); break;
        case 'G': 
                if ( op2>1.0 || op2<-1.0 )
                {
                    *error = ERR_INCORRECT_PARAMETERS;
                    return 0;
                }
                else
                    res =  acos(op2); break;
        case 'H': res =  atan(op2); break;
        case 'I': 
                if ( op2<1.0 )
                {
                    *error = ERR_INCORRECT_PARAMETERS;
                    return 0;
                }
                else
                    res =  sqrt(op2); break;
        case 'J': 
                if ( op2<1.0 )
                {
                    *error = ERR_INCORRECT_PARAMETERS;
                    return 0;
                }
                else
                    res =   log(op2); break;
        case 'K': res = floor(op2); break;
        case 'L': res =  ceil(op2); break;
        default: *error = ERR_OTHER;
    }
    return res;  
}  

// 函数功能：    字符串复制，并去除空格
// 输入参数：	输入字符串
// 输出参数：	输出字符串
void copyExpr(char *a, char const*b)
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
    int i=0,j=0;
    while( expr[i] == ' ' && expr[i] != '\0' ) ++i;
    if (expr[i] == '-') expr[i++] = '~';
    while( expr[i] )
    {
        if ( expr[i] == '-' )
        {
            j = i-1;
            while( expr[j] == ' ' ) --j;
            if ( !isDigit( &expr[j]) && expr[j]!=')' )
                expr[i] = '~';
        }
        ++i;
    }
}

// 当前不具备错误码输入输出功能。
// 函数功能：　	将常规中缀表达式转换为逆波兰（后缀）表达式
// 输入参数：	s 	包含中缀表达式的字符串,其中所有函数使用 funName 中对应的大写字母表示，且不含有任何其他字母。
// 输出参数：	output	包含逆波兰表达式的字符串,各元素间以空格作为间隔符。
void Polish (char const*s, char *output, error_t* error)
{  
    unsigned int outLen = 0,i=0,len = 0;  
    unsigned int top = 0,flag_e = 0;           //stack count

    char *stack = NULL;
    char *expr_in = NULL;
    
    len = 4*strlen(s) * sizeof(char);
    stack = (char*)malloc(len );
    expr_in = (char*)malloc( len);
    memset(stack,   '\0',len-1);
    memset(expr_in, '\0',len-1);
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
    strcpy(expr_in,s);
    strlwrt(expr_in);  // 将字符串中所有字母转换为小写字母
    minus2negative(expr_in);
    fun2sym(expr_in, error);
    
    if(*error != ERR_OK)
        return;
    
    while(*(expr_in+i))               //1）  
    {  
		if (isDigit(expr_in+i))
        {
            flag_e = 0;
            output[outLen++] = *(expr_in+i);        //3）假如是操作数，把它添加到输出串中。  
            while( *(expr_in+i+1) !='\0' && isDigit( expr_in+i +1 ))
            {  
                output[outLen++] = *( expr_in+i+1 );  
                ++i;
                if (*( expr_in+i ) == 'e' )
                {
                    flag_e = 1;
                    if ( *( expr_in+i+1 ) == '-' )
                    {
                        output[outLen++] = *( expr_in+i+1 );  
                        ++i;
                    }
                }
                if ( flag_e ==1 && *( expr_in+i+1 ) =='.' )  *error = ERR_INCORRECT_PARAMETERS;
            }
            output[outLen++] = ' ';     //空格隔开  
        }
        if (*(expr_in+i)=='(' || isSymFunction(*(expr_in+i)) )   //4）假如是闭括号，将它压栈。  
        {  
            ++top;  
            stack[top] = *(expr_in+i);  
        }  
        while (isOperator(*(expr_in+i)) || *(expr_in+i)=='~' )        //5)如果是运算符，执行算法对应操作；  
        {  
            if ( *(expr_in+i)=='~' && *(stack+top) == '~')
            {
                ++top;
                stack[top] = *(expr_in+i);
                break;
            }
            else if (top==0 || stack[top]=='(' || priority(*(expr_in+i)) > priority( *(stack+top))) //空栈||或者栈顶为)||新来的元素优先级更高  
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
            while (stack[top]!='(' && top >0)  
            {  
                output[outLen++] = stack[top];  
                output[outLen++] = ' ';  
                --top;  
            }  
            if (top >0)   --top;  // 此时stack[top]==')',跳过)  
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
    unsigned int top=0,i=0,len[2] = {0,0}; 
    // char *expr= (char*)malloc(2*strlen(expr_in) * sizeof(char));
	char dst[40] = {0};
	char flag = FALSE;
    // double *cSt = (double*)malloc( 2*sizeof(double)*strlen(expr_in));   	//波兰式需要用两个栈，逆波兰式只需要一个栈  
    
    char *expr= NULL;
    double *cSt = NULL;
    
    len[0] = 4 * strlen(expr_in) * sizeof(char);
    expr= (char*)malloc( len[0] );
    
    len[1] = 4 * strlen(expr_in) * sizeof(double);
    cSt = (double*)malloc( len[1] );   	//波兰式需要用两个栈，逆波兰式只需要一个栈  
    
    // no memory error test 
    if (expr == NULL || cSt == NULL )
    {
        // printf("ERROR: Memory Not Enough \n");
        *error = ERR_NOT_ENOUGH_MEMORY;
        free(expr);
        free(cSt);
        return 0;
    }
    memset(expr,'\0',len[0]-1);
    memset(cSt, 0, len[1]-1);
    
    
    Polish (expr_in, expr, error);
    // if the polish function gives error status
    // if (*error != ERR_OK)
    //     return 0;
	
	while ( *(expr+i) )  
	{  
        if (*error != ERR_OK) break;
		
        //printf( "\ns = %s ", (expr+i));
		if (*(expr+i) != ' ')  
		{  
			sscanf((expr+i),"%s",dst);  
			if (isDigit(dst))  
			{  
				++top;  
				if( isConstantNum( dst ) )
				{
					if (*dst == 'p' )  cSt[top] =3.141592653;
					else if (*dst == 'e' )  cSt[top] =2.718281828;
				}
				else
				cSt[top] = atof(dst);     //进栈  
				flag = TRUE;
			}  
			else if(isOperator(*dst))
			{  
				if (top <= 1)
                    *error = ERR_INCORRECT_PARAMETERS;
                else
                {
                    // printf("\n %f %c %f=",cSt[top-1], dst[0], cSt[top]);
                    cSt[top-1] = OP( cSt[top-1], cSt[top], dst[0], error); 
                    // printf("%f",cSt[top-1]); 
                    --top;     //操作数栈：出栈俩，进栈一 
                }
			}
			else if(isSymFunction(*dst)  || *dst=='~')
			{
				// printf("\n %c %f=", dst[0], cSt[top]);
                if (top == 0)
                    *error = ERR_INCORRECT_PARAMETERS;
                else
                    cSt[top] = OP( 0, cSt[top], dst[0], error); 
				// printf("%f",cSt[top]); 
			}
			while (*(expr+i) && *(expr+i) != ' ')  
			    ++i;
        }
        ++i;  
	}
    // printf ("top == %d",top);
    result = cSt[1];
    if ( *error == ERR_OK)
    {
        if (top >1)  *error = ERR_TOO_MANY_PARAMETERS;
        if(flag == FALSE) *error = ERR_WRONG_EXPRESSION;
        if( isnan(result) ) *error = ERR_NOT_NUMBER_RESULT;
        if( isinf(result) ) *error = ERR_INFINIT_RESULT;
    }
    free(cSt);
    cSt = NULL;
    free(expr);
    expr = NULL;
    
    return result;
}

//////////////////////////////////////////////////////////////////////////////

// 功能： 从标准输入读取字符
// 输入：in： 文件指针
// 		 *len： 字符串长度指针
// 输出：		返回动态分配后字符串首地址
char* ReadLine(FILE* in)
{
    char* line = (char*)malloc(2*sizeof(char));
    int temp;
    int len = 0;

    if(line == NULL)
    {
        printf("ERROR: Memory Not Enough \n");
        free(line);
        return NULL;
    }

    do
    {
        temp= fgetc(in);

        //考虑如果读到文件结尾后，在进行一次读写会不会出问题
        if(temp == EOF || temp == 26)
        {
            //遇到的文件为空文件
            if(len == 0)
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
        
        {
            line[len] = (char)temp;
            len++;
            line =(char*)realloc(line, ((len)+1)*sizeof(char));
            if(line == NULL)
            {
                printf("ERROR: Memory Not Enough \n");
                free(line);
                return NULL;
            }
        }
        
    }while(temp != '\n');
    
    // line[len-1] = 0;
    line[len-1] = '\0';
    return line;
 
}

// 功能： 初步判断计算需要
// 输入：line: 字符串
// 		len：字符串长度 
// 输出：是否需要计算
#define SPESYMmax 7
char spesym[SPESYMmax] ={'\0','\n','\r','\a','\b','\v','\f'};
int NeedCalculate(char const* line)
{
	int counter = 0, i = 0, len =0;
	//char lastch = TRUE;
    len = strlen(line);
    // remove the spaces
    while((line[counter] == ' ' || line[counter] == '\t') && counter<= len)
        counter++; 
    
    for ( i = 0; i < SPESYMmax; i++)
        if(line[counter] == spesym[i])//空行
        {  
            // printf("%s", line);        
            return 0;
        }
    if(line[counter] == '/' && line[counter+1] == '/')//注释行
	{
		// printf("%s", line);
		return 0;
	}	
    return 1;
}


void ProcessLine(char const* line)
{
    double result = 0;
    error_t lastError = ERR_OK;
    // error_t *lastError_t = &lastError;
    
    if (!NeedCalculate(line))
    {
      puts(line);
      return;
    }
    


    // strcpy(expr_in,line);
    // i = strlen(expr_in);
	// expr_in[i -1] = '\0';

    
    printf("%s == ", line);
    result = Calculate(line, &lastError);
    if (lastError == ERR_OK)
        printf("%lg\n", result);
    else
        ReportError(lastError);
    // free(expr_in); //这里有问题需要解决

}

int main(int argc, char const* argv[])
{
    FILE* in = stdin;
    char* line = NULL;

    //Private part
    // int len = 0;
    // int* len_p = &len;
    

    // Choose an input source
    if  (argc > 1 && (in = fopen(argv[1], "r")) == NULL)
    {
        printf("ERROR: Cannot open file '%s'", argv[1]);
		//getchar();
        return -1;
    }

    // Process the data line by line
    while (Statut_File == NORMAL && (line = ReadLine(in)) != NULL )
    {
        ProcessLine(line);
        free(line);
    }
  
    // while (((line = ReadLine(in, len_p)) != NULL) && Statut_File == NORMAL)
    // {
	// 	if(NeedCalculate(line))
	// 		ProcessLine(line);
	// 	free(line);
    // }
    //结尾处最后一行的处理
    // if(Statut_File == END_FILE)
    //     ProcessLine(line);
    // free(line);

    // Clean up
    if (in != stdin)
        fclose(in);
	//getchar();
    return 0;
}



/*
	maybe it is true
	try it!
*/

#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "ctype.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"

char prog[8000], token[8000], temp[110];
char ch, t;
int syn, p, m = 0, row, sum = 0, len, n;

const char *rwtab[32] = {"volatile", "void", "struct", "switch", "typedef", "unsigned", "union",
				   "goto", "if", "int", "long", "register", "return", "short", "signed",
				   "sizeof", "static", "else", "enum", "extern", "float", "for", "double",
				   "const", "continue", "default", "do", "auto", "break", "case", "char", "while"};
const char special[] = ";(){}[]#.?:,";
const int specialvalue[] = {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91};

void scaner();
void iskey();
int isspecial(char ch);
int isquote(char ch);

int main()
{
	int recv;
    int fd = open("1.c", O_RDONLY);
    while(recv = read(fd, temp, 50))
    {
    	temp[recv] = '\0';
    	strcat(prog, temp);
    }
    close(fd);

    FILE* fp = fopen("1.in", "w");

    p = 0;
    row = 1;
    len = strlen(prog);
    do
    {
        scaner();
        switch(syn)
        {
        	printf("syn: %d\n", syn);
        case 11: 
        	//printf("(%d, %d)\n", syn, sum); 
        	fprintf(fp, "(%d, %d)\n", syn, sum);
        	break;  
        case -1: 
        	//printf("Error in row %d\n", row);
        	fprintf(fp, "Error in row %d\n", row);
        	break;
        case -2: 
        	row++;
        	break;
        case -3:
        	break;
        default: 
        	//printf("(%d, %s)\n", syn, token);
        	fprintf(fp, "(%d, %s)\n", syn, token);
        	break;
        }
    }while(p!=len);

    fclose(fp);
}


void scaner()
{
    memset(token, 0, sizeof(token));

    ch = prog[p++];
    if(isalpha(ch) || ch=='_')
    {
        m = 0;
        while(isalnum(ch) || ch=='_')
        {
            token[m++] = ch;
            ch = prog[p++];
        }
        token[m++] = '\0';
        p--;
        syn = 10;
        iskey();
    }
    else if(isdigit(ch))
    {
        sum = 0;
        while(isdigit(ch)) 
        {
            sum = sum*10+ch-'0';
            ch = prog[p++];
        }
        p--;
        syn = 11;
        if(sum>2147483647)
            syn = -1;
    }
    else if(ch==' ' || ch=='\t')
    {
    	t = ch;
    	while(ch==t)
    	{
    		ch = prog[p++];
    	}
    	p--;
    	syn = -3;
    }
    else if(ch=='\n')
    {
    	syn = -2;
    	token[0] = '\0';
    }
    else if((n = isspecial(ch))!=-1)
    {
    	syn = 80+n;
    	token[0] = ch;
    }
    else if(isquote(ch))
    {
    	char t = ch;
    	syn = t=='\''?88:89;
    	m = 0;
    	ch = prog[p++];
    	while(ch!=t)
    	{
    		if(ch=='\\')
    		{
    			token[m++] = ch;
    			ch = prog[p++];
    		}
    		token[m++] = ch;
		    ch = prog[p++];
    	}
    }
    else if(ch=='~')
    {
        token[0] = ch;
        syn = 20;
    }
    else 
    {
    	m = 0;
    	token[m++] = ch;
    	switch(ch)
	    {
	    case '*':
	    	ch = prog[p++];
	    	if(ch=='=')
	    	{
	    		token[m++] = ch;
	    		syn = 31;
	    	}
	    	else
	    	{
	    		p--;
	    		syn = 13;
	    	}
        	break;
	    case '/':
	    	ch = prog[p++];
			if(ch=='=')
			{
				token[m++] = ch;
	    		syn = 32;
			}
			else if(ch=='*')
			{
				token[m++] = ch;
				ch = prog[p++];
				while((ch!='*' || prog[p]!='/') && m<8000)
				{
					token[m++] = ch;
					ch = prog[p++];
				}
				if(m>=8000)
				{
					syn = -1;
				}
				else
				{
					token[m++] = ch;
					token[m++] = prog[p++];
					syn = 99;
				}
			}
			else if(ch=='/')
			{
				while(ch!='\n')
				{
					token[m++] = ch;
					ch = prog[p++];
				}
				p--;
				syn = 98;
			}
			else
			{
				p--;
				syn = 14;
			}
			break;
	    case '+':
	    	ch = prog[p++];
        	if(ch=='=' || ch=='+')
        	{
        		token[m++] = ch;
        		ch = prog[p++];
        		syn = ch=='='?29:16;
        	}
        	else
        	{
        		p--;
        		syn = 15;
        	}
        	break;
	    case '-':
	    	ch = prog[p++];
        	if(ch=='=' || ch=='-')
        	{
        		token[m++] = ch;
        		ch = prog[p++];
        		syn = ch=='='?30:18;
        	}
        	else if(ch=='>')
        	{
        		token[m++] = ch;
        		ch = prog[p++];
        		syn = 71;
        	}
        	else
        	{
        		p--;
        		syn = 17;
        	}
        	break;
        case '%':
	    	ch = prog[p++];
	    	if(ch=='=')
	    	{
	    		token[m++] = ch;
	    		syn = 33;
	    	}
	    	else
	    	{
	    		p--;
	    		syn = 19;
	    	}	
        	break;
        case '!':
	    	ch = prog[p++];
	    	if(ch=='=')
	    	{
	    		token[m++] = ch;
	    		syn = 40;
	    	}
	    	else
	    	{
	    		p--;
	    		syn = 21;
	    	}	
        	break;
        case '&':
	    	ch = prog[p++];
	    	if(ch=='&' || ch=='=')
	    	{
	    		token[m++] = ch;
	    		syn = ch=='&'?60:34;
	    	}
	    	else
	    	{
	    		p--;
	    		syn = 22;
	    	}
        	break;
        case '|':
	    	ch = prog[p++];
	    	if(ch=='|' || ch=='=')
	    	{
	    		token[m++] = ch;
	    		syn = ch=='|'?62:35;
	    	}
	    	else
	    	{
	    		p--;
	    		syn = 23;
	    	}
        	break;
        case '^':
	    	ch = prog[p++];
	    	if(ch=='=')
	    	{
	    		token[m++] = ch;
	    		syn = 36;
	    	}
	    	else
	    	{
	    		p--;
	    		syn = 24;
	    	}
        	break;

        case '<':
            ch = prog[p++];
            if(ch=='>' || ch=='=')
            {
                token[m++] = ch;
                syn = ch=='>'?40:41;
            }
            else if(ch=='<')
            {
            	token[m++] = ch;
            	ch = prog[p++];
            	if(ch=='=')
            	{
            		token[m++] = ch;
            		syn = 37;
            	}
            	else
            	{
            		p--;
            		syn = 25;
            	}
            }
            else
            {
                syn = 42;
                p--;
            }
            break;
        case '>':
            ch = prog[p++];
            if(ch=='=')
            {
                token[m++] = ch;
                syn = 43;
            }
            else if(ch=='>')
            {
            	token[m++] = ch;
            	ch = prog[p++];
            	if(ch=='=')
            	{
            		token[m++] = ch;
            		syn = 38;
            	}
            	else
            	{
            		p--;
            		syn = 26;
            	}
            }
            else
            {
                syn = 44;
                p--;
            }
            break;
	        
        case '=':
        	ch = prog[p++];
        	if(ch=='=')
        	{
        		token[m++] = ch;
        		syn = 45;
        	}
        	else
        	{
        		p--;
        		syn = 39;
        	}
        	break;
		default: 
			syn = -1;
			break;
	    }
	}
}

void iskey()
{
	int i;
	for(i = 0; i < 32; i++) 
	    if(strcmp(token, rwtab[i])==0)
	    {
	        syn = 12;
	        break;
	    }
}
int isquote(char ch)
{
	if(ch=='\'' || ch=='\"')
		return 1;
	return 0;
}
int isspecial(char ch)
{
	int i;
	for(i = 0; i < 12; ++i)
	{
		if(ch==special[i])
			return i;
	}
	return -1;
}
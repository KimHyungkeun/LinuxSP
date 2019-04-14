#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


#define STACK_SIZE 10

int peek();
void push(int);
int pop();
void printStack();


int top = -1;
int stack[STACK_SIZE];
	}
 int peek(){
		return stack[top];
	}
 void push(int value){
		stack[++top] = value;
		printf("%d PUSH !\n", stack[top]);
	}
 int pop(){
		printf("%d POP !\n", stack[top]);
		return stack[top--];
	}
 void printStack(){
		printf("\n-----STACK LIST-----\n");
		for(int i=top; i>=0; i--){
		printf("%d\n",stack[i]);
		}
		printf("-----END OF LIST-----\n");
	}


int main(void){
		

push(5);
push(2);
push(3);
push(4);
push(1);
printStack();
pop();
pop();
push(15);
printf("TOP OF STACK : %d\n", peek());
printStack();
pop();
pop();
pop();
pop();
push(30);
printStack();
	}


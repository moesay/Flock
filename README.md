# Flock
#### Technically speaking, Flock messes your files in a reversible way.
---
- ### Yet another software to lock files! Why?
   Most of the file lockers are easy to break as they track the files and mark them as locked, then they watch the OS processes to catch any attempt to open that file. If somebody tried to open the file the software is triggered and asks for the password to check if its true. The rest of the process is already known to you...I guess.
   But maybe this is not the case. Maybe another software uses another technique. I don't know but no matter how the software is designed. The problem lies elsewhere.

   Consider the following ```C``` code
   ```C
  #include <stdio.h>
  int main() {
     int i, y=10;
     printf("Guess the value of y : ");
     scanf("%d", &i);
     if(i == y)
         printf("Correct number\n");
     else
         printf("Wrong number\n");
     return 0;
  }

   ```
   Compile that code to get the assembly output using ```gcc -S main.c```
   
   Now lets take a look at the assembly output
   Its a 76 lines file but we only care about few lines of it.
   
   ```assembly
   ; section (1) begin
   main:
  .LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$10, -12(%rbp)
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
  ; section (1) end
  ; section (2) begin
	leaq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
  ; section (2) end
  ; section (3) begin
	movl	-16(%rbp), %eax
	cmpl	%eax, -12(%rbp)
	jne	.L2
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
  ; section (3) end
   ```
   This is the assembly of the main function body, but we also don't care about most of it. I've splitted the code into three sections shown in the comments.
   
   The first section is to set the stack frame of the main function to be 16 bytes long, printing the first string which is ```Guess the value of y :``` and stores the value of y, which is 10, at ```[bp-12]```
   
   The second section reads the input and stores it at ```[bp-16]```.
   
   The third section, the most important one, does the comparison between ```[bp-12]``` and ```[bp-16]``` of coures after moving it to ```rax``` because you cant compare two memory locations.
   
- ### Ok then, Whats wrong!
  To translate ```if``` into its assembly representation, you do two things.
  1. Comparison
   - using ```cmp``` instruction, you can compare between two values. The result sets the flag registers and then comes the second step.
 
  2. Jumping
    - using a conditional jump like ```jne```, ```je``` or ```jae``` or any other one of them.

  In the above code, the third section jumps to submodule that prints the ```Wrong number``` string if the numbers are not the same, because that will lead ```jne``` to be triggered. If not the execution will continue to print the other message and exit. 
  
- ### And here is the problem.
  Do you know that, using a reverse engineering tool like __Olly Dbg__ or __edb__, you can change the ```jne``` to be ```je``` ?

  The effect of such a change is disastrous. Try it yourself. Change the ```jne``` to ```je``` and save the code and compile it using ```gcc -static main.s```.
  
  What happened? Now you have correctly predicted the __y value__ even if you didn't enter the correct value which is 10. And this is the problem. the ```if``` statement.
  
- ### Flock, the if-less.
  Besides being if-less, it's while-less too. Because you might have thought, Ok, i will use while instead. Good idea until you look at the assembly output figuring out that while uses ```cmp``` too.
  
  In Flock, the comparison is part of the process. The input file will always be decrypted. But it will only work if your password is correct. And this is the cost of the if-less code.
  
- ### Thats enough.
  Too much to read so i will stop, but i wanna say three last things:
    * I'm not a security engineer and i've never claimed to be an expert. Im just a guy who knows about assembly and how things work. So i wrote this code.
    * If somebody, somehow, managed to break the code, please let me know.
    * I wrote this code 6 years ago. I was still using ```C++03``` and ```std::thread``` wasn't there, yet. I've used ```p_thread``` and thats why the function parameters are void pointers. Even after compiling it with ```C++17``` compiler and changing to ```std::thread```, Im too lazy to change the parameters. Maybe later i will. 

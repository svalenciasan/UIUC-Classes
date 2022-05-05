# **Chapter 1:**  

## Question 1:  
```
#include <unistd.h>
int main() {
	write(1,"Hi! My name is Santiago",24);
	return 0;
}
```
## Question 2:  
```
#include <unistd.h>
int main() {
	void write_triangle(int n) {
		int i;
		for (i = 1; i <= n; i++) {
			int j;
			for (j = i; j > 0; j--) {
				write(1,"*",1);
			}
			write(1,"\n",1);
		}
	}
	write_triangle(10);
	return 0;
}
```
## Question 3:  
```
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	mode_t mode = S_IRUSR | S_IWUSR;
	int filedes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
	write(filedes,"Hi! My name is Santiago",24);
	close(filedes);
	return 0;
}
```
## Question 4:
```
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
	close(1);
	mode_t mode = S_IRUSR | S_IWUSR;
	int filedes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
	printf("Hi! My name is Santiago\n");
	close(filedes);
	return 0;
}
```
## Question 5:  
printf() prints out to stdout which is 1 while write() can be given a file descriptor to write to. Write() also requires that you specify the number of chars being sent which is not required for printf().

# **Chapter 2:**

## Question 1:  
at least 8 bits in a byte

## Question 2:  
1 byte

## Question 3:  
4,8,4,4,8 bytes

## Question 4:  
0x7fbd9d50

## Question 5:  
*(data+3)

## Question 6:  
ptr is in read only memory

## Question 7:  
12

## Question 8:  
5

## Question 9:  
sizeof("ab")

## Question 10:  
sizeof(5)

# **Chapter 3:**  

## Question 1:  
Using argc or looping through argv and counting.

## Question 2:  
The program execution name.

## Question 3:  
They are stored somewhere else together with command line arguments.

## Question 4:  
8 byte and 6 bytes respectively. sizeof(ptr) makes sense because we're getting the size of the pointer and sizeof(array)  makes sense because char is 1 byte and there are 5 chars plus the 1 zero value at the end.

## Question 5:  
A stack

# **Chapter 4:**

## Question 1:  
You would put it in the heap using malloc()

## Question 2:  
Heap memory is allocated during the execution of the program and requires programmers to deallocate the memory manually when done, this type of memory is contiguous. Stack memory is where temporary memorr allocation happens and does not require the programmer to de allocate manually as this memory is deallocated after the excecutions of the method/program.

## Question 3:  
Stack, heap, data segment, and text segment.

## Question 4:  
free()

## Question 5:  
Program using all heap memory

## Question 6:  
time() returns the number of seconds since 1970 as a tune_t and ctime() gives a result in ASCII.

## Question 7:  
Double freeing memory can confuse the heap's "bookeeping"

## Question 8:  
Trying to use memory that has been freed could cause errors because we don't know what is stored there anymore.

## Question 9:  
Once we free a pointer, set that pointer to null. Prevents dangling pointers.

## Question 10:  
```
struct Person {
	char* name;
	int age;
	struct Person** friends;
};

typedef struct Person person_t;
```

## Question 11:  
```
	person_t* smith = (person_t*) malloc(sizeof(person_t));
	smith->name = "Agent Smith";
	smith->age = 128;
	
	person_t* sonny = (person_t*) malloc(sizeof(person_t));
	sonny->name = "Sonny Moore";
	sonny->age = 256;
	
	(smith->friends) = malloc(sizeof(person_t*));
	*(smith->friends) = sonny;
	
	(sonny->friends) = malloc(sizeof(person_t*));
	*(sonny->friends) = smith;
```

## Question 12:  
```
person_t* create(char* aName, int aAge) {
	person_t* person = (person_t*)malloc(sizeof(person_t));
	person->name = strdup(aName);
	person->age = aAge;
	person->friends = malloc(sizeof(person_t*) * 10);
	
	return person;
}
```

## Question 13:  
```
void person_destroy(person_t* person) {
	free(person->name);
	free(person->friends);
	memset(person, 0, sizeof(person_t));
	free(person);
}
```

# **Chapter 5:**

## Question 1:  
printf, write, puts

## Question 2:  
Gets can overwrite other memory if the user inputs more than the allowed size

## Question 3:  
```
	char* data = "Hello 5 World";
	char hello[6];
	int five;
	char world[6];
	int value = sscanf(data, "%6s %d %6s", hello, &five, world);
	printf("%s %d %s\n", hello, five, world);
```

## Question 4:  
define _GNU_SOURCE, a char* buffer, and a ssize_t capacity

## Question 5:  
```
	File* file = fopen("hello_world.txt", "r");
	char* buffer = NULL;
	size_t capacity = 0;
	
	while(getline(&buffer, &capacity, file) > 0) {
		printf("line: %s\n", buffer);
	}
	free(buffer);
	fclose(file);
```
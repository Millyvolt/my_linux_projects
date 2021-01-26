
#include <stdio.h>



//int main(int argc, char **argv)
//{
//	printf("Hello!!\n");
//	
//	return 0;
//}


//void main (void)
//{
	//char input[256], name[256];
	//int age;
	//printf ("What is your name, user?\n");
	//fgets (input, 256, stdin);
	//sscanf (input, "%s", name);
	//printf ("Hello, %s. How old are you?\n", name);
	//while (1)
	//{
		//fgets (input, 256, stdin);
		//if (sscanf (input, "%d", &age) == 1) break;
		//printf ("I don't recognise that as an age - try again!\n");
	//}
	//printf ("Well, %s, you look young for %d...\n", name, age);
//}

int main (int argc, char *argv[])
{
	int param = 0;
	while (param < argc)
	{
		printf ("Parameter %d is %s\n", param, argv[param]);
		param++;
	}
	return 0;
}







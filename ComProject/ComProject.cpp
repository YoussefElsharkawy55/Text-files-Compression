#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<conio.h>
#include<windows.h>

FILE* Codes;
int height;
int counter = 0;
int counter0 = 0; //count how many zeros we added


struct Content
{
	char c;
	int frequency;
};

// struct for every char and its code
struct Cnumber
{
	char c;
	char* arr = new char[height];
	Cnumber* next;
};
Cnumber* binary_name = NULL;


// struct for the Huffman tree
struct TNODE
{
	Content data;
	TNODE* next;
	TNODE* right;
	TNODE* left;
};

//struct for the stack used to store (0,1)
struct Stack
{
	char* arr;
	int sp;
	int size;

	Stack()
	{
		arr = NULL;
		sp = -1;
		size = 0;
	}
};
Stack s;
//--------------Stack functions------------//
bool initialize(Stack* s, int n)
{
	s->arr = (char*)malloc(sizeof(char) * n);
	if (s->arr == NULL)
		return false;
	s->size = n;
	return true;
}

int push(Stack* s, char item)
{
	if (s->arr == NULL)
		return -1;
	if (s->sp + 1 == s->size)
		return -2;
	s->sp++;
	s->arr[s->sp] = item;
	return 1;
}

int pop(Stack* s, char* item)
{
	if (s->arr == NULL)
		return -1;
	if (s->sp == -1)
		return -2;
	*item = s->arr[s->sp];
	s->sp--;
	return 1;
}

bool isEmpty(Stack s)
{
	if (s.sp == -1)
		return true;
	return false;
}

void freeStack(Stack* s)
{
	free(s->arr);
	s->arr = NULL;
	s->sp = -1;
	s->size = 0;
}
//----------------------------------------------------//

//Check the Size of The FILE
long int getsize(char filename[])
{

	FILE* f = fopen(filename, "rb");


	if (f == NULL)
	{
		printf("File Not Found!\n");
		return -1;
	}

	fseek(f, 0L, SEEK_END);


	long int size = ftell(f);


	fclose(f);

	return size;
}


//-----insert_first function
TNODE* insertfirst(TNODE* h, char c)
{
	TNODE* tmp = new TNODE();
	tmp->data.c = c;
	tmp->data.frequency = 1;
	tmp->left = tmp->right = NULL;
	tmp->next = h;
	h = tmp;
	return h;
}

//insert_first for the binary_name LL
Cnumber* insertBinary(Cnumber* h, char c, char* arr)
{
	Cnumber* tmp = new Cnumber();
	tmp->c = c;
	strcpy(tmp->arr, arr);
	tmp->next = h;
	h = tmp;

	return h;
}

//frequency table function to add frequency
TNODE* addf(TNODE* h, char c) {
	bool flag = false;
	//if F.T. is empty
	if (h == NULL)
	{
		h = insertfirst(h, c);
		flag = true;
	}
	// add frequency for the character
	else
	{
		TNODE* it = h;
		while (it != NULL)
		{
			if (it->data.c == c)
			{
				it->data.frequency++;
				flag = true;
				break;
			}
			it = it->next;
		}
		// if char doesn't exist insert new node
		if (flag == false)
		{
			h = insertfirst(h, c);
		}
	}
	return h;
}

//assigning a given node to a new node
TNODE* assign(TNODE* f)
{
	TNODE* temp = new TNODE();
	temp->data.c = f->data.c;
	temp->data.frequency = f->data.frequency;
	temp->left = f->left;
	temp->right = f->right;
	return temp;
}


//priority queue function (insert)
TNODE* enqueue_priority(TNODE* h, TNODE* f)
{
	// check for the linklist
	if (h == NULL)
	{
		h = assign(f);
		h->next = NULL;
	}
	else
	{
		if (f->data.frequency <= h->data.frequency)
		{
			TNODE* temp = assign(f);
			temp->next = h;
			h = temp;
		}
		else
		{
			TNODE* it = h;
			while (it->next != NULL)
			{
				// add to P.Q depending on the the priority(frequency)
				if (f->data.frequency <= it->next->data.frequency)
				{
					TNODE* temp = assign(f);
					temp->next = it->next;
					it->next = temp;
					break;
				}
				it = it->next;
			}
			if (it->next == NULL)
			{
				TNODE* temp = assign(f);
				temp->next = it->next;
				it->next = temp;
			}
		}
	}
	return h;
}


//shift function to remove the (") character
void shiftleft(char str[], int index)
{
	for (int i = index; i < strlen(str); i++)
	{
		str[i] = str[i + 1];
	}
}

// dequeue the priority queue
TNODE* Dequeue(TNODE* h, TNODE** t)
{
	*t = h;
	h = h->next;
	(*t)->next = NULL;
	return h;
}

//building the Huffman Tree using P.Q
TNODE* Build_HuffmanTree(TNODE* h)
{
	TNODE* L, * R;
	while (h->next != NULL)
	{
		h = Dequeue(h, &L);
		h = Dequeue(h, &R);

		TNODE* temp = new TNODE();
		temp->left = L;
		temp->right = R;
		temp->data.c = '$';
		temp->data.frequency = L->data.frequency + R->data.frequency;

		h = enqueue_priority(h, temp);
	}
	// there is a single node left in the P.Q which is the root of the H.T
	return h;
}
// get the maximum height of the tree
int get_max_height(TNODE* root)
{
	//it start counting from the leaf to the root (recursion)
	if (root == NULL)
		return 0;
	else
	{
		int Lheight = get_max_height(root->left);
		int Rheight = get_max_height(root->right);
		if (Lheight > Rheight)
			return Lheight + 1;
		else
			return Rheight + 1;
	}
}

// generate a uniqe code for each character depending on its height (frequancy)
void GenerateCode(TNODE* root, char* arr, int index = 0)
{
	if (root->left)
	{
		arr[index] = '0';
		GenerateCode(root->left, arr, index + 1);
	}
	if (root->right)
	{
		arr[index] = '1';
		GenerateCode(root->right, arr, index + 1);
	}

	//leaf node found!!
	if (!root->left && !root->right)
	{
		arr[index] = '\0';
		//print on screen output
		printf("\n%c : ", root->data.c);
		//print in the .cod file
		fprintf(Codes, "%c\n", root->data.c);
		//add new node (character, character code) to the binary_name LL
		binary_name = insertBinary(binary_name, root->data.c, arr);

		for (int i = 0; i < index; i++)
		{
			printf("%c", arr[i]);
			fprintf(Codes, "%c", arr[i]);

		}
		fprintf(Codes, "\n");
	}
}
//pushing the binary code for each character in stack 
void getCode(char c)
{
	Cnumber* it = binary_name;
	while (it != NULL)
	{
		if (it->c == c)
		{
			for (int i = 0; i < strlen(it->arr); i++)
			{
				push(&s, it->arr[i]);
				counter++;
			}
			break;
		}
		else
		{
			it = it->next;
		}
	}
}

//comparing the given code with the every character code in binary_name LL
bool compare(char arr[], char* c)
{
	Cnumber* it = binary_name;
	while (it != NULL)
	{
		if (strcmp(arr, it->arr) == 0)
		{
			//character found!!
			*c = it->c;
			return true;
		}
		else
			it = it->next;
	}
	return false;
}

//free the F.T 
TNODE* freeTnode(TNODE* h)
{
	while (h != NULL)
	{
		TNODE* temp = h;
		h = h->next;
		free(temp);
	}
	return h;
}

//free binary_name 
Cnumber* freeCnumber(Cnumber* h)
{
	while (h != NULL)
	{
		Cnumber* temp = h;
		h = h->next;
		free(temp->arr);
		free(temp);
	}
	return h;
}


//free the Huffman Tree
TNODE* freeTree(TNODE* root)
{
	if (root->left)
		root->left = freeTree(root->left);
	if (root->right)
		root->right = freeTree(root->right);
	if (!root->left && !root->right)
		free(root);
	return NULL;
}


//function to clear the windows command prompt(CMD)
void cls(HANDLE hConsole)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SMALL_RECT scrollRect;
	COORD scrollTarget;
	CHAR_INFO fill;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Scroll the rectangle of the entire buffer.
	scrollRect.Left = 0;
	scrollRect.Top = 0;
	scrollRect.Right = csbi.dwSize.X;
	scrollRect.Bottom = csbi.dwSize.Y;

	// Scroll it upwards off the top of the buffer with a magnitude of the entire height.
	scrollTarget.X = 0;
	scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

	// Fill with empty spaces with the buffer's default text attribute.
	fill.Char.UnicodeChar = TEXT(' ');
	fill.Attributes = csbi.wAttributes;

	// Do the scroll
	ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

	// Move the cursor to the top left corner too.
	csbi.dwCursorPosition.X = 0;
	csbi.dwCursorPosition.Y = 0;

	SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}


int main()
{


	TNODE* FT = NULL; //frequncy table
	TNODE* pq_HT = NULL;//priority 

	// file pointers
	FILE* f;
	FILE* X;
	FILE* O;

	//window command prompet pointer initializing 
	HANDLE hStdout;
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	//user choice
	char choice = '-';


	//file location strings;
	char file_loc[100];
	char output[100];
	char compressed[100];
	char decompressed[100];
	char c;


	puts("press (1) to compress file!");
	puts("press (2) to decompress file!");
	puts("press (0) to Exit");
	printf("your choice: ");


	//programm running
	while (choice != '0')
	{
		choice = getche();//leave the screen with the character 

		if (choice == '1')
		{
			//----------------Compress---------------
			cls(hStdout);

			puts("------Compress-----");

			printf("Drop file here:\n");

			gets_s(file_loc);
			//delete the (") in the file address 
			for (int i = 0; i < strlen(file_loc); i++)
			{
				if (file_loc[i] == '"')
				{
					shiftleft(file_loc, i);
				}
			}

			long int size = getsize(file_loc);

			if (size != -1)
				printf("Size of the file is %1d bytes \n", size);
			else
				printf("Can't find the file!!");
			if (size > 2000000000)
			{
				printf(" The file size is greater than 2.0GB !!");
				return -1;
			}
			//Reading The Text File
			f = fopen(file_loc, "rb");
			if (f != NULL)
			{
				while (fscanf(f, "%c", &c) != EOF)
					FT = addf(FT, c);

			}
			else
			{
				printf("cannot open the file");
				return -2;
			}
			fclose(f);

			//prioriy queue
			TNODE* it_ft = FT;
			while (it_ft != NULL)
			{
				pq_HT = enqueue_priority(pq_HT, it_ft);
				it_ft = it_ft->next;
			}
			//build the H.T
			pq_HT = Build_HuffmanTree(pq_HT);

			height = get_max_height(pq_HT);

			//initializing the stack with size of the file and the max. height of the tree
			if (!initialize(&s, size * height))
			{
				puts("no enough memory!!");
				return -3;
			}


			char* arr = new char[height];

			strcpy(output, file_loc);
			//change the extenstion of the file to cod
			char code[] = { "doc" };

			for (int i = 0; i < 3; i++)
			{
				output[strlen(output) - 1 - i] = code[i];
			}

			strcpy(compressed, file_loc);

			//change the extenstion of the file to com
			char code2[] = { "moc" };

			for (int i = 0; i < 3; i++)
			{
				compressed[strlen(compressed) - 1 - i] = code2[i];

			}

			Codes = fopen(output, "wb");

			if (Codes == NULL)
			{
				printf("Can't create the file\n");
				return -4;
			}
			else
				GenerateCode(pq_HT, arr);
			free(arr);

			fclose(Codes);

			// read the file
			f = fopen(file_loc, "rb");
			if (f != NULL)
			{
				while (fscanf(f, "%c", &c) != EOF)
				{
					getCode(c);
				}
			}
			else
			{
				printf("cannot open the file");
				return -5;
			}
			fclose(f);
			//check the Xbinary is divisable by 8 to add zeros 
			while (counter % 8 != 0)
			{
				push(&s, '0');
				counter0++;
				counter++;
			}


			X = fopen(compressed, "wb");
			if (X != NULL)
			{

				fprintf(X, "%d", counter0);
				fprintf(X, "%c", height);
				int ASCII = 0;
				char w;
				while (!isEmpty(s))
				{
					for (int i = 0; i < 8; i++)
					{
						pop(&s, &w);
						int digit = w - '0';
						//transfer from decimal to binary 
						ASCII += digit * pow(2, 7 - i);
					}
					//trasnfer from decimal to ascii code
					w = ASCII;
					fprintf(X, "%c", w);
					ASCII = 0;
				}
			}
			else
			{
				printf("cannot open the file");
				return -6;
			}
			fclose(X);

			//free memory-----
			binary_name = freeCnumber(binary_name);
			FT = freeTnode(FT);
			pq_HT = freeTree(pq_HT);
			freeStack(&s);

			printf("\nCompression complete . . .\n");


			system("pause");

			//asking for new task----
			cls(hStdout);

			puts("press (1) to compress file!");
			puts("press (2) to decompress file!");
			puts("press (0) to Exit");
			printf("your choice: ");
		}
		else if (choice == '2')
		{
			//-----------Decompress-------------
			cls(hStdout);

			puts("-----Decompress-----");

			printf("Drop file here:\n");

			//get file location
			gets_s(file_loc);

			//removing the (") character from the string
			for (int i = 0; i < strlen(file_loc); i++)
			{
				if (file_loc[i] == '"')
				{
					shiftleft(file_loc, i);
				}
			}


			long int size = getsize(file_loc);


			//initializing the stack with size of the file
			if (!initialize(&s, size * 8))
			{
				puts("no enough memory!!");
				return -7;
			}

			//getting the location of .cod file 
			strcpy(output, file_loc);

			char code[] = { "doc" };

			for (int i = 0; i < 3; i++)
			{
				output[strlen(output) - 1 - i] = code[i];
			}



			f = fopen(file_loc, "rb");
			if (f != NULL)
			{
				int d;
				int digits[8];
				// getting number of added Zeros
				fscanf(f, "%d", &counter0);
				// getting the height of the Huffman Tree used to generate codes
				fscanf(f, "%c", &c);
				height = c;

				while (fscanf(f, "%c", &c) != EOF)
				{
					//transfer from ascii to decimal
					unsigned d = c;
					// transfer from decimal to binary
					for (int i = 7; i >= 0; i--)
					{
						digits[i] = d % 2;
						d /= 2;
					}
					// skipping the zeros that have been added in compression and pushing the rest
					for (int i = 0; i < 8; i++)
					{
						if (counter0 == 0)
						{
							push(&s, digits[i] + '0');
						}
						else
							counter0--;
					}
				}
			}
			else
			{
				printf("cannot open the file");
				return -8;
			}
			fclose(f);
			//opening the .cod file
			Codes = fopen(output, "rb");
			while (Codes == NULL)
			{
				printf("can't find (.cod) file, please drop the file here:\n");
				gets_s(output);
				for (int i = 0; i < strlen(output); i++)
				{
					if (output[i] == '"')
					{
						shiftleft(output, i);
					}
				}
				Codes = fopen(output, "rb");
			}
			//initializing a string with the height of the used H.T
			char* q = new char[height];
			char qc;
			//getting the code of each character 
			while (fscanf(Codes, "%c", &qc) != EOF)
			{
				char temp;
				fscanf(Codes, "%s", q);
				fscanf(Codes, "%c", &temp);
				binary_name = insertBinary(binary_name, qc, q);
			}
			free(q);
			fclose(Codes);


			//putting the location of the uncompressed file in the same location of the .com file
			strcpy(output, file_loc);

			char code3[] = { "txt" };

			for (int i = 0; i < 3; i++)
			{
				output[strlen(output) - 1 - i] = code3[i];
			}


			O = fopen(output, "wb");
			if (O != NULL)
			{
				char* arr = new char[height];
				strcpy(arr, "\0");
				//compare the binary code to get the character
				while (!isEmpty(s))
				{
					char x[2] = { '\0' };
					// pop and check
					pop(&s, &x[0]);
					strcat(arr, x);
					//if unmatched pop new bit in the arr until matched
					while (!compare(arr, &c))
					{
						pop(&s, &x[0]);
						strcat(arr, x);
					}
					fprintf(O, "%c", c);
					strcpy(arr, "\0");
				}
				free(arr);
			}
			else
			{
				printf("can't create file!!");
				return -9;
			}
			fclose(O);

			//free memory----
			binary_name = freeCnumber(binary_name);
			freeStack(&s);

			printf("\nDecompression complete . . .\n");


			system("pause");

			//asking for a new task
			cls(hStdout);

			puts("press (1) to compress file!");
			puts("press (2) to decompress file!");
			puts("press (0) to Exit");
			printf("your choice: ");
		}
		//Exit programm
		else if (choice == '0')
			break;
		else
		{
			puts("wrong input!!!");
			return -10;
		}
	}
	return 0;
}
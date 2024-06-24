#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<pthread.h>
#include<netinet/in.h>
#include<fcntl.h>
#include <arpa/inet.h>
#include <sys/sem.h>
#include <sys/time.h>
#define MAX_CUSTOMERS 100
#define MAX_CLIENTS 10
#define MAX_PRODUCTS 100
#define MAX_CARTSIZE 10
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
struct Product
{
	int ProductID;
	char ProductName[80];
	int quantity;
	int cost;
};
struct ProductList
{
	struct Product p1[MAX_PRODUCTS];
};
struct Cart{
    struct Product ProductsInCart[MAX_CARTSIZE];
};
struct Customer{
	int CustomerID;
	char CustomerName[100];
	char Passcode[100];
    struct Cart CustomerCart;
};
struct CustomerList{
	struct Customer C1[MAX_CUSTOMERS];
};
struct ProductList Prodarray;
struct CustomerList CustomerArray;
// Union semun definition
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
int sem_id;
// Function to create a new semaphore
int create_semaphore() {
    sem_id = semget(IPC_PRIVATE, MAX_PRODUCTS, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Error creating semaphore");
        exit(1);
    }

    union semun arg;
    arg.val = 1;

    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (semctl(sem_id, i, SETVAL, arg) == -1) {
            perror("Error initializing semaphore value");
            exit(1);
        }
    }

    return sem_id;
}

// Function to lock the semaphore
int semaphore_lock(int index) {
    struct sembuf op;
    op.sem_num = index;
    op.sem_op = -1;
    op.sem_flg = SEM_UNDO;

    if (semop(sem_id, &op, 1) == -1) {
        handle_error("Unable to lock semaphore\n");
        return -1;
    }
    return 1;
}

int semaphore_unlock(int index) {
    struct sembuf op;
    op.sem_num = index;
    op.sem_op = 1;
    op.sem_flg = SEM_UNDO;

    if (semop(sem_id, &op, 1) == -1) {
        handle_error("Unable to unlock semaphore\n");
        return -1;
    }

    return 1;
}

void generateLogFile() {
    FILE *file = fopen("log.txt", "w");
    if (file == NULL) {
        printf("Failed to open log file for writing.\n");
        return;
    }
    for (int i = 0; i < MAX_PRODUCTS; i++) {
		if(Prodarray.p1[i].ProductID!=-1){
        fprintf(file, "Product ID: %d\n", Prodarray.p1[i].ProductID);
        fprintf(file, "Product Name: %s", Prodarray.p1[i].ProductName);
        fprintf(file, "Product Cost: %d\n", Prodarray.p1[i].cost);
        fprintf(file, "Product Quantity: %d\n", Prodarray.p1[i].quantity);
        fprintf(file, "\n");
		}
    }
    fclose(file);
}
void printdatabase()
{
    for(int i=0;i<MAX_PRODUCTS;i++)
    {
		if(Prodarray.p1[i].ProductID!=-1)
        printf("The ProductID is:%d and ProductName is:%s Quantity available:%d, Cost:%d\n", Prodarray.p1[i].ProductID,Prodarray.p1[i].ProductName,Prodarray.p1[i].quantity,Prodarray.p1[i].cost);
    }
}
void printdatabase1()
{
    for(int i=0;i<MAX_CUSTOMERS;i++)
    {
		if(CustomerArray.C1[i].CustomerID!=-1)
		{
        printf("The CustomerID is:%d and CustomerName is:%s, Passcode is :%s\n", CustomerArray.C1[i].CustomerID,CustomerArray.C1[i].CustomerName,CustomerArray.C1[i].Passcode);
		}
    }
}
void addProduct(int ID, char Name[80], int instances, int price, int fd) {
    for (int i = 0; i < 100; i++) {
        if (Prodarray.p1[i].ProductID == -1) {
            Prodarray.p1[i].ProductID = ID;
            Prodarray.p1[i].cost = price;
            strcpy(Prodarray.p1[i].ProductName, Name);
            Prodarray.p1[i].quantity = instances;
            printf("ProductID:%d added into our list\n", Prodarray.p1[i].ProductID);
            break;
        }
    }
    lseek(fd, 0, SEEK_SET);  // Move the file pointer to the beginning of the file
    write(fd, &Prodarray, sizeof(struct ProductList));
    printf("Updated DataBase\n");
    printdatabase();
}
void updateProduct(int ID,int value,int code,int fd)
{
	for(int i=0;i<MAX_PRODUCTS;i++)
	{
		if(Prodarray.p1[i].ProductID==ID)
		{
			if(code==1)
			{
				Prodarray.p1[i].quantity=value;
				printf("The quantity of the Product with Product ID:%d has been updated to :%d", Prodarray.p1[i].ProductID,Prodarray.p1[i].quantity);
			}
			else if(code==2)
			{
				Prodarray.p1[i].cost=value;
				printf("The cost of Product:%d(ID) has been updated to :%d", Prodarray.p1[i].ProductID,Prodarray.p1[i].cost);
			}
			break;
		}
	}
    lseek(fd, 0, SEEK_SET);  // Move the file pointer to the beginning of the file
    write(fd, &Prodarray, sizeof(struct ProductList));
    printf("Updated DataBase\n");
    printdatabase();
	
}
void deleteProduct(int ID,int fd)
{
	for(int i=0;i<MAX_PRODUCTS;i++)
	{
		if(Prodarray.p1[i].ProductID==ID)
		{
			Prodarray.p1[i].ProductID=-1;
			printf("Product Succesfully removed from the database:%d", Prodarray.p1[i].ProductID);
			break;
		}
	}
    lseek(fd, 0, SEEK_SET);  // Move the file pointer to the beginning of the file
    write(fd, &Prodarray, sizeof(struct ProductList));
    printf("Updated DataBase\n");
    printdatabase();
}
int AddToCart(int nsd,int fd)
{
	struct Customer myCustomer;
	int ID,amount,Price;
	char ProdName[100];
	read(nsd,&ID,sizeof(ID));
	write(nsd,&ID,sizeof(ID));
	read(nsd,&amount,sizeof(amount));
	printf("Msg Rcvd, ProductID:%d and Quantity askedd for:%d\n",ID,amount);
	for(int i=0;i<MAX_PRODUCTS;i++)
	{
		if(Prodarray.p1[i].ProductID==ID)
		{
			if(Prodarray.p1[i].quantity>=amount && amount>=0)
			{
				write(nsd,"RequestValid",13);
				read(nsd,&myCustomer,sizeof(myCustomer));
				Price=Prodarray.p1[i].cost;
				strcpy(ProdName,Prodarray.p1[i].ProductName);
				break;
			}
			else
			{
				write(nsd,"InValidRequest",15);
				printf("Invalid Request Made by User\n");
				return -1;
			}
		}
		else if(i==MAX_PRODUCTS-1)
		{
			write(nsd,"InValidRequest",15);
			printf("Invalid Request Made by User\n");
			return -1;
		}
	}
	for(int i=0;i<MAX_CUSTOMERS;i++)
	{
		if(strcmp(CustomerArray.C1[i].CustomerName,myCustomer.CustomerName)==0)
		{
			for(int j=0;j<MAX_CARTSIZE;j++)
			{
				if(CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID==-1)
				{
					CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID=ID;
					CustomerArray.C1[i].CustomerCart.ProductsInCart[j].quantity=amount;
					CustomerArray.C1[i].CustomerCart.ProductsInCart[j].cost=Price;
					strcpy(CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductName,ProdName);
					printf("Product Added to Cart Successfully, ProductID:%d and quantity:%d\n",CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID,CustomerArray.C1[i].CustomerCart.ProductsInCart[j].quantity);
					break;
				}
			}
			break;
		}
	}
	lseek(fd, 0, SEEK_SET); 
	write(fd,&CustomerArray,sizeof(CustomerArray));

}
int ChangeCart(int nsd, int fd)
{
    struct Customer myCustomer;
	int ID,amount,Price;
	char ProdName[100];
	read(nsd,&ID,sizeof(ID));
	write(nsd,&ID,sizeof(ID));
	read(nsd,&amount,sizeof(amount));
	printf("Msg Rcvd, ProductID:%d and Quantity askedd for:%d",ID,amount);
	for(int i=0;i<MAX_PRODUCTS;i++)
	{
		printf("%d %d %d", i,ID,Prodarray.p1[i].ProductID);
		if(Prodarray.p1[i].ProductID==ID)
		{
			if(Prodarray.p1[i].quantity>=amount && amount>=0)
			{
				write(nsd,"RequestValid",13);
				read(nsd,&myCustomer,sizeof(myCustomer));
				Price=Prodarray.p1[i].cost;
				strcpy(ProdName,Prodarray.p1[i].ProductName);
				break;
			}
			else
			{
				printf("Invalid Request Made by User\n");
				return -1;
			}
		}
		else if(i==MAX_PRODUCTS-1)
		{
			printf("Invalid Request Made by User\n");
			return -1;
		}
	}
	for(int i=0;i<MAX_CUSTOMERS;i++)
	{
		if(strcmp(CustomerArray.C1[i].CustomerName,myCustomer.CustomerName)==0)
		{
			for(int j=0;j<MAX_CARTSIZE;j++)
			{
				if(CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID==-1)
				{
					CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID=ID;
					CustomerArray.C1[i].CustomerCart.ProductsInCart[j].quantity=amount;
					CustomerArray.C1[i].CustomerCart.ProductsInCart[j].cost=Price;
					strcpy(CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductName,ProdName);
					if(amount==0)
					{
						CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID=-1;
					}
					printf("Cart Updated Successfully, ProductID:%d and quantity:%d\n",CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID,CustomerArray.C1[i].CustomerCart.ProductsInCart[j].quantity);
					break;
				}
			}
			break;
		}
	}
	lseek(fd, 0, SEEK_SET); 
	write(fd,&CustomerArray,sizeof(CustomerArray));
}

int BuyCart(int nsd,int fd,int fd1)
{
	char Name[100];
	read(nsd,Name,sizeof(Name));
	puts(Name);
	int i=0;
	for(i=0;i<MAX_CUSTOMERS;i++)
	{
		if(strcmp(CustomerArray.C1[i].CustomerName,Name)==0)
		{
			if(CustomerArray.C1[i].CustomerCart.ProductsInCart[0].ProductID!=-1)
			{
				write(nsd,"RequestValid",13);
				break;
			}
			else
			{
				write(nsd,"InValid",8);
				return -1;
			}
		}
	}
	struct Customer LoggedinCustomer;
	read(nsd,&LoggedinCustomer,sizeof(LoggedinCustomer));
	int k=0,finalcost=0,j=0,payment=0;
	// struct sembuf sem_op; 
	while(LoggedinCustomer.CustomerCart.ProductsInCart[k].ProductID!=-1 && k<MAX_CARTSIZE)
	{
		if(semaphore_lock(k)==1)
		{
			k++;
		}
		else
		{
			printf("Buy Unsucessful\n");
			finalcost=-1;
			write(nsd,&finalcost,sizeof(finalcost));
			return -1;
		}
	}
	while(LoggedinCustomer.CustomerCart.ProductsInCart[j].ProductID!=-1 && j<MAX_CARTSIZE)
	{
		finalcost+=LoggedinCustomer.CustomerCart.ProductsInCart[j].cost*LoggedinCustomer.CustomerCart.ProductsInCart[j].quantity;
		for(int i=0;i<MAX_PRODUCTS;i++)
		{
			//printf("%d %d\n",Prodarray.p1[j].quantity,LoggedinCustomer.CustomerCart.ProductsInCart[j].quantity);
			if(Prodarray.p1[i].ProductID==LoggedinCustomer.CustomerCart.ProductsInCart[j].ProductID)
			{
				Prodarray.p1[i].quantity=Prodarray.p1[i].quantity-LoggedinCustomer.CustomerCart.ProductsInCart[j].quantity;
			}
			//printf("%d %d\n",Prodarray.p1[i].quantity,LoggedinCustomer.CustomerCart.ProductsInCart[j].quantity);
		}
		j++;
	}
	for(int x=0;x<MAX_CARTSIZE;x++)//empty cart...
	{
		CustomerArray.C1[i].CustomerCart.ProductsInCart[x].ProductID=-1;
	}
	write(nsd,&finalcost,sizeof(finalcost));
	read(nsd,&payment,sizeof(payment));
	if(payment>=finalcost)
	{
		write(nsd,"Buy Success",12);
		lseek(fd1, 0, SEEK_SET);
    	write(fd1, &Prodarray, sizeof(struct ProductList));
		lseek(fd, 0, SEEK_SET);
    	write(fd, &CustomerArray, sizeof(struct CustomerList));
		generateLogFile();
		k=0;
		while(LoggedinCustomer.CustomerCart.ProductsInCart[k].ProductID!=-1 && k<MAX_CARTSIZE)
		{
			if(semaphore_unlock(k)==1)
			{
				k++;
			}
		}
		return 0;
	}
	else{
		write(nsd,"Payment Insufficient",21);
		while(LoggedinCustomer.CustomerCart.ProductsInCart[k].ProductID!=-1 && k<MAX_CARTSIZE)
		{
			if(semaphore_unlock(k)==1)
			{
				k++;
			}
		}
		return -1;
	}
}
int DisplayCart(int nsd)
{
	char Name[100];
	read(nsd,Name,sizeof(Name));
	puts(Name);
	for(int i=0;i<MAX_CUSTOMERS;i++)
	{
		if(strcmp(CustomerArray.C1[i].CustomerName,Name)==0)
		{
			for(int j=0;j<MAX_CARTSIZE;j++)
			{
				printf("%d\n",CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID);
			}
			write(nsd,&CustomerArray.C1[i].CustomerCart,sizeof(CustomerArray.C1[i].CustomerCart));
		}
	}
}
int handle_clientservices(int nsd,int fd,int fd1)
{
	int choice;
	while(1){
		read(nsd,&choice,sizeof(choice));
		printf("The User choice is %d\n",choice);
		write(nsd,&choice,sizeof(choice));//for verification by client side..
		printf("%d\n",choice);
		switch (choice)
    	{
			case 1:
				break;
    	    case 2:
    	        DisplayCart(nsd);
    	        break;
    	    case 3:
    	        ChangeCart(nsd,fd);
    	        break;
    	    case 4:
    	        BuyCart(nsd,fd,fd1);
    	        break;
    	    case 5:
    	        AddToCart(nsd,fd);
    	        break;
			case 6:
				close(nsd);
    			pthread_exit(NULL);
				return 0;
    	    default:
    	        printf("User Choice Invalid\nSession Terminated\n");
    	        return -1;
    	}
	}
	return 0;
}
int handle_newuser(int nsd,struct Customer new,int fd,int fd1)
{
	printf("%d",fd);
	for(int i=0;i<MAX_CUSTOMERS;i++)
	{
		if(CustomerArray.C1[i].CustomerID==-1)
		{
			CustomerArray.C1[i].CustomerID=1;
			strcpy(CustomerArray.C1[i].CustomerName,new.CustomerName);
			strcpy(CustomerArray.C1[i].Passcode,new.Passcode);
			printf("New User:%s, With Passcode:%s Added Successfully\n", CustomerArray.C1[i].CustomerName,CustomerArray.C1[i].Passcode);
			write(nsd,"User Added Successfully",24);
			for(int j=0;j<MAX_CARTSIZE;j++)
			{
				CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID=-1;
			}
			// for(int j=0;j<MAX_CARTSIZE;j++)
			// {
			// 	printf("%d\n",CustomerArray.C1[i].CustomerCart.ProductsInCart[j].ProductID);
			// }
			break;
		}
	}
	lseek(fd, 0, SEEK_SET);  // Move the file pointer to the beginning of the file
    write(fd, &CustomerArray, sizeof(struct CustomerList));
    printf("Updated DataBase\n");
	return handle_clientservices(nsd,fd,fd1);
}
int handle_admin(int fd) 
{
	printf("Welcome Admin!!\n Products in Database are as follows:\n");
	printdatabase();
	while (1) {
		printf("Hey Admin!\n To Add a Product, ENTER 2\n");
		printf("To Update an Existing Product, ENTER 3\n");
        printf("To delete a Product, ENTER 4\n");
		printf("To Exit, ENTER 1\n");
		char Name[100];
		int cost, quantity, code, value, choice, ProductID;
		scanf("%d", &choice);
		getchar();  // Consume the newline character from the input buffer

		switch (choice) {
			case 1:
				generateLogFile();
				printf("Successfully Exited and LogFile Generated, See You Again!!\n");
				return -1;
			case 2:
				printf("ENTER Product Details\n");
				printf("ProductID: ");
				scanf("%d", &ProductID);
				getchar();  // Consume the newline character from the input buffer
				printf("Name: ");
				fgets(Name, sizeof(Name), stdin);
				printf("Quantity: ");
				scanf("%d", &quantity);
				printf("Cost: ");
				scanf("%d", &cost);
				addProduct(ProductID, Name, quantity, cost, fd);
				break;
			case 3:
				printf("What do you want to update?\n");
				printf("For Quantity, ENTER 1\n");
				printf("For Cost, ENTER 2\n");
				scanf("%d", &code);
				printf("ENTER Product Details:\n");
				printf("ProductID: ");
				scanf("%d", &ProductID);
				printf("ENTER The New Value:\n");
				scanf("%d", &value);
				updateProduct(ProductID, value, code,fd);
				break;
            case 4:
                printf("ENTER The ProductID Of The Product You Wish To Delete:");
                scanf("%d",&value);
                deleteProduct(value,fd);
			default:
				printf("Invalid Choice\n");
		}
	}
}
int handle_clientverification(void* arg)
{
    int* args = (int*)arg;
    int fd = args[2];//CustomerDataBase...
    int nsd = args[1];
	int fd1=args[0];//Productdatabase....
    free(arg);
	char role[6];
	memset(role,0,sizeof(role));
	if(read(nsd,role,sizeof(role))<=0)
	{
		handle_error("read");
	}
    //printf("New client connected with socket file descriptor: %d\n", nsd);
	//printf("Client Role:%s\n",role);
	//For admin...
	if(strcmp(role,"Admin")==0)
	{
		write(nsd,"Message Rcvd. Client Role:Admin",32);
		handle_admin(fd1);
	}
	//For User...
	else if(strcmp("User",role)==0)
	{
		int choice;
		write(nsd,"Message Rcvd. Client Role:User",31);
		read(nsd,&choice,sizeof(choice));
		printf("Msg Rcvd. choice: %d\n",choice);
		write(nsd,&choice,sizeof(choice));
		// printf("%d\n",fd);
		// printf("%d\n", nsd);
		struct Customer tester;
		read(nsd, &tester, sizeof(struct Customer));
		puts(tester.Passcode);
		puts(tester.CustomerName);
		perror("read");
		if(choice==2)
		{
			return handle_newuser(nsd,tester,fd,fd1);
		}
		else if(choice==1)
		{
			printf("Verifying User....\n");
			//printf("tester.Name:%s and Passcode:%s\n",tester.CustomerName,tester.Passcode);
			printf("Received UserName and Password: %s, %s\n", tester.CustomerName,tester.Passcode);
			int flag = 0;
			for (int i = 0; i < MAX_CUSTOMERS; i++) {
				if ((strcmp(CustomerArray.C1[i].CustomerName, tester.CustomerName) == 0) && strcmp(CustomerArray.C1[i].Passcode,tester.Passcode)==0) {
					write(nsd, "User Exists...", 15);
					printf("User Logged in Successfully\n");
					flag = 1;
					break;
				}
			}
			if (flag == 0) {
				printf("Invalid Credentials\n");
				write(nsd, "Invalid Credentials", 20);
				close(nsd);
    			pthread_exit(NULL);
				return -1;
			} 
			else return handle_clientservices(nsd,fd,fd1);
		}
		else
		{
			close(nsd);
    		pthread_exit(NULL);
			return -1;
		}
	}	
	else	
	{
		close(nsd);
    	pthread_exit(NULL);
		return -1;
	}
}
int main()
{
	sem_id=create_semaphore();
	char command[] = "rm '\023\273'";
    int result = system(command);
    if (result == -1) {
        printf("Command execution failed.\n");
        return 1;
    }
    printf("Command executed successfully.\n");
	int fd,fd1;
	fd=open("ProductDataBase",O_RDWR|O_CREAT|O_EXCL,0744);
	perror("open");
	if(fd!=-1)
	{
		for(int i=0;i<MAX_PRODUCTS;i++)
		{
			Prodarray.p1[i].ProductID=-1;
		}
	    write(fd,&Prodarray,sizeof(struct ProductList));
		perror("write");
	}
	if(fd==-1)
	{
		fd=open("ProductDataBase",O_RDWR);
        read(fd, &Prodarray, sizeof(struct ProductList));
        perror("read");
	}
	fd1=open("CustomerDataBase",O_RDWR|O_CREAT|O_EXCL,0744);
	perror("open");
	if(fd1!=-1)
	{
		for(int i=0;i<MAX_CUSTOMERS;i++)
		{
			CustomerArray.C1[i].CustomerID=-1;
		}
	    write(fd1,&CustomerArray,sizeof(struct CustomerList));
		perror("write");
	}
	if(fd1==-1)
	{
		fd1=open("CustomerDataBase",O_RDWR);
        read(fd1, &CustomerArray,sizeof(struct CustomerList));
        perror("read");
	}
	printf("%d\n", fd1);
	printdatabase();
	printdatabase1();
	//In our case we consider that we max.no of different products are 100..of course these can be extended as per our need...
	//socketprogramming code...
	struct sockaddr_in serv,client;
	int sd,nsd;
	sd=socket(AF_UNIX,SOCK_STREAM,0);
	if(sd==-1)
	{
		handle_error("Socket Descriptor:");
	}
	int reuseaddr = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) == -1) {
    	handle_error("setsockopt");
	}
	serv.sin_family=AF_UNIX;
	serv.sin_port=htons(5051);
	serv.sin_addr.s_addr=INADDR_ANY;//INADDR_ANY in this case, which means it will listen on all available network interfaces of the given system..
	if(bind(sd,(struct sockaddr *)(&serv),sizeof(serv))==-1)
	{
		handle_error("Bind:");
	}
	if(listen(sd,MAX_CLIENTS)==-1)
	{
		handle_error("Listen:");
	}
	int i=0;
	pthread_t threads[MAX_CLIENTS];
	int socket_descriptor;
	while (1) {
        // Accept a new connection
		int sz=sizeof(client);
        if ((socket_descriptor= accept(sd, (struct sockaddr*)&client, &sz))==-1) {
           handle_error("Accept Falied!");
        }
		int* args = malloc(3 * sizeof(int));
		args[0]=fd;
		args[1]=socket_descriptor;
		args[2]=fd1;
		int result;
		// Create a new thread to handle the client
		if (result=pthread_create(&threads[socket_descriptor], NULL, (void*)handle_clientverification, (void*)args) != 0)
		{
            handle_error("Thread Creation For User Unsuccessful!\n Connection Terminated");
        }
		// result = pthread_join(threads[socket_descriptor], NULL);
    	// if (result != 0) {
        // 	printf("Thread join failed\n");
    	// }
    }
	return 0;
}
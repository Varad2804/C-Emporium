#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<pthread.h>
#include<netinet/in.h>
#include <stdio.h>
#include <signal.h>
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define MAX_CARTSIZE 10
struct Product
{
	int ProductID;
	char ProductName[80];
	int quantity;
	int cost;
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
struct Customer user;
void generateLogFile()
{
    FILE *file = fopen("receipt.txt", "w");
    if (file == NULL) {
        printf("Failed to create the log file.\n");
        return;
    }
    
    fprintf(file, "Customer Name: %s\n\n", user.CustomerName);
    fprintf(file, "Products in Cart:\n");

    for (int i = 0; i < MAX_CARTSIZE; i++) {
        if (user.CustomerCart.ProductsInCart[i].ProductID != -1) {
            fprintf(file, "Product ID: %d\n", user.CustomerCart.ProductsInCart[i].ProductID);
            fprintf(file, "Product Name: %s\n", user.CustomerCart.ProductsInCart[i].ProductName);
            fprintf(file, "Quantity: %d\n", user.CustomerCart.ProductsInCart[i].quantity);
            fprintf(file, "Cost: %d\n", user.CustomerCart.ProductsInCart[i].cost*user.CustomerCart.ProductsInCart[i].quantity);
            fprintf(file, "\n");
        }
    }

    fclose(file);
    //printf("Receipt generated successfully. Please check the 'receipt.txt' file.\n");
}

void DisplayProducts() {
    FILE *file = fopen("log.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        printf("%s", line);
    }
    fclose(file);
}
int DisplayCart(int sd)
{
    int choice;
    struct Cart Cart1;
    char Name[100];
    strcpy(Name,user.CustomerName);
    read(sd,&choice,sizeof(choice));
    printf("Server:Choice of User1 %d\n",choice);
    write(sd,Name,sizeof(Name));
    read(sd,&Cart1,sizeof(Cart1));
    //printf("The username is: %s and the Passcode is :%s\n", user.CustomerName,user.Passcode);
    for(int i=0;i<MAX_CARTSIZE;i++)
    {
        if(Cart1.ProductsInCart[i].ProductID!=-1)
        printf("ProductID:%d, ProductName:%s, Quantity:%d, Cost:%d\n",Cart1.ProductsInCart[i].ProductID,Cart1.ProductsInCart[i].ProductName,Cart1.ProductsInCart[i].quantity,Cart1.ProductsInCart[i].cost);
    }
    return 0;
}
int AddToCart(int sd)
{
    int ProductID,choice,quantity;
    int arr[2];
    char buf[80];
    printf("Enter the ProductID of the Product\n");
    scanf("%d",&ProductID);
    read(sd,&choice,sizeof(choice));
    printf("Server:Choice of User %d\n",choice);
    write(sd,&ProductID,sizeof(ProductID));
    printf("Enter The Quantity of The Product:\n");
    scanf("%d",&quantity);
    read(sd,&ProductID,sizeof(ProductID));
    write(sd,&quantity,sizeof(quantity));
    read(sd,buf,sizeof(buf));
    if(strcmp(buf,"RequestValid")==0)
    {
        printf("Product Being added To Cart\n");
        write(sd,&user,sizeof(user));
        printf("Add to Cart Success\n");
        return 0;
    }
    else
    {
        printf("Request Invalid\n");
        return -1;
    }
}
int ChangeCart(int sd)
{
    int ProductID, choice, quantity;
    char buf[80];

    printf("Enter the ProductID of the Product to be changed: ");
    scanf("%d", &ProductID);

    read(sd, &choice, sizeof(choice));
    //printf("Server: Choice of User %d\n", choice);
    printf("Enter the new Quantity of the Product: ");
    scanf("%d", &quantity);

    read(sd, buf, sizeof(buf));
    if (strcmp(buf, "RequestValid") == 0) {
        printf("Changing Product Quantity in Cart\n");
        write(sd, &user, sizeof(user));
        printf("Change Cart Success\n");
        return 0;
    } else {
        printf("Request Invalid\n");
        return -1;
    }
}

int BuyCart(int sd)
{
    int choice;
    char buf[100];
    int payment,cost;
    read(sd, &choice, sizeof(choice));
    printf("Server: Choice of User %d\n", choice);
    strcpy(buf,user.CustomerName);
    write(sd,buf,sizeof(buf));
    memset(buf, 0, sizeof(buf));
    read(sd,buf,sizeof(buf));
    if(strcmp(buf,"RequestValid")==0)
    {
        //printf("User RequestValid\n");
        write(sd,&user,sizeof(user));
        read(sd,&cost,sizeof(cost));
        if(cost==-1)
        {
            //printf("Buy not Possible as Server is Busy\n");
            return -1;
        }
        else
        {   
            // printf("Total Cost is finally:%d\n",cost);
            // printf("To Buy Please Enter please pay;\n");
            scanf("%d",&payment);
            write(sd,&payment,sizeof(payment));
            memset(buf, 0, sizeof(buf));
            read(sd,buf,sizeof(buf));
            if(strcmp(buf,"Buy Success")==0)
            {
                generateLogFile();
                //printf("Successfully Purchased\n");
                return 0;
            }
            else
            {
                printf("Buy Invalid\n");
                return -1;
            }
        }
    }
    else
    {
        //printf("No Products To Buy\n");
        return -1;
    }
}
int handle_clientservices(int sd)
{
    while(1)
    {
        printf("a.To See Products Available, Enter 1\n");
        printf("b.To See Your Cart, Enter 2\n");
        printf("c.To Change Your Cart, Enter 3\n");
        printf("d.To Buy The Chosen Cart, Enter 4\n");
        printf("e.To Add Products To Your Cart, Enter 5\n");
        printf("f.To Logout, ENTER 6\n");
        int choice;
        scanf("%d",&choice);
        write(sd,&choice,sizeof(choice));
        switch (choice)
        {
        case 1:
            DisplayProducts();
            break;
        case 2:
            DisplayCart(sd);
            break;
        case 3:
            ChangeCart(sd);
            break;
        case 4:
            BuyCart(sd);
            break;
        case 5:
            AddToCart(sd);
            break;
        case 6:
            return 0;
            break;
        default:
            //printf("User Choice Invalid\nSession Terminated\n");
            return -1;
        }
    }
}
int LogUser(char UserName[100], char Passcode[100], int sd){
    user.CustomerID=1;
    strcpy(user.CustomerName, UserName);
    strcpy(user.Passcode, Passcode);
    // printf("The username is: %s and the Passcode is :%s\n", user.CustomerName,user.Passcode);
    // printf("Verifying User Credentials....\n");
    write(sd, &user, sizeof(struct Customer));
    char buf[80];
    memset(buf, 0, sizeof(buf));
    read(sd, buf, sizeof(buf));
    //printf("%s\n", buf);
    if (strcmp(buf, "User Exists...") != 0) {
        printf("Invalid Credentials\n");
        return -1;
    }
    return 0;
}
int adduser(char UserName[100], char Passcode[100], int sd) {
    user.CustomerID = 0; // Set an appropriate ID for the new user(will be done by server code..)
    strcpy(user.CustomerName, UserName);
    strcpy(user.Passcode, Passcode);
    printf("Registering New User....\n");
    write(sd, &user, sizeof(struct Customer));
    char buf[80];
    memset(buf, 0, sizeof(buf));
    read(sd, buf, sizeof(buf));
    printf("%s\n", buf);
    if (strcmp(buf, "User Added Successfully") == 0) {
        printf("User add Success\n");
        return 1; // User added successfully
    } else {
        printf("Failed to add user\n");
        return -1; // Failed to add user
    }
}
int handle_user(int sd) {
    printf("Welcome!! User\n");
    printf("ENTER 1, If You Are an Existing User\n");
    printf("ENTER 2, IF You Are a New User (Unregistered User)\n");

    int choice,choice1;
    char username[100];
    char Passcode[100];
    scanf("%d", &choice);
    write(sd,&choice,sizeof(choice));
    read(sd,&choice1,sizeof(choice1));
    printf("Server: User choice:%d",choice1);
    switch (choice) {
        case 1:
            printf("Enter Your Username: ");
            getchar();
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = '\0'; // Remove newline character

            printf("Enter Your Passcode: ");
            fgets(Passcode, sizeof(Passcode), stdin);
            Passcode[strcspn(Passcode, "\n")] = '\0'; // Remove newline character

            if (LogUser(username, Passcode, sd) == 0) {
                printf("User Logged in Successfully!\n");
                return handle_clientservices(sd);
            } else {
                printf("Invalid Input\nSession Terminated\n");
                return -1;
            }
            break;

        case 2:
            printf("Enter Your Username: ");
            getchar();
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = '\0'; // Remove newline character

            printf("SET Your Passcode: ");
            fgets(Passcode, sizeof(Passcode), stdin);
            Passcode[strcspn(Passcode, "\n")] = '\0'; // Remove newline character

            if (adduser(username, Passcode, sd) == 1) {
                printf("User Registered Successfully\n");
                handle_clientservices(sd);
            }
            break;
        default:
            printf("Invalid Input\nSession Terminated\n");
            return -1;
    }

    return 0;
}
int handle_admin(int sd)
{

}
int handle_admin(int sd) 
{
	printf("Welcome Admin!!\n Products in Database are as follows:\n");
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
				addProduct(ProductID, Name, quantity, cost, args[0]);
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
				updateProduct(ProductID, value, code,args[0]);
				break;
            case 4:
                printf("ENTER The ProductID Of The Product You Wish To Delete:");
                scanf("%d",&value);
                deleteProduct(value,args[0]);
			default:
				printf("Invalid Choice\n");
		}
	}
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    // Ignore the SIGINT signal using sigaction
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("Failed to set sigaction for SIGINT.\n");
        return 1;
    }
    int sd;
    char buf[80];
    struct sockaddr_in server;
    sd=socket(AF_UNIX,SOCK_STREAM,0);
    if(sd==-1)
    {
        handle_error("Socket Descriptor:");
    }
	server.sin_family=AF_UNIX;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(5051);
	if(connect(sd,(void *)(&server), sizeof(server))==-1)
	{
		handle_error("Server Not Reachable");
    }
    printf("Establishing connection to server Success\n");
    int choice;
    //printf("ENTER Your Role. Admin or User?\n");
    printf("Are you a user or the admin? Press 1 for user, 2 for admin\n");
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        write(sd,"User",4);
        memset(buf,0,sizeof(buf));
        read(sd,buf,sizeof(buf));
        puts(buf);
        handle_user(sd);
        break;
    case 2:
        write(sd,"Admin",5);
        memset(buf,0,sizeof(buf));
        read(sd,buf,sizeof(buf));
        puts(buf);
        handle_admin(sd);
        break;
    default:
        printf("Invalid Input\nSession Terminated\n");
        write(sd,"Invalid",8);
        break;
    }
}

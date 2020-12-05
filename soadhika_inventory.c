// Author: Soham Adhikary
// R11639607
// Project 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define SIZE 100
#define MAX_LEN 255

struct Item{
  int key; //identification number
  char name[100]; //name of itme
  int threshold; //point store needs to order more for that item
  int stock;
  float price; //price
};

struct GItem{
  int key;
  int amount;
};

struct Customer{
  char  name[100];
  float cash;
  struct GItem *groceryList[100];
  int listLength;
  int sold;
};


struct Item* hashTable[SIZE];
struct Item* tmpItem;
struct Customer *customers[100];
int numCustomers;
char restockMessages[100];
char choice[100];
//creates the hash
int hash(int key) {
   return key % SIZE;
}

//get item from table with given key
struct Item *get(int key) {
   int index = hash(key);

   while(hashTable[index] != NULL) {
      if(hashTable[index]->key == key){
        return hashTable[index];
      }

      ++index;
      index %= SIZE;
   }

   return NULL;
}

char *trimStr(char *str){
  char *end;

  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  end[1] = '\0';
  return str;
}
void insertItem(int key, char name[100] ,int threshold , int stock , float price) {

   //get the hash
   int index = hash(key);
   struct Item *item = (struct Item*) malloc(sizeof(struct Item));

   item->key = key;
   strcpy(item->name, name);
   item->threshold = threshold;
   item->stock = stock;
   item->price = price;

   //find empty
   while(hashTable[index] != NULL && hashTable[index]->key != -1) {
      index++;
		  index %= SIZE;
   }

   hashTable[index] = item;
}

struct Item* deleteItem(int key) {


   //get the hash
   int index = hash(key);

   //find empty
   while(hashTable[index] != NULL) {

      if(hashTable[index]->key == key) {
         struct Item* temp = hashTable[index];

         //hold empty position
         hashTable[index] = NULL;
         return temp;
      }

      //go to next cell
      index++;
      index %= SIZE;
   }

   return NULL;
}

//reads items to table
void readItems(){
    FILE* fp;
    char *token;

    fp = fopen("Inventory.txt", "r");
    if (fp == NULL) {
      perror("Failed to open file: ");
    }

    char buffer[MAX_LEN];
    char name[100];
    while (fgets(buffer, MAX_LEN - 1, fp)){

        buffer[strcspn(buffer, "}") + 0] = ' ';
        buffer[strcspn(buffer, "\n")] = 0;
        buffer[0] = ' ';

        token = strtok(buffer, ",");
        int key = atoi(token);

        token = strtok(NULL, ",");
        strcpy(name, trimStr(token));

        token = strtok(NULL, ",");
        int threshold = atoi(token);

        token = strtok(NULL, ",");
        int stock = atoi(token);

        token = strtok(NULL, ",");
        float price = atof(token);


        insertItem(key,  name ,threshold ,  stock , price);
    }

    fclose(fp);
}

void printInventoryMenu(){
  printf(">add item (syntax: add ​ key name threshold stock price )\n>delete item (syntax: delete ​ key​ )\n>restock item (syntax: restock ​ key amount​ )\n>restock all (syntax: restock all)\n>return to main menu (syntax: return)\n");
}

void printMainMenu(){
  printf("chekout customers (syntax : checkout filename)\nManage Inventory (syntax : inventory)\nClose Program (syntax : quit)\n>");
}

void addItem(char args[]){
      char name[100];
      char *token;

      token = strtok(args, " ");

      token = strtok(NULL, " ");
      int key = atoi(token);

      token = strtok(NULL, " ");
      strcpy(name, token);

      token = strtok(NULL, " ");
      int threshold = atoi(token);

      token = strtok(NULL, " ");
      int stock = atoi(token);

      token = strtok(NULL, " ");
      float price = atof(token);

      struct Item *item = get(key);
      if(item != NULL){
        printf("Failed : an item with that key already exists");
        return;
      }

      insertItem(key,  name ,threshold ,  stock , price);
}

void deleteFromList(char args[]){
    char *token;
    token = strtok(args, " ");
    token = strtok(NULL, " ");
    int key = atoi(token);
    deleteItem(key);
}

void restock(char args[]){
    char *token;
    token = strtok(args, " ");
    token = strtok(NULL, " ");
    int key = atoi(token);
    token = strtok(NULL, " ");
    int num = atoi(token);

    struct Item *item = get(key);
    item->stock += num;
}

void restockAll(){

   //get the hash
   int index = hash(0);

   //find empty
   while(index < SIZE) {
      if(hashTable[index] != NULL && hashTable[index]->stock < hashTable[index]->threshold){
         hashTable[index]->stock = hashTable[index]->threshold;
      }

      //go to next cell
      index++;
   }


}

void inventorySystem(){
  int isON = 1;

  printInventoryMenu();
  while (isON){
     printf("\n> ");
     memset(choice,0,strlen(choice));
     fgets(choice, 100, stdin);

     if(strstr(choice,"add")){
        addItem(choice);
     }else if (strstr(choice,"delete")){
        deleteFromList(choice);
     }else if (strstr(choice,"restock all")){
        restockAll();
     }else if(strstr(choice,"restock")){
        restock(choice);
     }else if(strstr(choice,"return")){
        return;
     }else{
        printf("Invalid selection\n");
     }
  }

}

void doSave(){

   FILE *file = fopen("Inventory.txt", "w");
   //get the hash
   int index = hash(0);

   //find empty
   while(index < 100) {

	   if(hashTable[index] != NULL){
         fprintf(file, "{ %d , %s, %d, %d, %.2f}\n",hashTable[index]->key,hashTable[index]->name,hashTable[index]->threshold,hashTable[index]->stock,hashTable[index]->price);
      }
      //go to next cell
      index++;
   }

   fclose(file);
}

void printReceipts(){
   FILE *file = fopen("receipts.txt","w");

   for (int i = 0; i < numCustomers; i++) {
       struct Customer *customer = customers[i];
       float total = 0;

       fprintf(file,"Customer - %s\n\n\n",customer->name);

       for(int j = 0;j < customer->listLength;j++){
         struct Item *it = get(customer->groceryList[j]->key);
         float am = (it->price * customer->groceryList[j]->amount);
         total += am;
         fprintf(file,"%s X %d @ $%.2f\n",it->name,customer->groceryList[j]->amount,it->price);
      }

      fprintf(file,"\nTotal: $%.2f\n",total);
      if(customer->sold == 0){
         fprintf(file,"Customer did not have enough money and was REJECTED\n");
      }
      fprintf(file,"Thank you, come back soon !\n");
      fprintf(file,"--------------------------------------------------------------------\n\n\n");
   }
   fclose(file);
}

void checkoutSystem(char file[]){
   file[strcspn(file, "\n")] = 0;
   FILE* fp = fopen(file, "r");

    if (fp == NULL) {
      perror("Failed to open file: ");
      return;
    }
    char buffer[MAX_LEN];
    char *token;

    char *name;
    float money;
    char *back;
    while (fgets(buffer, MAX_LEN - 1, fp)){
        struct Customer *customer = (struct Customer*)malloc(sizeof(struct Customer));
        customer->sold = 0;
        customer->listLength = 0;
        buffer[0] = ' ';
        token = strtok(buffer, "[");

        back = strtok(NULL,"[");

        name = strtok(token,",");
        strcpy(customer->name,name);

        token = strtok(NULL,",");
        money = atof(token);
        customer->cash = money;


        char ch = back[0];
        int i = 0;

        while (ch != ']'){
           ch = back[i];

           if(ch == ']')break;
           if(ch == '{'){

              i++;
              int k = 0;
              char found[10];
              found[10] = '\0';
              while (ch != '}'){
                ch = back[i];
                found[k] = ch;
                i++;
                k++;
                if(back[i] == '}')break;
              }
              struct GItem *item = (struct GItem*)malloc(sizeof(struct GItem));
              token = strtok(found,",");
              item->key = atoi(token);
              token = strtok(NULL,",");
              item->amount = atoi(token);
              customer->groceryList[customer->listLength++] = item;

           }
           if(back[i] != ']')i++;
        }
        customers[numCustomers++] = customer;
    }

    fclose(fp);

    for(int i = 0; i < numCustomers;i++){
      struct Customer *customer = customers[i];
      float cusTotal = 0;

      //get total

      for(int j = 0;j < customer->listLength;j++){
        struct Item *it = get(customer->groceryList[j]->key);
        cusTotal += (it->price * customer->groceryList[j]->amount);
      }

      if(cusTotal <= customer->cash){
           customer->sold = 1;
           for(int j = 0;j < customer->listLength;j++){
              struct Item *it = get(customer->groceryList[j]->key);
              it->stock -= customer->groceryList[j]->amount;
           }
      }

    }

    printReceipts();
    numCustomers = 0;

}


int main(){
     numCustomers = 0;

    tmpItem = (struct Item*) malloc(sizeof(struct Item));
    readItems();

    int isOn = 1;
    while (isOn) {
       printMainMenu();

       //read from item
       memset(choice,0,strlen(choice));
       fgets(choice, 100, stdin);
       //do action

       if(strstr(choice,"inventory")){
          inventorySystem();
       }else if (strstr(choice,"checkout")){
          char *file = strtok(choice," ");
          file = strtok(NULL," ");
          checkoutSystem(file);
       }else if (strstr(choice,"quit")) {
         doSave();
         break;
       }else{
          printf("Invalid selection\n");
       }

    }

    return 0;
}

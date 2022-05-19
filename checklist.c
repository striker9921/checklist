// main file of the checlist project.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const char DEBUG = 0;
const char *CSV_HEADER = "Task Name,Deadline,ETA,Priority,Custom Weight\n";
const int LINE_LIMIT = 50;

struct ChecklistItem {
    char *name;
    int deadline;
    int eta;
    int priority;
    int custom;
};

struct ListNode {
    struct ChecklistItem *item;
    struct ListNode *next;
};

struct ListNode *head;

struct ChecklistItem *stringToTask(char *str) {

	printf("Parsing string: %s\n", str);

	struct ChecklistItem *temp = malloc(sizeof(struct ChecklistItem));
	temp->name = malloc(sizeof(char) * (LINE_LIMIT-22));
	strcpy(temp->name, strtok(str, ","));
	temp->deadline = atoi(strtok(NULL, ","));
	temp->eta = atoi(strtok(NULL, ","));
	temp->priority = atoi(strtok(NULL, ","));
	temp->custom = atoi(strtok(NULL, ","));
	return temp;
}

//char *taskToString(struct ChecklistItem* task) {
//	char *temp = malloc(sizeof(char)*50);
//
//}

struct ListNode *readCSV() {

    FILE *fp = fopen("tasks.csv", "r");
    if (fp == NULL) {
        printf("Error opening tasks.csv.");
        return NULL;
    } 

    char *buf = malloc(sizeof(char) * LINE_LIMIT);
	fgets(buf, LINE_LIMIT, fp);
	
	if (strcmp(buf, CSV_HEADER)) {
		printf("tasks.csv has the wrong header. Please check formatting.\n");
		printf("Exiting readCSV()\n");
		return NULL;
	}

	struct ListNode *cur;
	struct ListNode *ret; // keep pointer to head ListNode to return;

	if (strcmp(fgets(buf, LINE_LIMIT, fp), "\n")) {
		cur = (struct ListNode *) malloc(sizeof(struct ListNode *));
		ret = cur;
		cur->item = stringToTask(buf);
	} else {
		printf("tasks.csv contains no tasks.\n");
		return NULL;
	}
	
	while (strcmp(fgets(buf, LINE_LIMIT, fp), "\n")) {
		cur->next = (struct ListNode *) malloc(sizeof(struct ListNode *));
		cur = cur->next;
		cur->item = stringToTask(buf);
	}

	free(buf);
	return ret;
}

void printItem(struct ChecklistItem* pItem) {
	printf("|%s\t|%d\t|%d\t|%d\t|%d\t|\n", pItem->name, pItem->deadline, pItem->eta, pItem->priority, pItem->custom);
}

void printList() {
	
	if (head->item == NULL) {
		printf("The List is Empty.\n");
		printf("head.item = %p\n", head->item);
		return;
	}
	
	printf("%s", CSV_HEADER);	
	
	struct ListNode *cur = head;
	printItem(cur->item);
	while (cur->next != NULL) {
		cur = cur->next;
		printItem(cur->item);
	}
}

int main(int argc, char **argv) {
    
	printf("Reading CSV.\n");
    head = readCSV();

	printf("Printing current list.\n");
    printList();




    return 0;

}


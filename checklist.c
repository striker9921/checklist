// main file of the checlist project.

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const char DEBUG = 0;
const char *CSV_HEADER = "Task Name,Deadline,ETA,Priority,Custom Weight\n";
const int LINE_LIMIT = 50;
const int TASK_LIMIT = 10;

struct ChecklistItem {
    char *name;
    int deadline;
    int eta;
    int priority;
    int custom;
};

ChecklistItem *list;

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

char *taskToString(struct ChecklistItem *task) {
	char *temp = malloc(sizeof(char) * LINE_LIMIT);
	snprintf(temp, LINE_LIMIT, "%s,%d,%d,%d,%d\n", task->name, task->deadline, task->eta, task->priority, task->custom);
	return temp;
}

void freeItem(struct ChecklistItem **task) {
	free((*task)->name);
	free(*task);
	task = NULL;
}

void readCSV(char *fpath) {

    FILE *fp = fopen(fpath, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening %s to read: %s\n", fpath, strerror(errno));
        return;
    }

    char *buf = malloc(sizeof(char) * LINE_LIMIT);
	fgets(buf, LINE_LIMIT, fp);
	
	if (strcmp(buf, CSV_HEADER)) {
		printf("%s has the wrong header. Please check formatting.\n", fpath);
		printf("Aborting read file.\n");
		free(buf);
		fclose(fp);
		return;
	}

	struct ListNode *cur;

	if (strcmp(fgets(buf, LINE_LIMIT, fp), "\n")) {
		cur = (struct ListNode *) malloc(sizeof(struct ListNode *));
		head = cur;
		cur->item = stringToTask(buf);
	} else {
		printf("%s contains no tasks.\n", fpath);
		free(buf);
		fclose(fp);
		return;
	}
	
	while (strcmp(fgets(buf, LINE_LIMIT, fp), "\n")) {
		cur->next = (struct ListNode *) malloc(sizeof(struct ListNode *));
		cur = cur->next;
		cur->item = stringToTask(buf);
	}

	free(buf);
	fclose(fp);
}

void writeCSV(char *fpath) {
	if (head == NULL) {
		printf("Loaded list is empty.\n");
		return;
	}

	FILE *fp = fopen(fpath, "w+");
	if (fp == NULL) {
		fprintf(stderr, "Error opening %s to write: %s\n", fpath, strerror(errno));
		return;
	}

	fprintf(fp, "%s", CSV_HEADER);

	char *buf;
	struct ListNode *cur = head;

	while(1) {
		buf = taskToString(cur->item);
		fprintf(fp, "%s", buf);
		free(buf);
		buf = NULL;
		if (cur->next == NULL) {
			break;
		}
		cur = cur->next;
	}
	fclose(fp);
}

void printItem(struct ChecklistItem* pItem) {
	printf("|%s\t|%d\t|%d\t|%d\t|%d\t|\n", pItem->name, pItem->deadline, pItem->eta, pItem->priority, pItem->custom);
}

void printList() {
	if (DEBUG) {printf("Printing list.\n");}

	if (head->item == NULL) {
		printf("The List is Empty.\n");
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

void terminate() {
	
	if (DEBUG) {printf("Terminating program (freeing heap mem.)\n");}
	
	if (head == NULL) {
		if (DEBUG) {printf("List is empty. Terminating.\n");}
		return;
	}
	
	struct ListNode *prev;
	struct ListNode *cur = head;
	
	while(cur->next != NULL) {
		prev = cur;
		cur = cur->next;
		freeItem(&(prev->item));
		free(prev);
		prev = NULL;
	}

	freeItem(&(cur->item));
	free(cur);
	cur = NULL;
	
}

int main(int argc, char **argv) {

	list = malloc(sizeof(ChecklistItem) * LIST_LIMIT);
    
	printf("Reading CSV.\n");
	readCSV("tasks.csv");

	printf("Printing current list.\n");
    printList();

	printf("Writing list to file.\n");
	writeCSV("tasksout.csv");

	terminate();

    return 0;

}


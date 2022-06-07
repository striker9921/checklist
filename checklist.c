// main file of the checlist project.

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char DEBUG = 0;
const char *CSV_HEADER = "Task Name,Deadline,ETA,Priority,Custom Weight\n";
const int LINE_LIMIT = 50;
const int TASKS_PER_PAGE = 10;

struct ChecklistItem {
    char *name;
    int deadline;
    int eta;
    int priority;
    int custom;
};

struct ChecklistItem **list;

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

	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		if (strcmp(fgets(buf, LINE_LIMIT, fp), "\n")) {
			list[i] = stringToTask(buf);
		} else {
			break;
		}
	}

	free(buf);
	fclose(fp);
}

void writeCSV(char *fpath) {
	if (list == NULL) {
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

	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		buf = taskToString(list[i]);
		fprintf(fp, "%s", buf);
		free(buf);
	}
}

void printItem(struct ChecklistItem* pItem) {
	printf("|%s\t|%d\t|%d\t|%d\t|%d\t|\n", pItem->name, pItem->deadline, pItem->eta, pItem->priority, pItem->custom);
}

void printList() {
	if (DEBUG) {printf("Printing list.\n");}

	if (list == NULL) {
		printf("The List is Empty.\n");
		return;
	}
	
	printf("%s", CSV_HEADER);	
	
	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		printf("%s\t|%d\t|%d\t|%d\t|%d\t|", list[i]->name, list[i]->deadline, list[i]->eta, list[i]->priority, list[i]->custom);
	}

}

void terminate() {
	
	if (DEBUG) {printf("Terminating program (freeing heap mem.)\n");}
	
	if (list == NULL) {
		if (DEBUG) {printf("List is empty. Terminating.\n");}
		return;
	}
	
	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		freeItem(&list[i]);
	}
}

void setupList() {
	list = malloc(sizeof(struct ChecklistItem) * TASKS_PER_PAGE);
	readCSV("tasksin.csv");
	writeCSV("tasksout.csv");

}

void input(char *buffer, int max) {
	fgets(buffer, max, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
}

int main(int argc, char **argv) {

	int running = 1;
	char *buffer = malloc(sizeof(char) * 50);
	while (running) {
		printf("Enter a command (print/read/write/quit): ");
		input(buffer, 49);
		if (!strcmp(buffer, "print")) {
			printList();
		} else if (!strcmp(buffer, "read")) {
			printf("Enter a file to read from: ");
			input(buffer, 49);
			readCSV(buffer);
		} else if (!strcmp(buffer, "write")) {
			printf("Enter a file to save to: ");
			input(buffer, 49);
			writeCSV(buffer);
		} else if (!strcmp(buffer, "quit")) {
			printf("Ending program.\n");
			running = 0;
		} else {
			printf("Invalid command.\n");
		}
	}

	free(buffer);
	terminate();
    return 0;
}


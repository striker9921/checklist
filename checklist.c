// main file of the checlist project.

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

const char *prefix = "[DEBUG]";
const char *CSV_HEADER = "Task Name,Deadline,ETA,Priority,Custom Weight\n";
const int LINE_LIMIT = 255;
const int TASKS_PER_PAGE = 10;

enum LOG_LEVEL {
	LOG,
	DEBUG,
	ERROR
};

const int LOGGING[3] = {0, 0, 0};
const char LOG_FORMAT[3][9] = {"[ LOG ] ", "[DEBUG] ", "[ERROR] "};


struct ChecklistItem {
    char *name;
    int deadline;
    int eta;
    int priority;
    int custom;
};

struct ChecklistItem **list;

void logp(enum LOG_LEVEL log, char *format, ...) {
	if (LOGGING[(int) log]) {
		printf(LOG_FORMAT[(int) log]);
		va_list args;
		va_start (args, format);
		vprintf (format, args);
		va_end (args);
	}
}

int str_eq(const char *str1, const char *str2) {
	return !strcmp(str1, str2);
}

void printItem(struct ChecklistItem* pItem) {
	printf("|%s\t|%d\t|%d\t|%d\t|%d\t|\n", pItem->name, pItem->deadline, pItem->eta, pItem->priority, pItem->custom);
}

struct ChecklistItem *stringToTask(char *str) {
	logp(LOG, "Parsing string: %s", str);

	struct ChecklistItem *temp = malloc(sizeof(struct ChecklistItem));
	char *ptr = str;

	temp->name = malloc(sizeof(char) * (LINE_LIMIT-22));
	strcpy(temp->name, strtok(str, ","));
	ptr += strlen(temp->name) + 1;
	temp->deadline = (int) strtol(ptr, &ptr, 10);
	ptr++;
	temp->eta = (int) strtol(ptr, &ptr, 10);
	ptr++;
	temp->priority = (int) strtol(ptr, &ptr, 10);
	ptr++;
	temp->custom = (int) strtol(ptr, &ptr, 10);
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
	logp(LOG, "Reading from file: %s\n", fpath);

    FILE *fp = fopen(fpath, "r");
    if (fp == NULL) {
        logp(ERROR, "Error opening %s to read: %s\n", fpath, strerror(errno));
        return;
    }

    char *buf = malloc(sizeof(char) * LINE_LIMIT);
	fgets(buf, LINE_LIMIT, fp);

	if (!str_eq(buf, CSV_HEADER)) {
		printf("%s has the wrong header. Please check formatting.\n", fpath);
		printf("Ending read file.\n");
		free(buf);
		fclose(fp);
		return;
	}
	
	int count = 0;
	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		fgets(buf, LINE_LIMIT, fp);
		if (feof(fp)) {
			break;
		}
		count++;
		list[i] = stringToTask(buf);
	}
	logp(LOG, "Read %d tasks into list.\n", count);

	free(buf);
	fclose(fp);
}

void writeCSV(char *fpath) {
	logp(LOG, "Writing tasks in list to file: %s\n", fpath);

	if (list == NULL) {
		printf("List is not loaded.\n");
		return;
	}

	if (list[0] == NULL) {
		printf("Loaded list is empty.\n");
		return;
	}

	FILE *fp = fopen(fpath, "w");
	if (fp == NULL) {
		logp(ERROR, "Error opening %s to write: %s", fpath, strerror(errno));
		return;
	}

	fprintf(fp, "%s", CSV_HEADER);

	char *buf;

	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		if (list[i] == NULL) {
			fclose(fp);
			break;
		}
		buf = taskToString(list[i]);
		fprintf(fp, "%s", buf);
		free(buf);
	}
	fclose(fp);
}

void printList() {

	if (list == NULL) {
		printf("The List is Empty.\n");
		return;
	}
	
	printf("%s", CSV_HEADER);	
	
	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		if (list[i] != NULL) {
			printf("|%02d.", i);
			printItem(list[i]);
		} else {break;}
	}

}

void terminate() {
	
	logp(LOG, "Terminating program (freeing heap mem.)\n");
	
	if (list == NULL) {
		logp(LOG ,"List is empty. Terminating.\n");
		return;
	}
	
	for (int i = 0; i < TASKS_PER_PAGE; i++) {
		freeItem(&list[i]);
	}
}

void input(char *buffer, int max) {
	fgets(buffer, max, stdin);
	buffer[strcspn(buffer, "\n")] = 0;
}



int main(int argc, char **argv) {

	int running = 1;
	char *buffer = malloc(sizeof(char) * LINE_LIMIT);
	list = calloc(TASKS_PER_PAGE, sizeof(struct ChecklistItem*));
	while (running) {
		printf("Enter a command (print/read/write/quit): ");
		input(buffer, LINE_LIMIT);
		if (str_eq(buffer, "print")) {
			printList();
		} else if (str_eq(buffer, "read")) {
			printf("Enter a file to read from: ");
			input(buffer, LINE_LIMIT);
			readCSV(buffer);
		} else if (str_eq(buffer, "write")) {
			printf("Enter a file to save to: ");
			input(buffer, LINE_LIMIT);
			writeCSV(buffer);
		} else if (str_eq(buffer, "quit")) {
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

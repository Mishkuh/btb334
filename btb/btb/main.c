#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BTB_SIZE 1024

typedef struct entry {
	char pc[7];
	char target_pc[7];
	int pred;
	int busy;
} Entry;

void find_next_address(char cur_address[7], char next_address[7]);
void print_btb(Entry btb[BTB_SIZE], int hit, int miss, int total, int right, int wrong);
int entry_exist(Entry btb[BTB_SIZE], char pc[7]);
int find_index(char pc[7]);
void add_to_btb(Entry btb[BTB_SIZE], char pc[7], char target_pc[7]);
int get_pc_prediction(Entry btb[BTB_SIZE], char pc[7], char predicted_pc[7]);
int is_taken_branch(char pc[7], char target_pc[7]);
int next_state(int cur_state, int taken);

int main(void)
{
	Entry btb[BTB_SIZE];
	char cur_address[7];
	char next_addr[7];
	char prev_addr[7];
	FILE *input = fopen("doduc.dat", "r");
	int taken;
	char predicted_pc[7];
	int total = 0, hit = 0, miss = 0, right = 0, wrong = 0;
	unsigned long index;

	for (int i = 0; i < BTB_SIZE; i++)
	{
		btb[i].busy = 0;
		btb[i].pred = 0;
		strcpy(btb[i].pc, "0");
		strcpy(btb[i].target_pc, "0");
	}

	fscanf(input, "%s", cur_address);

	while (!feof(input))
	{
		strcpy(prev_addr, cur_address);
		find_next_address(cur_address, next_addr);
		fscanf(input, "%s", cur_address);

		if (entry_exist(btb, prev_addr))
		{
			hit++;
			total++;

			get_pc_prediction(btb, prev_addr, predicted_pc);
			index = find_index(prev_addr);

			if (strcmp(cur_address, predicted_pc) == 0)
				right++;
			else
				wrong++;

			btb[index].pred = next_state(btb[index].pred, is_taken_branch(prev_addr, cur_address));
			
		}
		else if (is_taken_branch(prev_addr, cur_address))
		{
			miss++;
			total++;
			add_to_btb(btb, prev_addr, cur_address);
		}
	}

	print_btb(btb, hit, miss, total, right, wrong);

	fclose(input);
	return 0;
}

void find_next_address(char cur_address[7], char next_address[7])
{
	unsigned long next_addr_dec = 0;
	char *ptr;
	
	if (cur_address)
	{
		strcpy(next_address, cur_address);
		next_addr_dec = strtoul(cur_address, NULL, 16);
		next_addr_dec += 4;
		_ultoa(next_addr_dec, next_address, 16);
	}
}

int find_index(char pc[7])
{
	char temp[14];

	//char temp2[10]; //512
	char temp2[11]; // 1024
	unsigned long index;

	// 512
	// index = strtoul(pc + 3, NULL, 16);
	// _ultoa(index + 4096, temp, 2);
	// strncpy(temp2, temp + 2, 9);
	// temp2[9] = '\0';
	// index = strtoul(temp2, NULL, 2);

	index = strtoul(pc + 3, NULL, 16);
	_ultoa(index + 4096, temp, 2);
	strncpy(temp2, temp + 1, 10);
	temp2[10] = '\0';
	index = strtoul(temp2, NULL, 2);

	return index;
}

void add_to_btb(Entry btb[BTB_SIZE], char pc[7], char target_pc[7])
{
	unsigned long index = find_index(pc);

	strcpy(btb[index].pc, pc);
	strcpy(btb[index].target_pc, target_pc);
	btb[index].pred = 0; // initial state
	btb[index].busy = 1;
}

int get_pc_prediction(Entry btb[BTB_SIZE], char pc[7], char predicted_pc[7])
{
	unsigned long index = find_index(pc);

	if (btb[index].pred > 1)
		find_next_address(pc, predicted_pc);
	else
		strcpy(predicted_pc, btb[index].target_pc);

}

void print_btb(Entry btb[BTB_SIZE], int hit, int miss, int total, int right, int wrong)
{
	FILE *output = fopen("results_without_zeros.txt", "w");

	fprintf(output, "%10s%10s%10s%10s%10s\n", "ENTRY#", "PC", "TARGET", "PRED", "BUSY");

	for (int i = 0; i < BTB_SIZE; i++)
	{
		if(btb[i].busy == 1)
			fprintf(output,"%10d%10s%10s%10d%10d\n", i, btb[i].pc, btb[i].target_pc, btb[i].pred, btb[i].busy);
	}
	fprintf(output, "****************************************************************************\n");
	fprintf(output, "\nTotal Hits: %d\nTotal Number of Right Predictions: %d\nNumber of Misses: %d\n", total, right, miss);
	fprintf(output, "\nPercent Hit: %.2lf%% \nPercent Miss: %.2lf%% \nPercent Right: %.2lf%% \nPercent Wrong: %.2lf%%", 
		((double)hit / total) * 100, ((double)miss / total) * 100, ((double)right / hit) * 100, ((double)wrong / hit) * 100);

	fclose(output);
}

int entry_exist(Entry btb[BTB_SIZE], char pc[7])
{
	unsigned long index = find_index(pc);

	if (strcmp(btb[index].pc, pc) == 0)
		return 1;
	else
		return 0;
}

int is_taken_branch(char pc[7], char target_pc[7])
{
	char next_pc[7];
	find_next_address(pc, next_pc);
	if (strcmp(target_pc, next_pc) == 0)
		return 0;
	else
		return 1;
}

int next_state(int cur_state, int taken)
{
	int next_state;

	if (taken)
	{
		switch (cur_state)
		{
			case 0: next_state = 0; break;
			case 1: next_state = 0; break;
			case 2: next_state = 1; break;
			case 3: next_state = 2; break;
		}
	}
	else
	{
		switch (cur_state)
		{
			case 0: next_state = 1; break;
			case 1: next_state = 2; break;
			case 2: next_state = 3; break;
			case 3: next_state = 3; break;
		}
	}

	return next_state;
}
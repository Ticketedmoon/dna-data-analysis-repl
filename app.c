#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// Struct definitions
struct Row {
	char *rsid;
	int chromosome;
	long position;
	char allele_1;
	char allele_2;
};

// Consants
const char* PROMPT_FOR_FUNCTION_SELECT = 
	"If you need to see the function options again, enter `0`\n"
	"Please input a number from the ordered list to execute that function: ";

// Global variables
size_t TOTAL_LINES = 0;
size_t TOTAL_LINES_IN_SORTED_OUTPUT = 0;
size_t SLEEP_PERIOD = 100000; // microseconds

int SELECTED_CHROM_NUM_FOR_SORT = -1;

struct Row *chromosome_data;

// Function Declarations
void display_menu();

struct Row *load_raw_dna_data(struct Row *rows);

void display_raw_data(struct Row *rows);

void count_chromosome_records(struct Row *rows);

void display_chromosome_data(struct Row *rows);

void sort_chromosome_data(struct Row *rows);

void create_chromosome_file();

void search_rsid(struct Row *rows);

void compare_control(struct Row *rows);

// Functions
int main(void)
{
	struct Row *rows;

	printf("DNA Data Analysis Application Started.\n");
	display_menu();

	int selectedFunction = 1;
	while (selectedFunction != 9)
	{
		if (selectedFunction == 0)
		{
			display_menu();
		}
		else if (selectedFunction == 1)
		{
			rows = load_raw_dna_data(rows);
		}
		else if (selectedFunction == 2)
		{
			display_raw_data(rows);	
		}
		else if (selectedFunction == 3)
		{
			count_chromosome_records(rows);
		}
		else if (selectedFunction == 4)
		{
			display_chromosome_data(rows);
		}
		else if (selectedFunction == 5)
		{
			sort_chromosome_data(rows);
		}
		else if (selectedFunction == 6)
		{
			create_chromosome_file();
		}
		else if (selectedFunction == 7)
		{
			search_rsid(rows);
		}
		else if (selectedFunction == 8)
		{
			compare_control(rows);
		}

		// Re-prompt for input
		printf("\n%s", PROMPT_FOR_FUNCTION_SELECT);
		scanf("%d", &selectedFunction);
	}

	// When user exits the program, store the sorted chromosome data from function 5.
	create_chromosome_file();
	printf("> Exiting program...\n");
	return 0;
}

void display_menu()
{
	const char* menu = 
		"1. Load RAW DNA data\n"
	 	"2. Display RAW Data\n"
	 	"3. Count Chromosome Records\n"
		"4. Display Chromosome Data\n"
	 	"5. Sort Chromosome Data\n"
	 	"6. Create Chromosome Datafile.\n"
	 	"7. Search for DNA at Position\n"
	 	"8. Compare Control\n"
	 	"9. Exit System\n";
	printf("%s", menu);
}

struct Row *load_raw_dna_data(struct Row *rows)
{
	FILE *fPtr = NULL;
	if ((fPtr = fopen("./resources/data/dna_data.txt","r")) == NULL)
	{
		printf("Error loading raw dna data file from resources - exiting program.\n");
		exit(1);
	}

	size_t N_ROWS = 160;
	rows = malloc(N_ROWS * sizeof *rows);

	size_t initLength = 50;
	TOTAL_LINES = 0;

	// Reallocate only necessary space for rsid (Space Optimised)
	while (!feof(fPtr))
	{
		// Allocate enough space (50 bytes) initially.
		rows[TOTAL_LINES].rsid = (char*) malloc(initLength * sizeof(char*));

		/* Get actual length of rsid property.
		   strlen() counts characters so we can be certain it sizes it correctly. */
		size_t rsidLength = strlen(rows[TOTAL_LINES].rsid);

		// Resize memory allocation to be length of rsid property.
		rows[TOTAL_LINES].rsid = realloc(rows[TOTAL_LINES].rsid, rsidLength * sizeof(char) + 1);

		//rows[idx].rsid = tmp;
		char* rsid = malloc(rsidLength * sizeof(char));
		int chr;
		long pos;
		char a_1;
		char a_2;

		fscanf(fPtr, "%s %d %ld %c %c", rsid, &chr, &pos, &a_1, &a_2);

		// Store in arrow of Row structs (rows)
		// strcpy(rows[TOTAL_LINES].rsid, rsid);
		rows[TOTAL_LINES].rsid = rsid;
		rows[TOTAL_LINES].chromosome = chr;
		rows[TOTAL_LINES].position = pos;
		rows[TOTAL_LINES].allele_1 = a_1;
		rows[TOTAL_LINES].allele_2 = a_2;

		TOTAL_LINES++;
	}
	// Remove terminator line
	TOTAL_LINES--;

	printf("> DNA data file loaded - Found %lu total lines in dna data file\n", TOTAL_LINES);

	// End file read stream
	fPtr = NULL;
	return rows;
}

void display_raw_data(struct Row *rows)
{
	system("clear");
	printf("Displaying DNA data from local file: \n");

	// Reset file pointer to start of file
	int counter = 0;

	for (int i = 0; i < TOTAL_LINES-39; i++)
	{
		system("clear"); // "cls" only works on windows. If on linux use system("clear");
		printf("|%20s|%20s|%20s|%20s|%20s|%20s|\n", "Entry", "RSID", "CHROMOSOME", "POSITION", "ALLELE_1", "ALLELE_2");
		for (int j = 0; j < 40; j++)
		{
			int idx = i + j;
			printf("|%20d|%20s|%20d|%20ld|%20c|%20c|\n", idx+1, rows[idx].rsid, rows[idx].chromosome, rows[idx].position, 
				rows[idx].allele_1, rows[idx].allele_2);
		}
		// Sleep for a brief period since we only want to display 40 records "at a time".
		usleep(SLEEP_PERIOD);
	}
}

void count_chromosome_records(struct Row *rows)
{
	int chromNum;
	printf("[Count Chromosome Records] -- Enter a chromosome number to count: ");
	scanf("%d", &chromNum);

	int total = 0;
	for (int i = 0; i < TOTAL_LINES; i++)
	{
		if (rows[i].chromosome == chromNum)
		{
			total++;
		}
	}

	printf("> Chromosome Number: %d has %d occurrences\n", chromNum, total);
}

void display_chromosome_data(struct Row *rows)
{
	int chromNum;
	printf("[Display Chromosome Data] -- Enter a chromosome number to display entries for:");
	scanf("%d", &chromNum);

	printf("|%20s|%20s|%20s|%20s|%20s|%20s|\n", "ENTRY", "RSID", "CHROMOSOME", "POSITION", "ALLELE_1", "ALLELE_2");
	size_t rowIdx = 0;
	for (int i = 0; i < TOTAL_LINES; i++)
	{
		if (rows[i].chromosome == chromNum)
		{
			struct Row entry = rows[i];
			printf("|%20lu|%20s|%20d|%20ld|%20c|%20c|\n", rowIdx+1, entry.rsid, entry.chromosome, entry.position, entry.allele_1, entry.allele_2);
			rowIdx++;
		}
	}
	
}

void sort_chromosome_data(struct Row *rows)
{
	printf("[Sort Chromosome Data] -- Enter a chromosome number to sort by position [Descending]: ");
	scanf("%d", &SELECTED_CHROM_NUM_FOR_SORT);

	struct Row *local_chromosome_data = malloc(TOTAL_LINES * sizeof(struct Row));
	int chromosomeIdx = 0;

	for (int i = 0; i < TOTAL_LINES; i++)
	{
		if (rows[i].chromosome == SELECTED_CHROM_NUM_FOR_SORT)
		{
			local_chromosome_data[chromosomeIdx] = rows[i];
			chromosomeIdx++;
		}
	}

	// Realloc local_chromsome_data to be the count of entries with chromsome num.
	struct Row *temp = (struct Row*) realloc(local_chromosome_data, (chromosomeIdx * sizeof(struct Row)));
	if (temp == NULL)
	{
		printf("Cannot allocate more memory.\n");
		exit(1);
	}
	else
	{
		local_chromosome_data = temp;
	}

	// Sort copied struct array on position
	for (int i = 0; i < chromosomeIdx; i++)
	{
		for (int j = i+1; j < chromosomeIdx; j++)
		{
			if (local_chromosome_data[j].position > local_chromosome_data[i].position)
			{
				struct Row tmp = local_chromosome_data[i];
				local_chromosome_data[i] = local_chromosome_data[j];
				local_chromosome_data[j] = tmp;
			}
		}
	}

	printf("|%20s|%20s|%20s|%20s|%20s|%20s|\n", "ENTRY", "RSID", "CHROMOSOME", "POSITION", "ALLELE_1", "ALLELE_2");
	for (int i = 0; i < chromosomeIdx; i++)
	{
		struct Row entry = local_chromosome_data[i];
		printf("|%20d|%20s|%20d|%20ld|%20c|%20c|\n", i+1, entry.rsid, entry.chromosome, entry.position, entry.allele_1, entry.allele_2);
	}

	printf("> Entries for chromosome %d sorted on position descending.", SELECTED_CHROM_NUM_FOR_SORT);
	TOTAL_LINES_IN_SORTED_OUTPUT = chromosomeIdx;

	chromosome_data = local_chromosome_data;
}

void create_chromosome_file()
{
	if (SELECTED_CHROM_NUM_FOR_SORT == -1)
	{
		printf("No sorted chromsome data has been stored,\nplease use function [5] before executing this process.\n");
		return;
	}

	// Create directory "./out" if it does not exist.
	mkdir("out", 0755);
	char fileNameBuffer[36];
	snprintf(fileNameBuffer, sizeof(fileNameBuffer), "./out/chromosome%d_position_data.txt", SELECTED_CHROM_NUM_FOR_SORT);

	FILE *fPtr = NULL;
	if ((fPtr = fopen(fileNameBuffer,"w")) == NULL)
	{
		printf("Error opening chromosome file to write sorted chromosome data - exiting program.\n");
		exit(0);
	}

	for (int i = 0; i < TOTAL_LINES_IN_SORTED_OUTPUT; i++)
	{
		struct Row entry = chromosome_data[i];
		// Comma-Separated-Values (CSV)
		fprintf(fPtr, "%s, %d, %ld, %c, %c\n", entry.rsid, entry.chromosome, entry.position, entry.allele_1, entry.allele_2);
	}
	
	printf("> Sorted chromosome data stored in file at: './out/chromsome%d_position_data.txt'\n", SELECTED_CHROM_NUM_FOR_SORT);
	fclose(fPtr);
}

void search_rsid(struct Row *rows)
{
	char inputRsid[20];
	printf("[Search RSID] -- Enter a RSID value to display the alleles for: ");
	scanf("%s", inputRsid);

	printf("|%20s|%20s|\n", "Allele Value #1", "Allele Value #2");
	for (int i = 0; i < TOTAL_LINES; i++)
	{
		if (strcmp(rows[i].rsid, inputRsid) == 0)
		{
			printf("|%20c|%20c|\n", rows[i].allele_1, rows[i].allele_2);
			// We can break out of the loop here as we have found the only value that matches.
			break;
		}
	}
}

void compare_control(struct Row *rows)
{
	printf("[Compare Control] -- Enter 3 combinations of RSID values and alleles (e.g., \"rs123123 GG\"):\n");

	int chromosome_for_group = -1;
	int offset = 0;
	for (int i = 0; i < 3; i++)
	{
		char inputRsid[20];
		char allele_1;
		char allele_2;
		scanf("%s %c%c", inputRsid, &allele_1, &allele_2);

		char validAlleles[7]; // One extra byte for the string terminator.

		for (int i = 0; i < TOTAL_LINES; i++)
		{
			if (strcmp(rows[i].rsid, inputRsid) == 0 && rows[i].allele_1 == allele_1 && rows[i].allele_2 == allele_2)
			{
				if (chromosome_for_group == -1)
				{
					chromosome_for_group = rows[i].chromosome;
				}
				else if (rows[i].chromosome != chromosome_for_group)
				{
					printf("> Error: Found multiple chromosome values for selected RSID values");
					return;
				}

				validAlleles[offset] = allele_1;
				validAlleles[offset+1] = allele_2;
				offset += 2;
			}
		}

		// If we've found 3 rsid and allele value matches
		if (offset == 6)
		{
			printf("Congratulations! Found %s on chromosome %d\n", validAlleles, chromosome_for_group);
		}
	}

}

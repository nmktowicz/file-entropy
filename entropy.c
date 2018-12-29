#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#define HIGH_ENTROPY_THRESHOLD 7
#define LOW_ENTROPY_THRESHOLD 2
#define DOUBLE_PRECISION 3

/**
 * This function calculates the entropy of a given block of bytes.
 *
 * @param block_data vector with the data of the block.
 * @param block_size the size of the block in bytes.
 *
 * @return entropy of the block.
 */
static double block_entropy(const unsigned char* block_data, size_t block_size)
{

	double frequencies[256] = { 0 };
	double entropy = 0.0;
	double p = 0.0;
	
	// Gets the frequency of each byte in the block
	for (size_t i = 0; i < block_size; ++i) {
		frequencies[block_data[i]]++;
	}

	// Calculates the entropy of the block
	for (size_t i = 0; i < 256; ++i) {

		if (frequencies[i] > 0) { // Skips bytes with zero frequncy
			p = frequencies[i] / block_size;
			entropy -= p * (log(p) / log(2));
		}
	}

	return entropy;

}

int main(int argc, char* argv[])
{

	char filename[128] = { 0 };
	size_t block_size = 0;
	size_t total_blocks = 0;
	size_t last_block_size = 0;
	size_t file_size = 0;
	FILE* file = NULL;
	struct stat filestatus = { 0 };
	unsigned char* block_data = NULL;
	long h_entropy_blocks = 0;
	long l_entropy_blocks = 0;

	// Basic command-line interface
	if (argc < 2) {
		printf("usage: entropy [-b blocksize] <filename>\n");
		return 1;
	}

	for (int i = 1; i < argc; ++i) {

		if (strcmp(argv[i], "-b") == 0) {

			if (argc < 4 || i + 1 > argc) {
				printf("usage: entropy [-b blocksize] <filename>\n");
				return 1;
			}

			block_size = atoi(argv[++i]);

		}
		else {
			strncpy(filename, argv[i], sizeof(filename));
		}

	}

	// atoi can return an undefined block_size
	if (!(block_size > 0)) {
		block_size = 1024;
	}

	block_data = calloc(block_size, sizeof(unsigned char));
	if (block_data == NULL) {
		printf("failed to allocate memory\n");
		return 1;
	}

	stat(filename, &filestatus);

	// Since fread() returns 0 reading an incomplete element, and the result 
	// left in the buffer is undefined, we need to calculate the total size 
	// of the file in order to know the size of the last block
	file_size = filestatus.st_size;
	total_blocks = file_size / block_size;
	last_block_size = file_size % block_size;

	if (last_block_size != 0) {
		total_blocks++;
	}
	else {
		last_block_size = block_size;
	}

	file = fopen(filename, "rb");

	if (!file) {
		printf("failed to open file: %s\n", filename);
		return 1;
	}

	printf("entropy report for %s\n", filename);
	printf("block#\tentropy\n");

	for (size_t nblocks = 0; nblocks < total_blocks; nblocks++) {

		// Reads the file in blocks of n bytes
		if (nblocks == total_blocks - 1) {
			fread(block_data, last_block_size, 1, file);
		}
		else {
			fread(block_data, block_size, 1, file);
		}

		// Gets the entropy of the block 
		double e = block_entropy(block_data, block_size);

		if (e < LOW_ENTROPY_THRESHOLD) {
			l_entropy_blocks++;
		}
		else if (e > HIGH_ENTROPY_THRESHOLD) {
			h_entropy_blocks++;
		}

		printf("%ld\t%0.2f\n", nblocks, e);

		memset(block_data, 0x00, block_size * sizeof(unsigned char));

	}

	fclose(file);

	printf("Low entropy blocks: %ld\n", l_entropy_blocks);
	printf("High entropy blocks: %ld\n", h_entropy_blocks);

	return 0;

}
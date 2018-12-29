#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip> // std::setprecision
#include <string>
#include <vector>

constexpr auto HIGH_ENTROPY_THRESHOLD = 7;
constexpr auto LOW_ENTROPY_THRESHOLD = 2;
constexpr auto DOUBLE_PRECISION = 3;

using namespace std;

/**
 * This function calculates the entropy of a given block of bytes.
 *
 * @param block_data vector with the data of the block.
 * @param block_size the size of the block in bytes.
 *
 * @return entropy of the block.
 */
static double block_entropy(vector<unsigned char> block_data, size_t block_size)
{

	vector<double> frequencies(256, 0);

	// Gets the frequency of each byte in the block
	for (vector<unsigned char>::size_type i = 0; i < block_size; ++i) {
		frequencies[block_data[i]]++;
	}

	// Calculates the entropy of the block
	double entropy = 0.0;
	for (vector<double>::size_type i = 0; i < 256; ++i) {

		if (frequencies[i] > 0) { // Skip bytes with zero frequency
			double p = frequencies[i] / block_size;
			entropy -= p * (log(p)/log(2));
		}

	}

	return entropy;

}

int main(int argc, char* argv[])
{

	// Basic command-line interface
	if (argc < 2) {
		cerr << "usage: entropy [-b blocksize] <filename>" << endl;
		return 1;
	}

	size_t block_size = 0;
	string filename;
	for (int i = 1; i < argc; ++i) {
		
		string arg = argv[i];
		if (arg == "-b") {
			
			if (argc < 4 || i + 1 > argc) {
				cerr << "usage: entropy [-b blocksize] <filename>" << endl;
				return 1;
			}

			block_size = atoi(argv[++i]);

		}
		else {
			filename = argv[i];
		}

	}

	// atoi can return an undefined block_size
	if (!(block_size > 0)) {
		block_size = 1024;
	}

	vector<unsigned char> block_data(block_size, 0);

	ifstream file (filename, ios_base::in | ios_base::binary);

	if (!file) {
		cerr << "failed to open file: " << filename << endl;
		return 1;
	}
	
	long h_entropy_blocks = 0;
	long l_entropy_blocks = 0;
	long nblocks = 0;

	cout << "entropy report for " << filename << endl;
	cout << "block#\tentropy" << endl;

	while (1) {

                
		// Reads the file in blocks of n bytes
		file.read(reinterpret_cast<char*>(&block_data[0]), block_size);
                
		// Gets the entropy of the block 
		double e = block_entropy(block_data, block_size);

		if (e < LOW_ENTROPY_THRESHOLD) {
			l_entropy_blocks++;
		}
		else if (e > HIGH_ENTROPY_THRESHOLD) {
			h_entropy_blocks++;
		}

		cout << nblocks << "\t" << std::setprecision(DOUBLE_PRECISION) << e << endl;

		nblocks++;

		if (file.gcount() < block_size) {
			break;
		}
                
        fill(block_data.begin(), block_data.end(), 0);

	}

	file.close();

	cout << "Low entropy blocks: " << l_entropy_blocks << endl;
	cout << "High entropy blocks: " << h_entropy_blocks << endl;

	return 0;

}
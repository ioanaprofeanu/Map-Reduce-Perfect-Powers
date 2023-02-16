// Profeanu Ioana, 333CA
// source file containing the implementation of the processing functions
#include "perfect_powers.h"

vector<int> find_powers_binary_search(int number, int number_of_reducers)
{
	vector<int> exponents;

	// if the number is 0 or lower, the number is not a perfect power
	if (number <= 0) {
		return exponents;
	}
	
	// for each possible exponent, try to find a base for which the number is
	// a perfect power
	for (int i = 2; i < number_of_reducers + 2; i++) {
		// if the number is 1, each power represents a perfect one
		if (number == 1) {
			exponents.push_back(i);
			continue;
		}
		// if the number is lower than the case when the base is 2,
		// it means that the other bases are invalid as well
		if (number < pow(2, i)) {
			continue;
		// if the number is a perfect power when the base is 2
		} else if (number == pow(2, i)) {
			exponents.push_back(i);
			continue;
		}

		// start the binary search from the next possible base to the last one
		int left = 3;
		int right = sqrt(number);

		while (left <= right) {
			int middle = (left + right) / 2;

			// check if the middle base is a valid one;
			// otherwise, continue the search
			if (pow(middle, i) == number) {
				exponents.push_back(i);
			}
			if (pow(middle, i) < number) {
				left = middle + 1;
			} else {
				right = middle - 1;
			}
		}
	}

	// return the list of valid exponents
	return exponents;
}

queue<string> read_files_to_process(string input_file)
{
	// create a queue of file names
	queue<string> files_to_process;
	int number_of_files = 0;

	// open file and read the file names from it
	ifstream fin(input_file);
	fin >> number_of_files;
	for (int i = 0; i < number_of_files; i++) {
		string file_name;
		fin >> file_name;
		files_to_process.push(file_name);
	}

	fin.close();
	return files_to_process;
}

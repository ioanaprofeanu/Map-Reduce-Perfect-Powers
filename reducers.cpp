// Profeanu Ioana, 333CA
// source file containing the implementation of the reducers
#include "perfect_powers.h"

void *reducers_function(void *arg)
{
	struct info_for_reducers *data = (struct info_for_reducers *)arg;
	// wait for the other threads to reach the barrier
	// this barrier is used to await the finish of the mappers threads
	pthread_barrier_wait(data->barrier_common);

	// create an unordered set of the perfect powers
	unordered_set<int> numbers_same_power;
	// for each data set parsed by the mappers
	for (const auto &entry : *data->map_of_lists) {
		// get the numbers at the power equal to the value of the
		// current reducer thread and add them to the set
		for (auto it = entry.second[data->id + 2].begin();
			it != entry.second[data->id + 2].end(); ++it) {
			numbers_same_power.insert(*it);
		}
	}
	// open the output file
	stringstream output_file;
	output_file << "out" << data->id + 2 << ".txt";
	ofstream fout(output_file.str());
	// the size of the unordered set is the total number
	// of distinct perfect powers
	fout << numbers_same_power.size();
	fout.close();

	// wait for the other reducer threads to finish
	pthread_barrier_wait(data->barrier_reducers);

	pthread_exit(NULL);
}

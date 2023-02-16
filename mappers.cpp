// Profeanu Ioana, 333CA
// source file containing the implementation of the mappers
#include "perfect_powers.h"

bool mappers_try_read_file(struct info_for_mappers *data, string *file_name)
{
	// variable for checking if the current thread can read from a file
	bool can_read = false;
	// lock mutex because only one thread at a time can get
	// data from the queue of file names
	pthread_mutex_lock(data->mutex_mappers_get_file);
	// if there is still data in the queue
	if ((*data->to_process_files_queue).empty() == false) {
		// get the file name at the front of the queue and pop it
		*file_name = (*data->to_process_files_queue).front();
		(*data->to_process_files_queue).pop();
		can_read = true;
	}
	// unlock mutex
	pthread_mutex_unlock(data->mutex_mappers_get_file);
	return can_read;
}

void *mappers_function(void *arg)
{
	struct info_for_mappers *data = (struct info_for_mappers *)arg;
	// initialize the vector of unordered sets for the current thread
	vector<unordered_set<int>> list_of_perfect_powers;
	for (int i = 0; i < data->number_of_reducers + 2; i++) {
		unordered_set<int> new_set;
		list_of_perfect_powers.push_back(new_set);
	}

	string file_name;
	// while there are still files that have not been processed yet
	while (mappers_try_read_file(data, &file_name)) {
		// read data from file
		int numbers_in_file;
		ifstream fin(file_name);
		fin >> numbers_in_file;
		// for each number in the file
		for (int i = 0; i < numbers_in_file; i++) {
			int current_number;
			fin >> current_number;
			// get the exponents for when the number is a perfect power
			vector<int> exponents = find_powers_binary_search(current_number,
									data->number_of_reducers);
			// for each exponent, add the current number in the set that
			// corresponds to that exponent (the list at the index equal
			// to the exponent value)
			for (long unsigned int i = 0; i < exponents.size(); i++) {
				list_of_perfect_powers.at(exponents[i]).insert(current_number);
			}
		}
		
		fin.close();
	}
	// lock and then unlock mutex for writing in the common map
	// that has all the lists for all the threads
	pthread_mutex_lock(data->mutex_mappers_write_map);
	(*data->map_of_lists).insert(std::make_pair(data->id, 
						list_of_perfect_powers));
	pthread_mutex_unlock(data->mutex_mappers_write_map);
	// barrier used to wait for all the mappers to finish; used to also
	// announce the reducers that the mappers have finished processing
	pthread_barrier_wait(data->barrier_common);

	pthread_exit(NULL);
}

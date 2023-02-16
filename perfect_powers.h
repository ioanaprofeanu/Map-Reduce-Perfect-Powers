// Profeanu Ioana, 333CA
// header file
#ifndef PERFECT_POWERS_H
#define PERFECT_POWERS_H

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <math.h>
#include <pthread.h>
#include <sstream>

using namespace std;

// structure to be given as argument when creating the mapperes threads
struct info_for_mappers
{
	int id;
	int number_of_reducers;
	pthread_mutex_t *mutex_mappers_get_file;
	pthread_mutex_t *mutex_mappers_write_map;
	pthread_barrier_t *barrier_common;
	queue<string> *to_process_files_queue;
	unordered_map<int, vector<unordered_set<int>>> *map_of_lists;
};

// structure to be given as argument when creating the reducers threads
struct info_for_reducers
{
	int id;
	int number_of_mappers;
	pthread_barrier_t *barrier_common;
	pthread_barrier_t *barrier_reducers;
	unordered_map<int, vector<unordered_set<int>>> *map_of_lists;
};

// function for finding if a given number is a perfect power
// returns a vector of exponents of those perfect powers
vector<int> find_powers_binary_search(int number, int number_of_reducers);

// function used for reading data by mappers from the current to be read file
bool mappers_try_read_file(struct info_for_mappers *data, string *file_name);

// function which contains the processing done by mappers
void *mappers_function(void *arg);

// function swhich contains the processing done by reducers
void *reducers_function(void *arg);

// function for reading the files from which the perfect powers will be read
queue<string> read_files_to_process(string input_file);

#endif

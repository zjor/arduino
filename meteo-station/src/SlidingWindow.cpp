#include "SlidingWindow.h"

SlidingWindow::SlidingWindow(int size) {
	this->size = size;
	count = 0;
	first_ix = last_ix = 0;
	sum = 0.0;
}

double SlidingWindow::average() {
	return (count == 0) ? 0.0 : ((double) sum) / count;
}

double SlidingWindow::add(int value) {	
	buffer[last_ix++] = value;
	sum += value;

	if (count == size) {
		sum -= buffer[first_ix++];
	} else {
		count++;
	}

	if (last_ix == MAX_WINDOW_SIZE) {
		last_ix = 0;
	}

	if (first_ix == MAX_WINDOW_SIZE) {
		first_ix = 0;
	}

	return average();
}
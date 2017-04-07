#define MAX_WINDOW_SIZE 10

class SlidingWindow {
	int size;
	int count;
	int first_ix, last_ix;
	int buffer[MAX_WINDOW_SIZE];
	int sum;

	public:
		SlidingWindow(int);
		double average();
		double add(int);
};
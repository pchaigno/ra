#include "Database.h"

using namespace std;

/**
 * Constructor by recopy
 * @param tr A transaction.
 */
Transaction::Transaction(const Transaction &tr) {
	length = tr.length;
	t = new int[tr.length];
	for(int i=0; i<length; i++) {
		t[i] = tr.t[i];
	}
}

/**
 * Constructor
 * @param filename The filename.
 */
Database::Database(char *filename) {
	current = 0;
	in = fopen(filename, "rt");
}

/**
 * Destructor
 */
Database::~Database() {
	if(in) {
		fclose(in);
	}
}

/**
 * Builds the next transaction from the file.
 * Calls the right method to do so, depending on the type of data.
 * @return The next transaction.
 */
Transaction* Database::getNext() {
	Transaction* t = 0;

	t = getNextAsFlat();

	if(t) {
		current++;
	} else {
		rewind(in);
		current = 0;
	}

	return t;
}

/**
 * Builds the next transaction from the file.
 * This is the method to use for numbers type of data.
 * @return The next transaction.
 */
Transaction* Database::getNextAsFlat() {
	vector<int> list;
	char c;

	// read list of items
	do {
		int item = 0, pos = 0;
		c = getc(in);
		while(c>='0' && c<='9') {
			item *= 10;
			item += int(c) - int('0');
			c = getc(in);
			pos++;
		}
		if(pos) {
			list.push_back(item);
		}
	} while(c!='\n' && !feof(in));

	if (feof(in)) {
		return 0;
	}
	// Note, also last transaction must end with newline,
	// else, it will be ignored

	// sort list of items
	// sort(list.begin(),list.end());

	// put items in Transaction structure
	Transaction* t = new Transaction(list.size());
	for(int i=0; i<int(list.size()); i++) {
		t->t[i] = list[i];
	}

	return t;
}

/**
 * 
 * @return
 */
Transaction* Database::getNextAs() {
	Transaction* t;
	int tid, item, i;
	vector<int> list;
	static int cur = 0, prev = -1;
	static bool begin = true;

	if(feof(in)) {
		begin = true;
		prev = -1;
		return 0;
	}

	if(!begin) {
		list.push_back(cur);
	} else {
		begin = false;
	}

	while (true) {
		int dummy;
		fscanf(in, "%d %d %d", &dummy, &tid, &item);
		//fscanf(in, "%d %d", &tid, &item);
		if(feof(in)) {
			int size = list.size();
			t = new Transaction(size);
			for(i=0; i<size; i++) {
				t->t[i] = list[i];
			}
			list.clear();

			return t;
		} else if(prev < 0) {
			prev = tid;
		} else if(tid != prev) {
			prev = tid;
			cur = item;
			int size = list.size();
			t = new Transaction(size);
			for(i=0; i<size; i++) {
				t->t[i] = list[i];
			}
			list.clear();

			return t;
		}

		list.push_back(item);
	}
}

/**
 * 
 * @return
 */
Transaction* Database::getNextAsQuest() {
	int tmptid, tid, l, i;
	Transaction* t;

	fscanf(in, "%d %d %d", &tmptid, &tid, &l);
	if(feof(in)) {
		return 0;
	}

	t = new Transaction(l);
	for(i=0; i<l; i++) {
		fscanf(in, "%d", &t->t[i]);
	}
	return t;
}

/**
 * 
 * @return
 */
Transaction* Database::getNextBin() {
	int tmptid, tid, l, i;
	Transaction* t;

	fread(&tmptid, 4, 1, in);
	if(feof(in)) {
		return 0;
	}

	fread(&tid, 4, 1, in);
	fread(&l, 4, 1, in);
	t = new Transaction(l);
	for(i=0; i<l; i++) {
		fread(&t->t[i], 4, 1, in);
	}

	return t;
}
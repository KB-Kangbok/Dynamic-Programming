#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>

#define INFINITY 1000000;

class TextLayout {
public:
	TextLayout(const std::string& filename, const int max);
	~TextLayout();
	int recursive_compute(const int max, const int sNum, const int fNum);
	void dynamic_compute(const int max);
	void dynamic_print();
private:
	std::vector<std::string> words;
	int** memoize;
	int** substructure;
};

TextLayout::TextLayout(const std::string& filename, const int max) {
	std::ifstream in;
	in.open(filename);
	std::string currWord;

	//read individual word and store it in vector
	while (in.good()) {
		in >> currWord;
		words.push_back(currWord);
	}

	//initialize 2-dimensional dynamic arrays
	memoize = new int*[words.size()];
	substructure = new int*[words.size()];
	for (int i = 0; i < words.size(); i++) {
		memoize[i] = new int[words.size()];
		substructure[i] = new int[words.size()];
	}

	//set all memoize matrix elements to default value: INFINITY
	for (int i = 0; i < words.size(); i++) {
		for (int j = 0; j < words.size(); j++) {
			memoize[i][j] = INFINITY;
		}
	}
	
	//run dynamic programming version of printing
	dynamic_compute(max);
	dynamic_print();

	//this is memoized recursive programming version that I first used to
	//figure out the dynamic programming version
	//std::cout << recursive_compute(max, 0, words.size()-1);
}

//destructor
TextLayout::~TextLayout() {
	words.clear();
	for (int i = 0; i < words.size(); i++) {
		delete[] memoize[i];
		delete[] substructure[i];
	}
	delete[] memoize;
	delete[] substructure;
}

//dynamic programming using the same logic as memoized recursive programming
void TextLayout::dynamic_compute(const int max) {
	int n = words.size();
	int maxComp;
	for (int i = 0; i < n; i++) {
		if (i == n - 1) {
			substructure[i][i] = i;
			memoize[i][i] = 0;
		}
		else {
			substructure[i][i] = i;
			memoize[i][i] = std::pow(max - words[i].size(), 3);
		}
	}
	
	for (int l = 1; l < n; l++) {
		for (int i = 0; i < n - l; i++) {
			int j = i + l;
			memoize[i][j] = INFINITY;
			maxComp = 0;
			for (int k = i; k <= j; k++) {
				maxComp += words[k].size();
			}
			maxComp += j - i;
			if (maxComp <= max) {
				if (j == n - 1) {
					substructure[i][j] = j;
					memoize[i][j] = 0;
				}
				else {
					substructure[i][j] = j;
					memoize[i][j] = std::pow(max - maxComp, 3);
				}
			}
			else {
				for (int k = i; k < j; k++) {
					maxComp = memoize[i][k] + memoize[k + 1][j];
					if (maxComp < memoize[i][j]) {
						substructure[i][j] = k;
						memoize[i][j] = maxComp;
					}
				}
			}
		}
	}
}

//print using the substructure made in dynamic programming
void TextLayout::dynamic_print() {
	int s = 0;
	int f = words.size() - 1;
	while (s <= f) {
		int curr = substructure[s][f];
		while (s <= curr) {
			std::cout << words[s] << " ";
			s++;
		}
		std::cout << std::endl;
	}
}

//memoized recursive programming
int TextLayout::recursive_compute(const int max, const int sNum, const int fNum) {
	int min = INFINITY;
	
	//memoized part
	if (memoize[sNum][fNum] < min) {
		return memoize[sNum][fNum];
	}

	//for one word case, just compute max - word and cube it.
	if (sNum == fNum) {
		if (fNum != words.size() - 1) {
			min = std::pow(max - words[sNum].size(), 3);
			memoize[sNum][fNum] = min;
			return min;
		}
		//if the word is the last word, ignore the value.
		else {
			return 0;
		}
	}
	int curr1, curr2, comp, maxComp;
	//compute the length of sentence that contains all the words from sNum to fNum index.
	maxComp = 0;
	for (int i = sNum; i <= fNum; i++) {
		maxComp += words[i].size();
	}
	maxComp += fNum - sNum;

	//if maxComp <= max, then that could be a sentence which is obviously the minimum case
	if (maxComp <= max) {
		if (fNum == words.size() - 1) {
			memoize[sNum][fNum] = 0;
			return 0;
		}
		//if final word is actually last word of the text, we have to ignore it.
		else {
			min = std::pow(max - maxComp, 3);
			memoize[sNum][fNum] = min;
			return min;
		}
	}
	//this means the sentence from sNum word to fNum word exceeds the max so we have to divide this sentence
	else {
		//find the minimum division point, ith word, by recursion.
		for (int i = sNum; i < fNum - 1; i++) {
			curr1 = recursive_compute(max, sNum, i);
			curr2 = recursive_compute(max, i + 1, fNum);
			comp = curr1 + curr2;
			if (comp < min) {
				min = comp;
			}
		}
		memoize[sNum][fNum] = min;
		return min;
	}
}
#ifndef BUCKET_SORT_H
#define BUCKET_SORT_H
#include <vector>
#include <iostream>
struct BucketSort {
   // vector of numbers
   std::vector<unsigned int> numbersToSort;
   void sort(unsigned int numCores);
};

std::vector<std::vector<unsigned int>> radixSortBuckets(std::vector<unsigned int>::iterator start, std::vector<unsigned int>::iterator end, int pos);
std::vector<unsigned int> radixSortConcat(std::vector<std::vector<unsigned int>> buckets);
int getNumDigits(unsigned int x);
int getDigit(unsigned int number, int digit);

#endif /* BUCKET_SORT_H */
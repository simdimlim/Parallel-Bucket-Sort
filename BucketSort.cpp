#include "BucketSort.h"
#include <algorithm>
#include <cmath>
#include <thread>
#include <iterator>
#include <climits>

// returns the digit at a specific position e.g. getDigit(98765, 2) = 8
int getDigit(unsigned int number, int digit) {
   int numDigits = getNumDigits(number);
   digit = numDigits - digit;
   
   if (digit > 0 && digit <= log10(number)+1) {
      return (unsigned int) (number / pow((double) 10, (double)(digit-1))) % 10;
   } else {
      return -1;
   }
}

// returns the number of digits for a given integer
int getNumDigits(unsigned int x) {
    if (x >= 10000) {
        if (x >= 10000000) {
            if (x >= 100000000) {
                if (x >= 1000000000)
                    return 10;
                return 9;
            }
            return 8;
        }
        if (x >= 100000) {
            if (x >= 1000000)
                return 7;
            return 6;
        }
        return 5;
    }
    if (x >= 100) {
        if (x >= 1000)
            return 4;
        return 3;
    }
    if (x >= 10)
        return 2;
    return 1;
}

void BucketSort::sort(unsigned int numCores) {
   int n = numbersToSort.size();
   int m = numCores;
   int partitionStart = 0;
   int increment = ceil((double) n/m);
      
   // sorted results:
   //    threads
   //       |
   //       |_ buckets
   //             |
   //             |_ numbers
   
   // store all the sorted data from each thread
   std::vector<std::vector<std::vector<unsigned int>>> sortedResults(numCores);
   
   // after each thread has performed radix sort on their set of numbers,
   // their buckets will be inserted into the big buckets
   std::vector<std::vector<unsigned int>> bigBuckets(11);
   
   std::vector<std::thread> threads;
   
   auto radixSort = [&sortedResults] (std::vector<unsigned int>::iterator start, 
   std::vector<unsigned int>::iterator end, int i)
   {   
      // perform radix sort on the thread's given partition
      sortedResults[i] = radixSortBuckets(start, end, 0);
   };
   
   // create a thread for every number of cores
   for (unsigned int i = 0; i < numCores; ++i) {
      // partition numbers evenly between the threads
      auto start = numbersToSort.begin()+partitionStart;
      auto end = numbersToSort.begin()+partitionStart+increment;
      
      // checks if the beginning of a partition + increment goes past the 
      // end of the numbersToSort vector
      if (end > numbersToSort.end()) {
         end = numbersToSort.end();
      }
      
      threads.push_back(std::thread(radixSort, start, end, i));
      
      // increment the start of the partition for next thread
      partitionStart += increment;
      
      // if we've reached the end of the vector, no need to create more threads
      if (numbersToSort.begin()+partitionStart > numbersToSort.end()) {
         break;
      }
   }
   
   // wait for all threads to finish
   for (auto &t : threads) {
      t.join();
   }
   
   // after all threads have finished their radix sort, move elements from small 
   // buckets to large buckets
   for (auto it = sortedResults.begin(); it != sortedResults.end(); ++it) {
      auto currThreadResults = *it;
      for (unsigned int i = 0; i < currThreadResults.size(); ++i) {
         auto currBucket = currThreadResults[i];
         bigBuckets[i].insert(bigBuckets[i].end(), currBucket.begin(), currBucket.end());
      }
   }
   
   // go through each big bucket and recursively bucket sort on each consecutive digit
   for (auto it = bigBuckets.begin(); it != bigBuckets.begin()+10; ++it) {
      auto currBucket = *it;
      if (currBucket.size() > 1) {
         auto temp = radixSortBuckets(currBucket.begin(), currBucket.end(), 0);
         *it = radixSortConcat(temp);
      }
   }
   
   numbersToSort = radixSortConcat(bigBuckets);
}

// performs radix sort on a specific partition of numbers
std::vector<std::vector<unsigned int>> radixSortBuckets(
                  std::vector<unsigned int>::iterator start, 
                  std::vector<unsigned int>::iterator end, int pos) {
                     
   // buckets for 0-9 and +1 for numbers that need to be padded
   std::vector<std::vector<unsigned int>> buckets(11); 
   std::vector<unsigned int> output;
   
   for (auto it = start; it != end; ++it) {
      int digit = getDigit(*it, pos);
      if (digit < 0) {
         // we put numbers that need to be padded into bucket[10] e.g.
         // getDigit() returns -1
         digit = 10;
      }
      // std::cout << "inserting " << *it << " into bucket " << digit << std::endl;
      buckets[digit].push_back(*it);
   }
   
   for (auto it = buckets.begin(); it != buckets.begin()+10; ++it) {
      // go through each bucket and recursively bucket sort on each consecutive digit
      auto currBucket = *it;
      if (currBucket.size() > 1) { // only recursively sort if there is more than 1 element
         auto temp = radixSortBuckets(currBucket.begin(), currBucket.end(), pos+1);
         *it = radixSortConcat(temp);
      }
   }
   
   return buckets;
}

// takes in buckets and concatenates them into one vector
std::vector<unsigned int> radixSortConcat(std::vector<std::vector<unsigned int>> buckets) {   
   std::vector<unsigned int> output;
   
   // insert all padded numbers first (from bucket[10])
   auto lastBucket = buckets[10];
   output.insert(output.end(), lastBucket.begin(), lastBucket.end());
   
   for (auto it = buckets.begin(); it != buckets.begin()+10; ++it) {
      // go through each bucket and concatenate numbers and store in output vector
      auto currBucket = (*it);
      output.insert(output.end(), currBucket.begin(), currBucket.end());
   }
   
   return output;
}


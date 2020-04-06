#include <cstdint>
#include <vector>
#include <iostream>
using namespace std;

uint64_t area(vector<pair<int, int>> &points) {
  // This code has signed overflows. :)
  // Please fix this so it correctly evaluates area..!
  int64_t total = 0;
  uint64_t area = 0;
  size_t n = points.size();
  int remain = 0;

  for (unsigned i = 0; i < n; i++) {
    unsigned j = (i + 1) % n;
    int64_t x_i = points[i].first;
    int64_t y_i = points[i].second;
    int64_t x_j = points[j].first;
    int64_t y_j = points[j].second;

    area = (uint64_t)(x_i * (y_j - y_i) - y_i * (x_j - x_i));
    total += area/2;

    // truncation occurs by rounding off
    if( (area%2 == 1) && (remain == 1) ) {
      total++;
      remain = 0;
    }
    else if(area%2 == 1)
      remain = 1;
  }

  return total;
}

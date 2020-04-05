#include<stdio.h>

int main() {
  printf("Type two (small, large) positives to calculate average: ");
  int a, b;
  scanf("%d %d", &a, &b);

  int average = a + (b - a) / 2;
  printf("Average: %d\n", average);

  return 0;
}

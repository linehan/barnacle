#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void merge(uint32_t *left, int l_len, uint32_t *right, int r_len, uint32_t *out)
{
	uint32_t i, j, k;
	for (i = j = k = 0; i < l_len && j < r_len; )
		out[k++] = left[i] < right[j] ? left[i++] : right[j++];
 
	while (i < l_len) out[k++] = left[i++];
	while (j < r_len) out[k++] = right[j++];
}
 
/* inner recursion of merge sort */
void recur(uint32_t *buf, uint32_t *tmp, int len)
{
	int l = len/2;
	if (len <= 1) return;
 
	/* note that buf and tmp are swapped */
	recur(tmp, buf, l);
	recur(tmp + l, buf + l, len - l);
 
	merge(tmp, l, tmp + l, len - l, buf);
}
 
/* preparation work before recursion */
void merge_sort(uint32_t *buf, int len)
{
	/* call alloc, copy and free only once */
	uint32_t *tmp = malloc(sizeof(uint32_t) * len);
	memcpy(tmp, buf, sizeof(uint32_t) * len);
 
	recur(buf, tmp, len);
 
	free(tmp);
}

int merge_test(void)
{
        #define LEN 20
	uint32_t i, x[LEN];
 
	for (i = 0; i < LEN; i++)
		x[i] = rand() % LEN;
 
	puts("before sort:");
	for (i = 0; i < LEN; i++) printf("%d ", x[i]);
	putchar('\n');
 
	merge_sort(x, LEN);
 
	puts("after sort:");
	for (i = 0; i < LEN; i++) printf("%d ", x[i]);
	putchar('\n');
 
	return 0;
}

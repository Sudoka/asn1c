#include <UTF8String.c>
#include <OCTET_STRING.c>
#include <ber_decoder.c>
#include <ber_tlv_length.c>
#include <ber_tlv_tag.c>
#include <der_encoder.c>
#include <constraints.c>
#include <sys/time.h>

static void
check(int expect_length, char *buf, int buflen) {
	UTF8String_t st;
	int ret;

	if(buflen < 0) buflen = strlen(buf);

	st.buf = (uint8_t *)buf;
	st.size = buflen;
	printf("[");

	for(ret = 0; ret < buflen; ret++)
		printf("%c", buf[ret]);
	ret = UTF8String_length(&st, 0, 0, 0);
	printf("]: size=%d, expect=%d, got=%d\n",
		buflen, expect_length, ret);
	assert(ret == expect_length);
}

static int
check_speed() {
	int cycles = 1000000;
	double start, stop;
	struct timeval tv;
	UTF8String_t st;
	char long_test[] =
		"a\303\237a\303\237a\303\237a\303\237"
		"a\303\237a\303\237a\303\237a\303\237"
		"a\303\237a\303\237a\303\237a\303\237"
		"a\303\237a\303\237a\303\237a\303\237"
		"a\303\237a\303\237a\303\237a\303\237";
	int ret;
	int i;

	st.buf = long_test;
	st.size = sizeof(long_test) - 1;

	ret = UTF8String_length(&st, 0, 0, 0);
	assert(ret == 40);
	printf("Now wait a bit...\n");

	gettimeofday(&tv, 0);
	start = tv.tv_sec + tv.tv_usec / 1000000.0;
	for(i = 0; i < cycles; i++) {
		ret += UTF8String_length(&st, 0, 0, 0);
	}
	gettimeofday(&tv, 0);
	stop = tv.tv_sec + tv.tv_usec / 1000000.0;

	printf("%d cycles in %.3fms\n", cycles, stop - start);

	return ret;
}

int
main() {

	check(0, "", 0);
	check(1, "\0", 1);
	check(-1, "\377", 1);
	check(1, "a", 1);
	check(2, "ab", 2);
	check(3, "abc", 3);
	assert(sizeof("a\303\237cd") == 6);
	check(4, "a\303\237cd", 5);
	check(-1, "a\303", 2);
	check(-1, "a\370\200\200\200c", 5);
	check(3, "a\370\201\200\201\257c", 7);
	/* not yet check(-1, "a\370\200\200\200\257c", 7); */

	check_speed();

	return 0;
}

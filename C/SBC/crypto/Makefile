SOURCE = aes.c CCM.c counter_block.c format.c global.c hash.c ISAAC.c mac.c MAIN.c modular_arithmetic.c rand.c sha3.c STS_protocol.c util.c

CC=gcc

default: release

debugl2: $(SOURCE)
	$(CC) -o $@ $^ -D DEBUGL2=1 -g

debugl1: $(SOURCE)
	$(CC) -o $@ $^ -D DEBUGL1=1 -g

production: $(SOURCE)
	$(CC) -o $@ $^ -D PRODUCTION_TEST=1 -O3

release: $(SOURCE)
	$(CC) -o $@ $^ -O3	
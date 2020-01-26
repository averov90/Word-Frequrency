#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

unsigned short ByteHash(const unsigned char *start, const unsigned char *end) {
	unsigned char offset = 0;
	unsigned short hash = 0;
	for (; start != end; ++start) {
		hash += offset ? *start >> offset | (unsigned char)(*start << 8 - offset) : *start;
		if (offset == 7) offset = 0;
		else ++offset;
	}
	return hash & 0x3FF;
}

#define UD_BLOCK_THRESHOLD 768

static struct block {
	char *str;
	unsigned short mcount;
	struct block *greater, *smaller;
};

static struct ud_block {
	struct block *block;
	unsigned short elem_count;
} *ud_blocks, *ud_blocks_e;
unsigned short ud_blocks_count;

static struct sblock {
	char *str;
	struct block *next;
} top100_max = { 0 }, top100_min = { 0 };

void PlaceString(unsigned char *start, unsigned char *end) {
	unsigned short hash = ByteHash(start, end);
	unsigned char presult = 2;

	struct ud_block *ud_blocks_cp, *ud_blocks_tr = 0;
	struct block *block_cp, *bpl_block = 0, *block_e = 0, *block_temp = 0;
	for (ud_blocks_cp = ud_blocks; ud_blocks_cp != ud_blocks_e; ++ud_blocks_cp) {
		block_cp = ud_blocks_cp->block + hash;
		if (block_cp->str && !strcmp(block_cp->str, start)) {
			bpl_block = block_cp;
			presult = 0;
			break;
		}
	}

	if (presult) {
		unsigned char nhas_underthreshold = 1;
		unsigned short blockoff_c, blockoff_bst = USHRT_MAX;
		for (ud_blocks_cp = ud_blocks; ud_blocks_cp != ud_blocks_e; ++ud_blocks_cp) {
			block_cp = ud_blocks_cp->block + hash;
			if (block_cp->str) {
				block_e = ud_blocks_cp->block + 1024;
				blockoff_c = 0;
				
				block_temp = block_cp;
				do {
					if (++block_cp == block_e) block_cp = ud_blocks_cp->block;
					++blockoff_c;
					if (block_cp->str) {
						if (!strcmp(block_cp->str, start)) {
							bpl_block = block_cp;
							presult = 0;
							goto fexit;
						}
					} else {
						if (blockoff_bst > blockoff_c) {
							bpl_block = block_cp;
							ud_blocks_tr = ud_blocks_cp;

							blockoff_bst = blockoff_c;
							presult = 2;
						}
						break;
					}
				} while (block_temp != block_cp);
			} else if (presult == 2) {
				bpl_block = block_cp;
				ud_blocks_tr = ud_blocks_cp;
				
				blockoff_bst = 0;
				presult = 1;
				break;
			}

			if (nhas_underthreshold && ud_blocks_cp->elem_count < UD_BLOCK_THRESHOLD) nhas_underthreshold = 0;
		}
		if (ud_blocks_cp == ud_blocks_e && nhas_underthreshold) {
			ud_blocks = realloc(ud_blocks, (ud_blocks_count <<= 1) * sizeof(struct ud_block));
			ud_blocks_e = ud_blocks + ud_blocks_count;
			for (struct ud_block *ib = ud_blocks + (ud_blocks_count >> 1); ib != ud_blocks_e; ++ib) {
				ib->block = calloc(1024, sizeof(struct block));
				ib->elem_count = 0;
			}
		}
	fexit:;
	}

	if (presult) {
		bpl_block->str = start;
		bpl_block->mcount = 1;
		++ud_blocks_tr->elem_count;

		if (top100_min.next->str) {
			bpl_block->greater = top100_min.next;
			bpl_block->smaller = &top100_min;
			top100_min.next->smaller = bpl_block;
			top100_min.next = bpl_block;
		} else {
			bpl_block->greater = &top100_max;
			bpl_block->smaller = &top100_min;
			top100_max.next = top100_min.next = bpl_block;
		}
	} else {
		unsigned short tcount = ++bpl_block->mcount, hcount = 0;
		block_e = bpl_block;
		while ((block_e = block_e->greater)->str && tcount > block_e->mcount) ++hcount;

		if (hcount) {
			if (block_e->str) {
				block_e = block_e->smaller; //Set real target pos

				if (hcount == 1) {
					block_e->greater->smaller = bpl_block;
					bpl_block->greater = block_e->greater;
					block_e->greater = bpl_block;

					block_e->smaller = bpl_block->smaller;
					bpl_block->smaller = block_e;
					if (block_e->smaller->str) block_e->smaller->greater = block_e;
					else top100_min.next = block_e;
				} else {
					block_e->greater->smaller = bpl_block;
					bpl_block->greater->smaller = block_e;

					block_e->smaller->greater = bpl_block;

					block_temp = bpl_block->greater;
					bpl_block->greater = block_e->greater;
					block_e->greater = block_temp;

					if (bpl_block->smaller->str) {
						bpl_block->smaller->greater = block_e;

						block_temp = bpl_block->smaller;
						bpl_block->smaller = block_e->smaller;
						block_e->smaller = block_temp;
					} else {
						top100_min.next = block_e;

						bpl_block->smaller = block_e->smaller;
						block_e->smaller = &top100_min;;
					}
				}
			} else {
				block_e = top100_max.next; //Set real target pos

				if (hcount == 1) {
					top100_max.next = bpl_block;
					bpl_block->greater = &top100_max;
					block_e->greater = bpl_block;

					block_e->smaller = bpl_block->smaller;
					bpl_block->smaller = block_e;
					if (block_e->smaller->str) block_e->smaller->greater = block_e;
					else top100_min.next = block_e;
				} else {
					top100_max.next = bpl_block;
					bpl_block->greater->smaller = block_e;

					block_e->smaller->greater = bpl_block;

					block_e->greater = bpl_block->greater;
					bpl_block->greater = &top100_max;
					if (bpl_block->smaller->str) {
						bpl_block->smaller->greater = block_e;

						block_temp = bpl_block->smaller;
						bpl_block->smaller = block_e->smaller;
						block_e->smaller = block_temp;
					} else {
						top100_min.next = block_e;

						bpl_block->smaller = block_e->smaller;
						block_e->smaller = &top100_min;
					}
				}
			}
		}
	}
}

int main() {
	FILE *of;
	
	if (!(of = fopen("text.txt", "rb"))) {
		printf("Can't open file!\n");
		goto exit;
	}

	fseek(of, 0, SEEK_END);
	unsigned int fsize = ftell(of) + 1;
	fseek(of, 0, SEEK_SET);

	char *fbytes = (char*)malloc(fsize);
	fread(fbytes, 1, fsize, of);

	fclose(of);

	//Prepare unordered dictionary
	ud_blocks = malloc(sizeof(struct ud_block));
	ud_blocks_count = 1;
	ud_blocks_e = ud_blocks + 1;

	ud_blocks->block = calloc(1024, sizeof(struct block));
	ud_blocks->elem_count = 0;

	top100_max.next = &top100_min;
	top100_min.next = &top100_max;
	//End of preparing unordered dictionary

	for (char *fend = fbytes + fsize, *fcp = fbytes /*File start and end*/, *strfs = fbytes, *strfe = fbytes /*String start and end*/; fcp != fend; ++fcp) {
		switch (*fcp) {
			case '.':
			case '?':
			case '!':
			case ':':
			case ';':
			case ',':
			case '-':
			case '(':
			case ')':
			case '"':
			case ' ':
			case '\n':
				if (strfs == strfe)
					strfs = ++strfe;
				else {
					*strfe = '\0';

					PlaceString(strfs, strfe);
					strfs = strfe = fcp + 1;
				}
				break;
			default:
				++strfe;
				break;
		}
	}

	{
		printf("Place. Count  -  Word\n");
		struct block *blocks_cp = top100_max.next;
		for (unsigned char i = 0; blocks_cp->str && i != 100; blocks_cp = blocks_cp->smaller, ++i)
			printf("%4hu. %5hu   -   %s\n", i + 1, blocks_cp->mcount, blocks_cp->str);
	}

	for (struct ud_block *ud_blocks_cp = ud_blocks; ud_blocks_cp != ud_blocks_e; ++ud_blocks_cp)
		free(ud_blocks_cp->block);
	free(ud_blocks);

	free(fbytes);

exit:;
	syste
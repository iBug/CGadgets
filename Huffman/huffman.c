// File: huffman.c
// Author: iBug

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#endif

// 64 KiB buffer can provide enough performance
#define BUFSIZE 65536

typedef unsigned char byte;

typedef struct _HuffNode {
    unsigned data;
    struct _HuffNode *left, *right, *parent;
} HuffNode;

void count_frequency(FILE* fp, uint32_t* freq) {
    size_t orig_pos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    size_t end_pos = ftell(fp);
    fseek(fp, orig_pos, SEEK_SET);
    size_t size = end_pos - orig_pos;

    // Use a buffer
    byte *buf = malloc(BUFSIZE * sizeof(buf));
    while (size >= BUFSIZE) {
        fread(buf, BUFSIZE, 1, fp);
        for (int i = 0; i < BUFSIZE; i++)
            freq[buf[i]]++;
        size -= BUFSIZE;
    }
    if (size > 0) {
        fread(buf, size, 1, fp);
        for (int i = 0; i < size; i++)
            freq[buf[i]]++;
        size -= size;
    }
    fseek(fp, orig_pos, SEEK_SET);
    free(buf);
}

void construct_huffman(const uint32_t* freq_in, HuffNode* tree) {
    int count = 256;
    uint32_t freq[256];
    HuffNode *node[256];

    // Initialize data
    for (int i = 0; i < 256; i++) {
        freq[i] = freq_in[i];
        tree[i].data = i;
        tree[i].left = tree[i].right = NULL;
        node[i] = &tree[i];
    }

    // Sort by frequency, decreasing order
    /* WARNING: Although this Quick Sort is an unstable sort,
     * it should at least give the same result for the same input frequency table,
     * therefore I'm leaving this code here
     */
    {
        uint32_t lower[256], upper[256], top = 1;
        lower[0] = 0, upper[0] = 256;
        while (top > 0) {
            top--;
            int left = lower[top], right = upper[top];
            int i = left, j = right - 1, flag = 0;
            if (i >= j) // Nothing to sort
                continue;
            while (i < j) {
                if (freq[i] < freq[j]) {
                    uint32_t t = freq[i]; freq[i] = freq[j]; freq[j] = t;
                    HuffNode *p = node[i]; node[i] = node[j]; node[j] = p;
                    flag = !flag;
                }
                flag ? i++ : j--;
            }
            lower[top] = left, upper[top] = i;
            lower[top + 1] = i + 1, upper[top + 1] = right;
            top += 2;
        }
    }

    // Construct tree
    while (count > 1) {
        int pos = 512 - count;
        HuffNode *parent = &tree[pos];
        // Select lowest 2 by freq
        int i = count - 2, j = count - 1;
        // Create tree, lower freq left
        parent->left = node[j]; parent->right = node[i];
        node[j]->parent = node[i]->parent = parent;
        node[i] = parent;
        freq[i] += freq[j];
        // Insert
        for (; i > 0 && freq[i] > freq[i - 1]; i--) {
            uint32_t t = freq[i]; freq[i] = freq[i - 1]; freq[i - 1] = t;
            HuffNode *p = node[i]; node[i] = node[i - 1]; node[i - 1] = p;
        }
        count--;
    }
    // Now HEAD = node[0] = tree[511]
    node[0]->parent = NULL;
}

void encode_stream(FILE* fin, FILE* fout, const HuffNode* tree, uint32_t* padding) {
    int n;
    byte ch;
    byte buf = 0, nbuf = 0;
    const HuffNode *p;

    // First create a lookup table
    byte *code[256];
    size_t codelen[256];
    for (int i = 0; i < 256; i++) {
        byte t[256];
        // Encode
        p = &tree[i];
        n = 0;
        while (p->parent) {
            if (p == p->parent->left) {
                // Left is 0
                t[n] = 0;
            } else if (p == p->parent->right) {
                t[n] = 1;
            }
            p = p->parent;
            n++;
        }
        codelen[i] = n;
        code[i] = malloc(n * sizeof(byte));
        for (int j = 0; j < n; j++)
            code[i][j] = t[n - 1 - j];
    }

    size_t startpos = ftell(fin);
    fseek(fin, 0L, SEEK_END);
    size_t endpos = ftell(fin);
    fseek(fin, startpos, SEEK_SET);
    size_t size = endpos - startpos;
    byte *readbuf = malloc(BUFSIZE * sizeof(byte)),
         *writebuf = malloc(BUFSIZE * sizeof(byte));
    size_t readsize = 0, writesize = 0;
    while (1) {
        if (readsize == 0) {
            if (size == 0)
                goto EXIT;
            if (size >= BUFSIZE)
                readsize = BUFSIZE;
            else
                readsize = size;
            fread(readbuf + BUFSIZE - readsize, readsize, 1, fin);
            size -= readsize;
        }
        ch = readbuf[BUFSIZE - readsize];
        readsize--;

        // Write
        for (int i = 0; i < codelen[ch]; i++) {
            buf |= code[ch][i] << nbuf;
            nbuf++;
            if (nbuf == 8) {
                writebuf[writesize++] = buf;
                if (writesize == BUFSIZE) {
                    fwrite(writebuf, writesize, 1, fout);
                    writesize = 0;
                }
                nbuf = buf = 0;
            }
        }
    }
EXIT:
    if (nbuf == 0) {
        *padding = 0;
    } else {
        *padding = 8 - nbuf;
        writebuf[writesize++] = buf;
    }
    fwrite(writebuf, writesize, 1, fout);
    writesize = 0;
    free(readbuf);
    free(writebuf);
    for (int i = 0; i < 256; i++)
        free(code[i]);
}

void decode_stream(FILE* fin, FILE* fout, const HuffNode* tree, uint32_t padding) {
    size_t startpos = ftell(fin); // should be 1028
    fseek(fin, 0L, SEEK_END);
    size_t endpos = ftell(fin); // last byte handling
    fseek(fin, startpos, SEEK_SET);
    size_t size = endpos - startpos;
    byte *readbuf = malloc(BUFSIZE * sizeof(byte)),
         *writebuf = malloc(BUFSIZE * sizeof(byte));
    size_t readsize = 0, writesize = 0;

    byte buf = 0, nbuf = 0, bit;
    const HuffNode *p;
    while (size > 0 || readsize > 0 || nbuf > 0) {
        // Start from tree top
        p = tree + 510;
        while (p->left || p->right) {
            // Prepare next bit if needed
            if (nbuf == 0) {
                if (readsize == 0) {
                    if (size == 0)
                        goto EXIT;
                    if (size >= BUFSIZE)
                        readsize = BUFSIZE;
                    else
                        readsize = size;
                    fread(readbuf + BUFSIZE - readsize, readsize, 1, fin);
                    size -= readsize;
                }
                buf = readbuf[BUFSIZE - readsize];
                readsize--;

                if (readsize == 0 && size == 0) {
                    // Last bit
                    nbuf = 8 - padding;
                    if (nbuf == 0)
                        goto EXIT;
                } else {
                    nbuf = 8;
                }
            }
            // p has child
            bit = buf & 1;
            buf >>= 1;
            nbuf--;
            if (bit == 0)
                p = p->left;
            else
                p = p->right;
        }
        writebuf[writesize++] = p->data;
        if (writesize == BUFSIZE) {
            fwrite(writebuf, writesize, 1, fout);
            writesize = 0;
        }
    }
EXIT:
    if (writesize > 0)
        fwrite(writebuf, writesize, 1, fout);
    free(readbuf);
    free(writebuf);
}

void compress_file(const char* filename, const char* newname) {
    FILE *fin = fopen(filename, "rb"),
         *fout = fopen(newname, "wb");

    uint32_t freq[256] = {}, padding;
    HuffNode tree[512];
    size_t padding_pos;
    count_frequency(fin, freq);
    construct_huffman(freq, tree);
    rewind(fin);
    fwrite(freq, 1024, 1, fout);
    // Write a placeholder for the padding
    padding_pos = ftell(fout);
    fwrite(&padding, 4, 1, fout);
    encode_stream(fin, fout, tree, &padding);
    // Write the padding to the placeholder
    fseek(fout, padding_pos, SEEK_SET);
    fwrite(&padding, 4, 1, fout);
    fclose(fin);
    fclose(fout);
}

void uncompress_file(const char* filename, const char* newname) {
    FILE *fin = fopen(filename, "rb"),
         *fout = fopen(newname, "wb");
    uint32_t freq[256], padding;
    HuffNode tree[512];
    fread(freq, 1024, 1, fin);
    fread(&padding, 4, 1, fin);
    construct_huffman(freq, tree);
    decode_stream(fin, fout, tree, padding);
    fclose(fin);
    fclose(fout);
}

void print_help(void) {
    puts("Usage: huffman (-c|-d) input output");
    puts("  -c    Compress file from input to output");
    puts("  -d    Uncompress file from input to output");
    puts("\nCreated by iBug");
}

int main(int argc, char** argv) {
    #ifdef _WIN32
    const char *ALL_FILES = "All files\0*.*\0",
               *HUFF_FILE = "All files\0*.*\0Huffman-encoded file (.huff)\0*.huff\0";
    if (argc == 1) {
        int op = MessageBox(NULL, "Do you want to compress?\n\nSelect [Yes] to compress\nSelect [No] to uncompress",
            "Huffman", 36);
        char infile[260] = {}, outfile[260] = {};
        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;

        ofn.lpstrFile = infile;
        ofn.nMaxFile = sizeof(infile);
        ofn.nFilterIndex = 2;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (op == 6)
            ofn.lpstrFilter = ALL_FILES;
        else if (op == 7)
            ofn.lpstrFilter = HUFF_FILE;
        GetOpenFileNameA(&ofn);

        ofn.lpstrFile = outfile;
        ofn.nMaxFile = sizeof(outfile);
        ofn.nFilterIndex = 2;
        ofn.Flags = 0;
        if (op == 6)
            ofn.lpstrFilter = HUFF_FILE;
        else if (op == 7)
            ofn.lpstrFilter = ALL_FILES;
        GetSaveFileNameA(&ofn);

        if (op == 6)
            compress_file(infile, outfile);
        else if (op == 7)
            uncompress_file(infile, outfile);
        else
            return 2;
    } else
    #endif
    if (argc != 4) {
        print_help();
        return 1;
    } else if (!strcmp(argv[1], "-c")) {
        compress_file(argv[2], argv[3]);
    } else if (!strcmp(argv[1], "-d")) {
        uncompress_file(argv[2], argv[3]);
    } else {
        print_help();
        return 1;
    }
    return 0;
}

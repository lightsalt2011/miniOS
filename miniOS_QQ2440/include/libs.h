#ifndef LIBS_H
#define LIBS_H

void xtos(unsigned long n);
void dtos(unsigned long n);
void memset(char * dest, long len, int value);
char * memcpy(char * dest, const char * src, long len);
void mem_dump(const unsigned long * src, long len);
void stack_dump(unsigned long * sp);

#endif
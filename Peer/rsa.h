#pragma once

int UpdateKey();
int check_prime(unsigned long long int n);
unsigned long int encrypt(unsigned long long int, unsigned long long int, unsigned long int);
unsigned short int decrypt(unsigned long long int, unsigned long long int, unsigned long int);
void rsaenc(char * ori, char * enc, unsigned long long int * key, unsigned long int len);
void rsadec(char * enc, char * ori, unsigned long long int * key, unsigned long int len);
unsigned long long int mul(unsigned long long int a, unsigned long long int b, unsigned long long int m);
unsigned long long int jpl(unsigned long long int b, unsigned long long int e, unsigned long long int m);
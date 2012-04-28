#include <stdio.h>
#include <stdlib.h>

char K[] = { 1,0,1,0,1,0,1,0,1,0,1,0 };
char E[] = { 2,1,2,3,4,5,4,5,6,7,8,1 };  // 1 origin
char S[8][64] = {                        // 1 origin
  { 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
     0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
     4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13, },
  { 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9, },
  { 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
    13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12, },
  {  7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
    13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
    10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14, },
  {  2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
    14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
     4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3, },
  { 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13, },
  {  4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
    13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12, },
  { 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
     1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11, },
};

char P[] = { 7, 8, 2, 1, 4, 6, 5, 3 };  // 1 origin

enum eType {
  eEncrypt,
  eDecrypt,
};

void s_transform(int n, char* preS, char* postS){
  int row    = preS[0]*2 + preS[5];
  int column = preS[1]*8 + preS[2]*4 + preS[3]*2 + preS[4];
  int value  = S[n][row*16+column];
  int bit = 8;
  for (int i = 0; i < 4; i++) {
    postS[i] = ( value & bit ) ? 1 : 0;
    bit >>= 1;
  }
}

void feistel_function(char* out_R, char* in_R) {
  char preS[12];
  for (int i = 0; i < 12; i++) {
    preS[i] = in_R[ E[i]-1 ];  // extension E
  }
  for (int i = 0; i < 12; i++) {
    preS[i] ^= K[i];           // work xor K
  }
  char postS[8];
  s_transform(0, &preS[0], &postS[0]);
  s_transform(1, &preS[6], &postS[4]);
  for (int i = 0; i < 8; i++) {
    out_R[i]  = postS[ P[i]-1 ];
  }
}

void print_result(char* l, char* r, int num) {
  printf("L=");
  for (int i = 0; i < num; i++) {
    printf("%d", l[i]);
  }
  printf(" ");
  printf("R=");
  for (int i = 0; i < num; i++) {
    printf("%d", r[i]);
  }
  printf("\n");
}

void swap_array(char* l, char* r, int num) {
  for (int i =0 ; i < num; i++){
    char temp = r[i];
    r[i] = l[i];
    l[i] = temp;
  }
}

void des_like(char* l, char* r, int num, eType type) {
  if (type == eDecrypt) {
    swap_array(l, r, 8);
  }
  char postF[8];
  feistel_function(postF, r);
  for (int i = 0; i < 8; i++) { postF[i] ^= l[i]; }
  for (int i = 0; i < 8; i++) { l[i] = postF[i];  }
  if (type == eEncrypt){
    swap_array(l, r, 8);
  }
}

int main(int argc, char* argv[]) {
  char L[8];
  char R[8];
  if (argc == 3) {
    for(int i = 0; i < 8; i++){
      L[i] = argv[1][i] - '0';
      R[i] = argv[2][i] - '0';
    }
  }else{
    printf("usage:\n");
    printf("  des_like 00000000 11111111\n");
    exit(0);
  }
  des_like(L, R, 8, eEncrypt);
  print_result(L, R, 8);
  des_like(L, R, 8, eDecrypt);
  print_result(L, R, 8);
}

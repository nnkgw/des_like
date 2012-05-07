#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum eOperation {
  eEncrypt,
  eDecrypt,
};

enum eMode {
  eECB,
  eCBC,
  eCFB,
  eOFB,
};

char K[] = { 1,0,1,0,1,0,1,0,1,0,1,0 };
char E[] = { 2,1,2,3,4,5,4,5,6,7,8,1 };  // 1 origin
char P[] = { 7,8,2,1,4,6,5,3 };          // 1 origin
char S[2][64] = {                        // 0 origin
  { 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
     0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
     4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13, },
  { 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9, },
};

void s_transform(int n, char* preS, char* postS){
  int row    = preS[0]*2 + preS[5];
  int column = preS[1]*8 + preS[2]*4 + preS[3]*2 + preS[4];
  int value  = S[n][row*16+column];
  int bit = 8;
  for (int i = 0; i < 4; i++) {
    postS[i] = (value & bit) ? 1 : 0;
    bit >>= 1;
  }
}

void feistel_function(char* out_R, char* in_R) {
  char preS[12];
  for (int i = 0; i < 12; i++) { preS[i] = in_R[ E[i]-1 ]; }  // extension E
  for (int i = 0; i < 12; i++) { preS[i] ^= K[i];          }  // xor K
  char postS[8];
  s_transform(0, &preS[0], &postS[0]);
  s_transform(1, &preS[6], &postS[4]);
  for (int i = 0; i < 8; i++) { out_R[i] = postS[ P[i]-1 ]; }
}

void print_result(char* l, char* r, int num, char* text) {
  printf("%s", text);
  for (int i = 0; i < num; i++) { printf("%d", l[i]); }
  printf(" ");
  for (int i = 0; i < num; i++) { printf("%d", r[i]); }
  printf("\n");
}

void swap_array(char* l, char* r, int num) {
  for (int i =0 ; i < num; i++){
    char temp = r[i];
    r[i] = l[i];
    l[i] = temp;
  }
}

void xor_iv(char* l, char* r, char* iv) {
  for (int i = 0; i < 8; i++) { 
    l[i] ^= iv[i];
    r[i] ^= iv[i+8];
  }
}

void des_like(char* l, char* r, char* iv, int num, eOperation op, eMode mode) {
  if (op == eDecrypt) { swap_array(l, r, 8); }
  else{ 
    if (mode == eCBC) { xor_iv(l, r, iv); }
  }
  char postF[8];
  feistel_function(postF, r);
  for (int i = 0; i < 8; i++) { postF[i] ^= l[i]; }
  for (int i = 0; i < 8; i++) { l[i] = postF[i];  }
  if (op == eEncrypt) { swap_array(l, r, 8); }
  else{
    if (mode == eCBC) { xor_iv(l, r, iv); }
  }
}

void read_vector_from_argv(char* vec, char* arg, int num) {
  for(int i = 0; i < num; i++){
    vec[i] = arg[i] - '0';
  }
}

int main(int argc, char* argv[]) {
  char L[8];
  char R[8];
  char IV[16];
  eMode mode = eECB;
  if ((argc == 3)||(argc == 4)){
    read_vector_from_argv(L, &argv[1][0], 8);
    read_vector_from_argv(R, &argv[2][0], 8);
    if (argc == 4) { // optional
      if (strncmp(argv[3], "CBC:", 4) == 0){ mode = eCBC; }
      if (strncmp(argv[3], "CFB:", 4) == 0){ mode = eCFB; }
      if (strncmp(argv[3], "OFB:", 4) == 0){ mode = eOFB; }
      if (mode != eECB){ read_vector_from_argv(IV, &argv[3][4], 16); }
    }
  }else{
    printf("usage:\n  des_like 00000000 11111111 [CBC:1111111100000000]\n");
    exit(0);
  }
  if ((mode == eCFB)||(mode == eOFB)) { des_like(&IV[0], &IV[8], 0, 8, eEncrypt, eECB); }
  if ((mode == eOFB)||(mode == eCFB)) {
    xor_iv(L, R, IV);
    print_result(L, R, 8, "Encrypt:");
    xor_iv(L, R, IV);
    print_result(L, R, 8, "Decrypt:");
    if (mode == eOFB) { print_result(&IV[0], &IV[8], 8, "Random :"); }
  }else{
    des_like(L, R, IV, 8, eEncrypt, mode);
    print_result(L, R, 8, "Encrypt:");
    des_like(L, R, IV, 8, eDecrypt, mode);
    print_result(L, R, 8, "Decrypt:");
  }
}

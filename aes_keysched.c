/* Universidad Nacional Autónoma de México */
/* Paulo Contreras Flores */
/* Jorge Luis Ferrusca */
/* paulo.contreras.flores@gmail.com */
/* jorge.ferrusca@bec.seguridad.unam.mx */


// compilar gcc aes_keysched.c -o aes_keysched.o

#include <stdio.h>
#include <stdlib.h>
#include "aes.h"
#include <stdint.h>

/*FROM https://en.wikipedia.org/wiki/Rijndael_S-box*/

#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

B32 RotWord(B32);
// void initialize_aes_sbox(uint8_t[]);
void initialize_aes_sbox(uint8_t[]);
uint8_t byValue(uint8_t);
B32 SubWord(B32);
// void printState(int, int, B8);
void printKey(B32, B32, B32, B32);
void SubBytes(B8[4][4]);

uint8_t sbox[256];

B8 input[4][4] = {
  {0x32, 0x88, 0x31, 0xe0},
  {0x43, 0x5a, 0x31, 0x37}, 
  {0xf6, 0x30, 0x98, 0x07},
  {0xa8, 0x8d, 0xa2, 0x34}
};

B8 output[4][4] = {
  {0x00, 0x00, 0x00, 0x00}, 
  {0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00}, 
  {0x00, 0x00, 0x00, 0x00}
};

int multMatrix[4][4] = {
  {2, 3, 1, 1},
  {1, 2, 3, 1},
  {1, 1, 2, 3},
  {3, 1, 1, 2},
};

B8 etable[256] = { 0x01, 0x03, 0x05, 0x0F, 0x11, 0x33, 0x55, 0xFF, 0x1A, 0x2E, 0x72, 0x96, 0xA1, 0xF8, 0x13, 0x35, 0x5F, 0xE1, 0x38, 0x48, 0xD8, 0x73, 0x95, 0xA4, 0xF7, 0x02, 0x06, 0x0A, 0x1E, 0x22, 0x66, 0xAA, 0xE5, 0x34, 0x5C, 0xE4, 0x37, 0x59, 0xEB, 0x26, 0x6A, 0xBE, 0xD9, 0x70, 0x90, 0xAB, 0xE6, 0x31, 0x53, 0xF5, 0x04, 0x0C, 0x14, 0x3C, 0x44, 0xCC, 0x4F, 0xD1, 0x68, 0xB8, 0xD3, 0x6E, 0xB2, 0xCD, 0x4C, 0xD4, 0x67, 0xA9, 0xE0, 0x3B, 0x4D, 0xD7, 0x62, 0xA6, 0xF1, 0x08, 0x18, 0x28, 0x78, 0x88, 0x83, 0x9E, 0xB9, 0xD0, 0x6B, 0xBD, 0xDC, 0x7F, 0x81, 0x98, 0xB3, 0xCE, 0x49, 0xDB, 0x76, 0x9A, 0xB5, 0xC4, 0x57, 0xF9, 0x10, 0x30, 0x50, 0xF0, 0x0B, 0x1D, 0x27, 0x69, 0xBB, 0xD6, 0x61, 0xA3, 0xFE, 0x19, 0x2B, 0x7D, 0x87, 0x92, 0xAD, 0xEC, 0x2F, 0x71, 0x93, 0xAE, 0xE9, 0x20, 0x60, 0xA0, 0xFB, 0x16, 0x3A, 0x4E, 0xD2, 0x6D, 0xB7, 0xC2, 0x5D, 0xE7, 0x32, 0x56, 0xFA, 0x15, 0x3F, 0x41, 0xC3, 0x5E, 0xE2, 0x3D, 0x47, 0xC9, 0x40, 0xC0, 0x5B, 0xED, 0x2C, 0x74, 0x9C, 0xBF, 0xDA, 0x75, 0x9F, 0xBA, 0xD5, 0x64, 0xAC, 0xEF, 0x2A, 0x7E, 0x82, 0x9D, 0xBC, 0xDF, 0x7A, 0x8E, 0x89, 0x80, 0x9B, 0xB6, 0xC1, 0x58, 0xE8, 0x23, 0x65, 0xAF, 0xEA, 0x25, 0x6F, 0xB1, 0xC8, 0x43, 0xC5, 0x54, 0xFC, 0x1F, 0x21, 0x63, 0xA5, 0xF4, 0x07, 0x09, 0x1B, 0x2D, 0x77, 0x99, 0xB0, 0xCB, 0x46, 0xCA, 0x45, 0xCF, 0x4A, 0xDE, 0x79, 0x8B, 0x86, 0x91, 0xA8, 0xE3, 0x3E, 0x42, 0xC6, 0x51, 0xF3, 0x0E, 0x12, 0x36, 0x5A, 0xEE, 0x29, 0x7B, 0x8D, 0x8C, 0x8F, 0x8A, 0x85, 0x94, 0xA7, 0xF2, 0x0D, 0x17, 0x39, 0x4B, 0xDD, 0x7C, 0x84, 0x97, 0xA2, 0xFD, 0x1C, 0x24, 0x6C, 0xB4, 0xC7, 0x52, 0xF6, 0x01};

B8 ltable[256] = { 0x00, 0x00, 0x19, 0x01, 0x32, 0x02, 0x1A, 0xC6, 0x4B, 0xC7, 0x1B, 0x68, 0x33, 0xEE, 0xDF, 0x03, 0x64, 0x04, 0xE0, 0x0E, 0x34, 0x8D, 0x81, 0xEF, 0x4C, 0x71, 0x08, 0xC8, 0xF8, 0x69, 0x1C, 0xC1, 0x7D, 0xC2, 0x1D, 0xB5, 0xF9, 0xB9, 0x27, 0x6A, 0x4D, 0xE4, 0xA6, 0x72, 0x9A, 0xC9, 0x09, 0x78, 0x65, 0x2F, 0x8A, 0x05, 0x21, 0x0F, 0xE1, 0x24, 0x12, 0xF0, 0x82, 0x45, 0x35, 0x93, 0xDA, 0x8E, 0x96, 0x8F, 0xDB, 0xBD, 0x36, 0xD0, 0xCE, 0x94, 0x13, 0x5C, 0xD2, 0xF1, 0x40, 0x46, 0x83, 0x38, 0x66, 0xDD, 0xFD, 0x30, 0xBF, 0x06, 0x8B, 0x62, 0xB3, 0x25, 0xE2, 0x98, 0x22, 0x88, 0x91, 0x10, 0x7E, 0x6E, 0x48, 0xC3, 0xA3, 0xB6, 0x1E, 0x42, 0x3A, 0x6B, 0x28, 0x54, 0xFA, 0x85, 0x3D, 0xBA, 0x2B, 0x79, 0x0A, 0x15, 0x9B, 0x9F, 0x5E, 0xCA, 0x4E, 0xD4, 0xAC, 0xE5, 0xF3, 0x73, 0xA7, 0x57, 0xAF, 0x58, 0xA8, 0x50, 0xF4, 0xEA, 0xD6, 0x74, 0x4F, 0xAE, 0xE9, 0xD5, 0xE7, 0xE6, 0xAD, 0xE8, 0x2C, 0xD7, 0x75, 0x7A, 0xEB, 0x16, 0x0B, 0xF5, 0x59, 0xCB, 0x5F, 0xB0, 0x9C, 0xA9, 0x51, 0xA0, 0x7F, 0x0C, 0xF6, 0x6F, 0x17, 0xC4, 0x49, 0xEC, 0xD8, 0x43, 0x1F, 0x2D, 0xA4, 0x76, 0x7B, 0xB7, 0xCC, 0xBB, 0x3E, 0x5A, 0xFB, 0x60, 0xB1, 0x86, 0x3B, 0x52, 0xA1, 0x6C, 0xAA, 0x55, 0x29, 0x9D, 0x97, 0xB2, 0x87, 0x90, 0x61, 0xBE, 0xDC, 0xFC, 0xBC, 0x95, 0xCF, 0xCD, 0x37, 0x3F, 0x5B, 0xD1, 0x53, 0x39, 0x84, 0x3C, 0x41, 0xA2, 0x6D, 0x47, 0x14, 0x2A, 0x9E, 0x5D, 0x56, 0xF2, 0xD3, 0xAB, 0x44, 0x11, 0x92, 0xD9, 0x23, 0x20, 0x2E, 0x89, 0xB4, 0x7C, 0xB8, 0x26, 0x77, 0x99, 0xE3, 0xA5, 0x67, 0x4A, 0xED, 0xDE, 0xC5, 0x31, 0xFE, 0x18, 0x0D, 0x63, 0x8C, 0x80, 0xC0, 0xF7, 0x70, 0x07};

void printState(int rows, int cols, B8 in[rows][cols]) {
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      printf("%02x", in[j][i]);
    }
  }
  printf("\n");
}

void printKey(B32 w1, B32 w2, B32 w3, B32 w4) {
  printf("%08x",w1);
  printf("%08x",w2);
  printf("%08x",w3);
  printf("%08x",w4);
  printf("\n");
}

void initialize_aes_sbox(uint8_t sbox[]) {
  uint8_t p = 1, q = 1;
  
  /* loop invariant: p * q == 1 in the Galois field */
  do {
    /* multiply p by 3 */
    p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

    /* divide q by 3 (equals multiplication by 0xf6) */
    q ^= q << 1;
    q ^= q << 2;
    q ^= q << 4;
    q ^= q & 0x80 ? 0x09 : 0;

    /* compute the affine transformation */
    uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

    sbox[p] = xformed ^ 0x63;
  } while (p != 1);

  /* 0 is a special case since it has no inverse */
  sbox[0] = 0x63;
}

B32 Rcon[10] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

main(){
    B8 key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
                  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    B16 keylength = 128;

    initialize_aes_sbox(sbox);
    KeyBlockRound(keylength);
    KeyExpansion(key, w, Nk);
    printf("\t%s\n", "--------------------- CIFRADO ---------------------");
    Cipher(input, output, w);
}



void KeyExpansion(B8 key[4*Nk], B32 w[Nb*(Nr+1)], B16 Nk){

  B32 temp;
  B16 i = 0;

  while(i < Nk){
    // corrimiento izq 8, 16 o 24 bits para unirlos en 32 bits 
    w[i] = ((key[4*i]) << 24) | ((key[4*i+1]) << 16) |
             ((key[4*i+2]) << 8) | ((key[4*i+3])); 
    // if(debug){
    //   printf("\t%x\n", key[4*i]<<24);
    //   printf("\t  %x\n", key[4*i+1]<<16);
    //   printf("\t    %x\n", key[4*i+2]<<8);
    //   printf("\t      %x\n", key[4*i+3]);
    //   printf("-------------------\n");
    //   printf(" w[%i] = %x\n\n", i,w[i]);
    // }
    i = i + 1;
  }
  i = Nk;

  printf("%12s", "i");
  printf("%12s", "temp");
  printf("%12s", "Aft Rotword");
  printf("%12s", "Aft Subword");
  printf("%12s", "Rcon[i/Nk]");
  printf("%12s", "Af Xor Rcon");
  printf("%12s", "w[i-Nk]");
  printf("%12s", "tmp|w[i-nk]");
  printf("\n");
  while(i < Nb * (Nr+1)) {
    printf("%12d",i);
    B32 temp = w[i-1];
    printf("%4s%08x", " ", temp);
    if(i % Nk == 0) {
      temp = SubWord(RotWord(temp));
      //after subword
      printf("%12x", temp);
      //rcon
      printf("%4s%08x", " ", Rcon[(i/Nk)-1]); //rest 1 because of indexes
      temp = temp ^ Rcon[(i/Nk)-1];
      //aft rcon
      printf("%4s%08x", " ", temp);
    }
    else if (Nk > 6 && i % Nk == 4) {
      temp = SubWord(temp);
    } else {
      printf("%12s", " ");
      printf("%12s", " ");
      printf("%12s", " ");
      printf("%12s", " ");
    }
    //w[[i-Nk]]
    printf("%4s%08x", " ", w[i-Nk]);
    w[i] = w[i-Nk] ^ temp;
    //w[i]=temp XOR w[i-Nk]
    printf("%4s%08x", " ", w[i]);
    printf("\n\n");
    i += 1;
  }
}

B32 RotWord(B32 in){
  return ((in >> 24)&(0x000000FF) | (in << 8)&(0xFFFFFF00));
}

B32 SubWord(B32 in){
  // after rotword
  printf("%12x", in);
  return (
    (sbox[(in>>24)&(0x000000FF)] <<24) |
    (sbox[(in>>16)&(0x000000FF)] <<16) |
    (sbox[(in>>8)&(0x000000FF)] << 8) |
    sbox[(in)&(0x000000FF)]
  );
}

uint8_t byValue(uint8_t a) {
  return a;
}

void KeyBlockRound(B16 keylength){

    switch(keylength){
        case 128: Nk = 4; Nr = 10; break;
        case 192: Nk = 6; Nr = 12; break;
        case 256: Nk = 8; Nr = 14; break;
        default: printf("Error-longitud de llave incorrecto\n");
    }

}

void SubBytes (B8 state[4][Nb]) {
  for(int i = 0; i < 4 ; i++) {
    for(int j = 0; j < 4; j++) {
      state[i][j] = sbox[state[i][j]];
    }
  }
  // return in;
}

B8 shift(B8 r, B8 c) {
  return r;
}

void ShiftRows(int rows, int cols, B8 in[rows][cols]) {
  int tempRow[4];
  for(int r = 0; r < rows; r++) {
    if(r > 0) { 
      //performed only in last three rows
      for (int c = 0; c < cols; ++c)
      {
        tempRow[c] = in[r][c];
        // in[r][c] = in[r][(c + shift(r, Nb)) % Nb];
      }
      for (int c = 0; c < cols; ++c)
      {
        in[r][c] = tempRow[(c+shift(r,Nb))%4];
      }
    }
  }
}

void AddRoundKey(B8 state[4][Nb], B32 w1, B32 w2, B32 w3, B32 w4){
  for (int i = 0; i < 4; ++i)
  {
    state[i][0] = state[i][0] ^ (w1>>abs(-3+i)*8);
    state[i][1] = state[i][1] ^ (w2>>abs(-3+i)*8);
    state[i][2] = state[i][2] ^ (w3>>abs(-3+i)*8);
    state[i][3] = state[i][3] ^ (w4>>abs(-3+i)*8);
  }
}

B8 lookupTables(B8 stateValue, int multMatrix){
  if(stateValue == 0x00 || multMatrix == 0x00) return 0x00;
  if(stateValue == 0x01) return multMatrix;
  if(multMatrix == 0x01) return stateValue;
  int sum = ltable[stateValue] + ltable[multMatrix];
  // printf("%x - %x\n", stateValue, multMatrix);
  // printf("%x - %x\n", ltable[stateValue], ltable[multMatrix]);
  // printf("%s %x\n", "suma:", sum);
  if (sum > 0xFF) {
    sum -= 0xFF;
  }
  // printf("%s %x\n", "suma despues:", sum);
  // printf("%s %x\n\n", "Valor en etable: ", etable[sum]);
  return etable[sum];
}

void MixColumns(B8 state[4][Nb]) {
  B8 copyState[4][4];
  //COPY BY VALUE CHAKA 
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < Nb; ++j)
    {
      copyState[i][j] = state[i][j];
    }
  } 
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < Nb; ++j)
    {
      state[j][i] = lookupTables(copyState[0][i], multMatrix[j][0]) ^ lookupTables(copyState[1][i], multMatrix[j][1]) ^ lookupTables(copyState[2][i], multMatrix[j][2]) ^ lookupTables(copyState[3][i], multMatrix[j][3]);
      // printf("%s %d\n", "Row", i);
      // printf("%x", state[j][j]);
    }
  }
}

void assignState(B8   in[4][Nb], B8 state[4][Nb]){
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < Nb; ++j)
    {
      state[i][j] = in[i][j];
    }
  }
}

void Cipher(B8 in[4][Nb], B8 out[4*Nb], B32 w[Nb*(Nr+1)]) {
  B8 state[4][Nb];
  // B8 output[4][4];
  assignState(in, state);
  printf("\tround[ %d].input     ", 0);
  printState(4, Nb, state);
  printf("\tround[ %d].k_sch     ", 0);
  printKey(w[0], w[1], w[2], w[3]);
  AddRoundKey(state, w[0], w[1], w[2], w[3]);

  for(int round = 1; round < Nr; round++){
    printf("\tround[ %d].start     ", round);
    printState(4, Nb, state);
    
    SubBytes(state);
    printf("\tround[ %d].s_box     ", round);
    printState(4, Nb, state);
    
    ShiftRows(4, Nb, state);
    printf("\tround[ %d].s_row     ", round);
    printState(4, Nb, state);
    
    MixColumns(state);
    printf("\tround[ %d].m_col     ", round);
    printState(4, Nb, state);

    printf("\tround[ %d].k_sch     ", round);
    printKey(w[round*Nb], w[((round+1)*Nb)-3] , w[((round+1)*Nb)-2],  w[((round+1)*Nb)-1]);
    AddRoundKey(state, w[round*Nb], w[((round+1)*Nb)-3] , w[((round+1)*Nb)-2],  w[((round+1)*Nb)-1]);
  }
    // SubBytes(4, Nb, 4, Nb, state);
    // ShiftRows(4, Nb, 4, Nb, state);
    // AddRoundKey(4, Nb, state, w[Nr*Nb, (Nr+1)*Nb-1]);
    // out = state;
    printf("\tround[%d].start     ", Nr);
    printState(4, Nb, state);
    
    SubBytes(state);
    printf("\tround[%d].s_box     ", Nr);
    printState(4, Nb, state);
    
    ShiftRows(4, Nb, state);
    printf("\tround[%d].s_row     ", Nr);
    printState(4, Nb, state);

    printf("\tround[%d].k_sch     ", Nr);
    printKey(w[Nr*Nb], w[((Nr+1)*Nb)-3] , w[((Nr+1)*Nb)-2],  w[((Nr+1)*Nb)-1]);
    AddRoundKey(state, w[Nr*Nb], w[((Nr+1)*Nb)-3] , w[((Nr+1)*Nb)-2],  w[((Nr+1)*Nb)-1]);

    printf("\tround[%d].output    ", Nr);
    out = state;
    printState(4, Nb, out);

}

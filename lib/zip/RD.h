

#ifndef _CE_RD_
#define _CE_RD_

#define WM_CLIENT_ACCEPT WM_USER+101
#define WM_CLIENT_READCLOSE WM_USER+102
#define WM_CLIENT_READCOLOSE WM_USER + 103


BYTE Logtable[256] = {
  0,   0,  25,   1,  50,   2,  26, 198,  75, 199,  27, 104,  51, 238, 223,   3, 
100,   4, 224,  14,  52, 141, 129, 239,  76, 113,   8, 200, 248, 105,  28, 193, 
125, 194,  29, 181, 249, 185,  39, 106,  77, 228, 166, 114, 154, 201,   9, 120, 
101,  47, 138,   5,  33,  15, 225,  36,  18, 240, 130,  69,  53, 147, 218, 142, 
150, 143, 219, 189,  54, 208, 206, 148,  19,  92, 210, 241,  64,  70, 131,  56, 
102, 221, 253,  48, 191,   6, 139,  98, 179,  37, 226, 152,  34, 136, 145,  16, 
126, 110,  72, 195, 163, 182,  30,  66,  58, 107,  40,  84, 250, 133,  61, 186, 
 43, 121,  10,  21, 155, 159,  94, 202,  78, 212, 172, 229, 243, 115, 167,  87, 
175,  88, 168,  80, 244, 234, 214, 116,  79, 174, 233, 213, 231, 230, 173, 232, 
 44, 215, 117, 122, 235,  22,  11, 245,  89, 203,  95, 176, 156, 169,  81, 160, 
127,  12, 246, 111,  23, 196,  73, 236, 216,  67,  31,  45, 164, 118, 123, 183, 
204, 187,  62,  90, 251,  96, 177, 134,  59,  82, 161, 108, 170,  85,  41, 157, 
151, 178, 135, 144,  97, 190, 220, 252, 188, 149, 207, 205,  55,  63,  91, 209, 
 83,  57, 132,  60,  65, 162, 109,  71,  20,  42, 158,  93,  86, 242, 211, 171, 
 68,  17, 146, 217,  35,  32,  46, 137, 180, 124, 184,  38, 119, 153, 227, 165, 
103,  74, 237, 222, 197,  49, 254,  24,  13,  99, 140, 128, 192, 247, 112,   7, 
};

BYTE Alogtable[256] = {
  1,   3,   5,  15,  17,  51,  85, 255,  26,  46, 114, 150, 161, 248,  19,  53, 
 95, 225,  56,  72, 216, 115, 149, 164, 247,   2,   6,  10,  30,  34, 102, 170, 
229,  52,  92, 228,  55,  89, 235,  38, 106, 190, 217, 112, 144, 171, 230,  49, 
 83, 245,   4,  12,  20,  60,  68, 204,  79, 209, 104, 184, 211, 110, 178, 205, 
 76, 212, 103, 169, 224,  59,  77, 215,  98, 166, 241,   8,  24,  40, 120, 136, 
131, 158, 185, 208, 107, 189, 220, 127, 129, 152, 179, 206,  73, 219, 118, 154, 
181, 196,  87, 249,  16,  48,  80, 240,  11,  29,  39, 105, 187, 214,  97, 163, 
254,  25,  43, 125, 135, 146, 173, 236,  47, 113, 147, 174, 233,  32,  96, 160, 
251,  22,  58,  78, 210, 109, 183, 194,  93, 231,  50,  86, 250,  21,  63,  65, 
195,  94, 226,  61,  71, 201,  64, 192,  91, 237,  44, 116, 156, 191, 218, 117, 
159, 186, 213, 100, 172, 239,  42, 126, 130, 157, 188, 223, 122, 142, 137, 128, 
155, 182, 193,  88, 232,  35, 101, 175, 234,  37, 111, 177, 200,  67, 197,  84, 
252,  31,  33,  99, 165, 244,   7,   9,  27,  45, 119, 153, 176, 203,  70, 202, 
 69, 207,  74, 222, 121, 139, 134, 145, 168, 227,  62,  66, 198,  81, 243,  14, 
 18,  54,  90, 238,  41, 123, 141, 140, 143, 138, 133, 148, 167, 242,  13,  23, 
 57,  75, 221, 124, 132, 151, 162, 253,  28,  36, 108, 180, 199,  82, 246,   1, 
};

BYTE cTable[2]={1,3};
BYTE dTable[4]={0x96,0x52,0x5f,0xb6,};
BYTE kTable[4]={1,4,};

BYTE S[256]={
	217,  27,  70,  77, 127, 197, 215, 165, 139, 190,  23, 124,  40, 222, 232, 233,
               19,  93, 214,   5,  43,  96, 149, 109,   2,  86,  52, 218, 229, 162, 126, 253,
			  130, 155, 251, 114,  10, 186, 237, 247, 248, 125, 228, 138, 173,  54,  75, 188,
			  239, 194, 234, 110, 220, 133,  33,  11,  34, 250,  78, 108, 211,  55, 216, 117,
			  174, 111, 112, 143,  60, 244,  39, 116, 167, 196,  29,  94,   7,  62, 122,  16,
			    3, 226,  44, 191,  65,  73, 243, 235,  37, 141, 219, 119, 189,  61, 104, 184,
			  131,  81,  89, 105,  42, 101,  71,  63, 161, 176, 207,   1, 242, 246, 135,  26,
			  132, 171,  84,  14, 238, 182,  45, 202,  15,  53, 151, 137,   6,  25, 205, 107,
			  106, 224,  79, 121, 120,  13,  57, 195,  49,  88, 181, 208,  74, 128,  76, 231,
			   48, 178,  99, 142,  67, 159, 210,  56,   9, 168, 221,  90,  41,  47,  59, 172,
			  179, 140, 201, 152,  32,  46,  80, 230,  38, 154,  97,  28,  51,  95, 252, 203,
			   50, 192,  35, 236, 241, 254,  91, 150, 223, 213, 185,  66, 115, 156, 144, 177,
			    0,  31,  20,  24, 145,   8,  12, 166,  83,  98,   4,  72, 157, 180, 200,  21,
			  169, 225, 170,  30, 153,  58,  36, 147, 206, 183, 193, 204, 255, 199,  17, 163,
			  212, 158, 240,  69,  85,  92, 148, 175,  87, 198, 134, 146,  18, 187,  82, 118,
			  103, 164,  64, 160, 123, 245, 102, 249, 100, 113, 227, 129, 136,  68, 209,  22,

};

BYTE Si[256] = {

 192,107, 24, 80,202, 19,124, 76,197,152, 36, 55,198,133,115,120,
 79,222,236, 16,194,207,255, 10,195,125,111,  1,171, 74,211,193,
164, 54, 56,178,214, 88,168, 70, 12,156,100, 20, 82,118,165,157,
144,136,176,172, 26,121, 45, 61,151,134,213,158, 68, 93, 77,103,
242, 84,187,148,253,227,  2,102,203, 85,140, 46,142,  3, 58,130,
166, 97,238,200,114,228, 25,232,137, 98,155,182,229, 17, 75,173,
 21,170,201,146,248,101,246,240, 94, 99,128,127, 59, 23, 51, 65,
 66,249, 35,188, 71, 63,239, 91,132,131, 78,244, 11, 41, 30,  4,
141,251, 32, 96,112, 53,234,110,252,123, 43,  8,161, 89,147, 67,
190,196,235,215,230, 22,183,122,163,212,169, 33,189,204,225,149,
243,104, 29,223,241,  7,199, 72,153,208,210,113,159, 44, 64,231,
105,191,145,160,205,138,117,217, 95,186, 37,237, 47, 92,  9, 83,
177,218, 49,135, 73,  5,233,221,206,162,119,175,219,126,216,106,
139,254,150, 60,224,185, 18,  6, 62,  0, 27, 90, 52,154, 13,184,
129,209, 81,250, 42, 28,167,143, 14, 15, 50, 87,179, 38,116, 48,
226,180,108, 86, 69,245,109, 39, 40,247, 57, 34,174, 31,181,220,

};

BYTE M[256]={0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c,0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x3c,
              0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,0x60,0x64,0x68,0x6c,0x70,0x74,0x78,0x7c,
			  0x80,0x84,0x88,0x8c,0x90,0x94,0x98,0x9c,0xa0,0xa4,0xa8,0xac,0xb0,0xb4,0xb8,0xbc,
			  0xc0,0xc4,0xc8,0xcc,0xd0,0xd4,0xd8,0xdc,0xe0,0xe4,0xe8,0xec,0xf0,0xf4,0xf8,0xfc,
			  0x1b,0x1f,0x13,0x17,0x0b,0x0f,0x03,0x07,0x3b,0x3f,0x33,0x37,0x2b,0x2f,0x23,0x27,
			  0x5b,0x5f,0x53,0x57,0x4b,0x4f,0x43,0x47,0x7b,0x7f,0x73,0x77,0x6b,0x6f,0x63,0x67,
			  0x9b,0x9f,0x93,0x97,0x8b,0x8f,0x83,0x87,0xbb,0xbf,0xb3,0xb7,0xab,0xaf,0xa3,0xa7,
			  0xdb,0xdf,0xd3,0xd7,0xcb,0xcf,0xc3,0xc7,0xfb,0xff,0xf3,0xf7,0xeb,0xef,0xe3,0xe7,
			  0x36,0x32,0x3e,0x3a,0x26,0x22,0x2e,0x2a,0x16,0x12,0x1e,0x1a,0x06,0x02,0x0e,0x0a,
			  0x76,0x72,0x7e,0x7a,0x66,0x62,0x6e,0x6a,0x56,0x52,0x5e,0x5a,0x46,0x42,0x4e,0x4a,
			  0xb6,0xb2,0xbe,0xba,0xa6,0xa2,0xae,0xaa,0x96,0x92,0x9e,0x9a,0x86,0x82,0x8e,0x8a,
			  0xf6,0xf2,0xfe,0xfa,0xe6,0xe2,0xee,0xea,0xd6,0xd2,0xde,0xda,0xc6,0xc2,0xce,0xca,
			  0x2d,0x29,0x25,0x21,0x3d,0x39,0x35,0x31,0x0d,0x09,0x05,0x01,0x1d,0x19,0x15,0x11,
			  0x6d,0x69,0x65,0x61,0x7d,0x79,0x75,0x71,0x4d,0x49,0x45,0x41,0x5d,0x59,0x55,0x51,
			  0xad,0xa9,0xa5,0xa1,0xbd,0xb9,0xb5,0xb1,0x8d,0x89,0x85,0x81,0x9d,0x99,0x95,0x91,
			  0xed,0xe9,0xe5,0xe1,0xfd,0xf9,0xf5,0xf1,0xcd,0xc9,0xc5,0xc1,0xdd,0xd9,0xd5,0xd1,

};

BYTE iG[4][4] = {
  { 0x0e, 0x09, 0x0d, 0x0b, }, 
  { 0x0b, 0x0e, 0x09, 0x0d, },
  { 0x0d, 0x0b, 0x0e, 0x09, },
  { 0x09, 0x0d, 0x0b, 0x0e, },
};

BYTE rcon[30] = { 
  0x01,0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, };

BYTE shifts[4][3][4] = { 
  {
    {1, 2, 3, 0,},
    { 2, 3, 0, 1,},
    { 3, 0, 1, 2,},
  },					 
  {
    { 3, 0, 1, 2, },
    { 2, 3, 0, 1, },
    { 1, 2, 3, 0, },
  },
  {
    { 0, 1, 2, 3, },
    { 0, 1, 2, 3, },
    { 0, 1, 2, 3, },
  },				 
  {
    { 2, 3, 0, 1, },
    { 0, 1, 2, 3, },
    { 2, 3, 0, 1, },
  },
}; 

#endif

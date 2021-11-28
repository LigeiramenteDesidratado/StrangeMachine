#ifndef BIT_MASKING_H
#define BIT_MASKING_H

#define MASK_SET(n, f) ((n) |= (f))
#define MASK_CLR(n, f) ((n) &= ~(f))
#define MASK_TGL(n, f) ((n) ^= (f))
#define MASK_CHK(n, f) ((n) & (f))

#endif // BIT_MASKING_H


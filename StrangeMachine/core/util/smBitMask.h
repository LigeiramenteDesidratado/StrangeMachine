#ifndef SM_CORE_UTIL_BIT_MASK_H
#define SM_CORE_UTIL_BIT_MASK_H

#define SM_MASK_SET(n, f)    ((n) |= (f))
#define SM_MASK_CLR(n, f)    ((n) &= ~(f))
#define SM_MASK_TGL(n, f)    ((n) ^= (f))
#define SM_MASK_CHK(n, f)    ((n) & (f))
#define SM_MASK_CHK_EQ(n, f) (((n) & (f)) == (f))

#endif /* SM_CORE_UTIL_BIT_MASK_H */

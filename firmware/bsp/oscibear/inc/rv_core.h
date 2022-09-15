
#ifndef __RV_CORE_H
#define __RV_CORE_H

#ifdef __cplusplus
  #define   __I     volatile             /** Defines "read only" permissions */
#else
  #define   __I     volatile const       /** Defines "read only" permissions */
#endif
#define     __O     volatile             /** Defines "write only" permissions */
#define     __IO    volatile             /** Defines "read / write" permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /** Defines "read only" structure member permissions */
#define     __OM     volatile            /** Defines "write only" structure member permissions */
#define     __IOM    volatile            /** Defines "read / write" structure member permissions */

/* ================ basic types ================ */
#define SET_BITS(REG, BIT)                    ((REG) |= (BIT))
#define CLEAR_BITS(REG, BIT)                  ((REG) &= ~(BIT))
#define READ_BITS(REG, BIT)                   ((REG) & (BIT))
#define WRITE_BITS(REG, CLEARMASK, SETMASK)   ((REG) = (((REG) & (~(CLEARMASK))) | (SETMASK)))

typedef enum {
  OK = 0U,
  ERROR,
  BUSY,
  TIMEOUT
} Status;


#endif /* __RV_CORE_H */
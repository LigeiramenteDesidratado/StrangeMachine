#ifndef SM_CORE_KEY_CODE_H
#define SM_CORE_KEY_CODE_H

#include "smpch.h"

typedef uint16_t sm_key_code;

#define sm_key_a         ((sm_key_code)4)
#define sm_key_b         ((sm_key_code)5)
#define sm_key_c         ((sm_key_code)6)
#define sm_key_d         ((sm_key_code)7)
#define sm_key_e         ((sm_key_code)8)
#define sm_key_f         ((sm_key_code)9)
#define sm_key_g         ((sm_key_code)10)
#define sm_key_h         ((sm_key_code)11)
#define sm_key_i         ((sm_key_code)12)
#define sm_key_j         ((sm_key_code)13)
#define sm_key_k         ((sm_key_code)14)
#define sm_key_l         ((sm_key_code)15)
#define sm_key_m         ((sm_key_code)16)
#define sm_key_n         ((sm_key_code)17)
#define sm_key_o         ((sm_key_code)18)
#define sm_key_p         ((sm_key_code)19)
#define sm_key_q         ((sm_key_code)20)
#define sm_key_r         ((sm_key_code)21)
#define sm_key_s         ((sm_key_code)22)
#define sm_key_t         ((sm_key_code)23)
#define sm_key_u         ((sm_key_code)24)
#define sm_key_v         ((sm_key_code)25)
#define sm_key_w         ((sm_key_code)26)
#define sm_key_x         ((sm_key_code)27)
#define sm_key_y         ((sm_key_code)28)
#define sm_key_z         ((sm_key_code)29)
#define sm_key_1         ((sm_key_code)30)
#define sm_key_2         ((sm_key_code)31)
#define sm_key_3         ((sm_key_code)32)
#define sm_key_4         ((sm_key_code)33)
#define sm_key_5         ((sm_key_code)34)
#define sm_key_6         ((sm_key_code)35)
#define sm_key_7         ((sm_key_code)36)
#define sm_key_8         ((sm_key_code)37)
#define sm_key_9         ((sm_key_code)38)
#define sm_key_0         ((sm_key_code)39)
#define sm_key_enter     ((sm_key_code)40)
#define sm_key_escape    ((sm_key_code)41)
#define sm_key_backspace ((sm_key_code)42)
#define sm_key_tab       ((sm_key_code)43)
#define sm_key_space     ((sm_key_code)44)

#define sm_key_period ((sm_key_code)55)
#define sm_key_comma  ((sm_key_code)54)
#define sm_key_slash  ((sm_key_code)56)

#define sm_key_right ((sm_key_code)79)
#define sm_key_left  ((sm_key_code)80)
#define sm_key_down  ((sm_key_code)81)
#define sm_key_up    ((sm_key_code)82)

#define sm_key_lctrl  ((sm_key_code)224)
#define sm_key_lshift ((sm_key_code)225)
#define sm_key_lalt   ((sm_key_code)226)
#define sm_key_rctrl  ((sm_key_code)228)
#define sm_key_rshift ((sm_key_code)229)
#define sm_key_ralt   ((sm_key_code)230)

const char *sm_key_to_str(sm_key_code code);

#endif /* SM_CORE_KEY_CODE_H */

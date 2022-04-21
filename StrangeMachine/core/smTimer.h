#ifndef SM_CORE_TIMER_H
#define SM_CORE_TIMER_H

struct timer_s;

struct timer_s *timer_new(void);

void timer_ctor(struct timer_s *t);
void timer_dtor(struct timer_s *t);
void timer_reset(struct timer_s *t);
double timer_get_elapsed(struct timer_s *t);

#endif /* SM_CORE_TIMER_H */

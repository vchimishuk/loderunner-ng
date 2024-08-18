#ifndef EXIT_H_
#define EXIT_H_

__attribute__((noreturn)) void die(char *fmt, ...);
void die_sdl(char *func);

#endif /* EXIT_H_ */

#ifndef DONNEES_H
#define DONNEES_H

#include <stddef.h>
#include <stdint.h>

uint64_t xorshift64(uint64_t *state);

void gen_aleatoire(int *t, size_t n, uint64_t *state);
void gen_trie(int *t, size_t n, uint64_t *state);
void gen_inverse(int *t, size_t n, uint64_t *state);
void gen_presque_trie(int *t, size_t n, uint64_t *state);
void gen_doublons(int *t, size_t n, uint64_t *state);

void array_copie(int *dest, const int *src, size_t n);

typedef void (*gen_func)(int *t, size_t n, uint64_t *state);

typedef struct {
  const char *nom;
  gen_func f;
} Jeu;

extern const Jeu JEUX[];
extern const size_t NB_JEUX;
#endif // !

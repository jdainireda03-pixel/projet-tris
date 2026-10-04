#include "donnees.h"

#include <string.h>

#define DECALAGE_31_BITS 33
#define NB_VALEURS_DOUBLONS 10
#define DIVISEUR_PERTURB 20
#define DISTANCE_MAX 10

uint64_t xorshift64(uint64_t *state) {
  uint64_t x = *state;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  *state = x;
  return x;
}

void gen_aleatoire(int *t, size_t n, uint64_t *state) {
  for (size_t i = 0; i < n; i++) {
    t[i] = (int)(xorshift64(state) >> DECALAGE_31_BITS);
  }
}

void gen_trie(int *t, size_t n, uint64_t *state) {
  (void)state;
  for (size_t i = 0; i < n; i++) {
    t[i] = (int)i;
  }
}

void gen_inverse(int *t, size_t n, uint64_t *state) {
  (void)state;
  for (size_t i = 0; i < n; i++) {
    t[i] = (int)(n - 1 - i);
  }
}

void gen_doublons(int *t, size_t n, uint64_t *state) {
  for (size_t i = 0; i < n; i++) {
    t[i] = (int)(xorshift64(state) % NB_VALEURS_DOUBLONS);
  }
}

void gen_presque_trie(int *t, size_t n, uint64_t *state) {
  gen_trie(t, n, state);
  if (n < 2) {
    return;
  }
  size_t nb_echanges = n / DIVISEUR_PERTURB;
  for (size_t k = 0; k < nb_echanges; k++) {
    size_t i = (size_t)(xorshift64(state) % (n - 1));
    size_t d = 1 + (size_t)(xorshift64(state) % DISTANCE_MAX);
    size_t j = i + d;
    if (j > n - 1) {
      j = n - 1;
    }
    int tmp = t[i];
    t[i] = t[j];
    t[j] = tmp;
  }
}

void array_copie(int *dest, const int *src, size_t n) {
  if (n > 0) {
    memcpy(dest, src, n * sizeof(int));
  }
}

const Jeu JEUX[] = {
    {"aleatoire", gen_aleatoire}, {"trie", gen_trie},
    {"inverse", gen_inverse},     {"presque_trie", gen_presque_trie},
    {"doublons", gen_doublons},
};

const size_t NB_JEUX = sizeof(JEUX) / sizeof(JEUX[0]);

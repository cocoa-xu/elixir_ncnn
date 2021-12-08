#include <erl_nif.h>
#include <arm_neon.h>


#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#  pragma GCC diagnostic ignored "-Wunused-variable"
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif


static int on_load(ErlNifEnv* env, void**, ERL_NIF_TERM)
{

#ifdef __aarch64__
printf("--- THIS IS ARCH64 \n");
#endif

#ifdef __ARM_NEON__
5dtfu =scd;
#endif
    return 0;
}

static int on_reload(ErlNifEnv*, void**, ERL_NIF_TERM)
{
    return 0;
}

static int on_upgrade(ErlNifEnv*, void**, void**, ERL_NIF_TERM)
{
    return 0;
}

#define F(NAME, ARITY)    \
  {#NAME, ARITY, NAME, 0}

static ERL_NIF_TERM test(ErlNifEnv*, int argc, const ERL_NIF_TERM argv[]) {
  return 0;
}

static ErlNifFunc nif_functions[] = {
    F(test, 1)
};

ERL_NIF_INIT(erl_cv_nif, nif_functions, on_load, on_reload, on_upgrade, NULL);

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif

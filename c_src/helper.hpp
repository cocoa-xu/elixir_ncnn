#ifndef __ELIXIR_NCNN_HELPER__
#define __ELIXIR_NCNN_HELPER__

#pragma once

#include <erl_nif.h>
#include <map>
#include <string>
#include "nif_utils.hpp"

static
ERL_NIF_TERM erlang_get_kw(ErlNifEnv *env, const std::map<std::string, ERL_NIF_TERM>& erl_terms, const std::string& key) {
    auto iter = erl_terms.find(key);
    if (iter == erl_terms.end()) {
        return erlang::nif::atom(env, "nil");
    }
    return iter->second;
}

bool erlang_to_binary(ErlNifEnv *env, ERL_NIF_TERM obj, ErlNifBinary* bin) {
    if (enif_term_to_binary(env, obj, bin)) {
        return true;
    }

    return false;
}

template<class T>
const T& clamp(const T& v, const T& lo, const T& hi)
{
    return v < lo ? lo : hi < v ? hi : v;
}

#endif  // __ELIXIR_NCNN_HELPER__

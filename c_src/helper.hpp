#ifndef __ELIXIR_NCNN_HELPER__
#define __ELIXIR_NCNN_HELPER__

#pragma once

#include <erl_nif.h>
#include <map>
#include <string>
#include "nif_utils.hpp"
#include "ncnn/net.h"

template<typename R>
struct erlang_res {
    R val;
    static ErlNifResourceType * type;
};
template<typename R> ErlNifResourceType * erlang_res<R>::type = nullptr;
static ErlNifResourceType * ncnn_net_type = nullptr;

template<typename R>
int alloc_resource(erlang_res<R> **res) {
    *res = (erlang_res<R> *)enif_alloc_resource(ncnn_net_type, sizeof(erlang_res<R>));
    return (*res != nullptr);
}

ERL_NIF_TERM erlang_from_net(ErlNifEnv *env, ncnn::Net* obj) {
    erlang_res<ncnn::Net*> * res;
    if (alloc_resource(&res)) {
        res->val = obj;
    } else {
        return erlang::nif::error(env, "no memory");
    }

    ERL_NIF_TERM ret = enif_make_resource(env, res);
    enif_release_resource(res);
    return ret;
}

ERL_NIF_TERM erlang_to_net(ErlNifEnv *env, ERL_NIF_TERM obj, ncnn::Net*& value) {
    if (erlang::nif::check_nil(env, obj)) {
        return false;
    }

    erlang_res<ncnn::Net *> * in_res;
    if (enif_get_resource(env, obj, ncnn_net_type, (void **)&in_res)) {
        if (in_res) {
            value = in_res->val;
            return true;
        }
    }
    return false;
}

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

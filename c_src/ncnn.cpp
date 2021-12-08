#include <erl_nif.h>
#include <memory>
#include <vector>
#include <tuple>
#include <string>
#include "ncnn/net.h"
#include "ncnn/mat.h"
#include "nif_utils.hpp"

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#  pragma GCC diagnostic ignored "-Wunused-variable"
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define ERL_TYPE_INIT(NIF_NAME, NAME, STORAGE)                                                      \
    {                                                                                               \
        rt = enif_open_resource_type(env, #NIF_NAME , "ncnn_##NAME##_type", destruct_##NAME ,       \
                ERL_NIF_RT_CREATE, NULL);                                                           \
        if (!rt) return 1;                                                                          \
        erlang_res<STORAGE>::type = rt;                                                             \
    }

#define ERL_TYPE_DECLARE(NAME, STORAGE, SNAME)                                                      \
    static bool erlang_##NAME##_getp(ErlNifEnv *env, ERL_NIF_TERM self, STORAGE * & dst)            \
    {                                                                                               \
        erlang_res<STORAGE> * VAR;                                                                  \
        if (!enif_get_resource(env, self, erlang_res<STORAGE>::type, (void **)&VAR))                \
            return enif_make_badarg(env);                                                           \
        else                                                                                        \
        {                                                                                           \
            dst = &(VAR->val);                                                                      \
            return true;                                                                            \
        }                                                                                           \
        return false;                                                                               \
    }                                                                                               \
    static ERL_NIF_TERM erlang_##NAME##_Instance(ErlNifEnv *env, const STORAGE &r)                  \
    {                                                                                               \
        erlang_res< STORAGE > * VAR;                                                                \
        VAR = (decltype(VAR))enif_alloc_resource(erlang_res< STORAGE >::type,                       \
                                sizeof(erlang_res< STORAGE >));                               \
        if (!VAR)                                                                                   \
            return erlang::nif::error(env, "no memory");                                            \
        new (&(VAR->val)) STORAGE(r);                                                               \
        ERL_NIF_TERM ret = enif_make_resource(env, VAR);                                            \
        enif_release_resource(VAR);                                                                 \
        return ret;                                                                                 \
    }                                                                                               \
    static void destruct_##NAME(ErlNifEnv *env, void *args)                                         \
    {                                                                                               \
        using namespace std;                                                                        \
    }

#define ERRWRAP2(expr, env, error_flag, error_term)  \
try                                                  \
{                                                    \
    expr;                                            \
}                                                    \
catch (const std::exception &e)                      \
{                                                    \
    error_flag = true;                               \
    error_term = erlang::nif::error(env, e.what());  \
}                                                    \
catch (...)                                          \
{                                                    \
    error_flag = true;                               \
    error_term = erlang::nif::error(env,            \
          "Unknown C++ exception from OpenCV code"); \
}

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

#define HAS_CONVERSION_ERROR(x) (((x) == -1))

// convert erlang term *from* T
template<typename T> static
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const T& obj) {
    return erlang::nif::error(env, "not implemented");
}

// convert erlang term *to* T
template<typename T> static
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM o, T& value) {
    fprintf(stderr, "some erlang_to not implemented\n");
    return false;
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

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, ncnn::Net*& value) {
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

template<>
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const bool& value)
{
    if (value) return erlang::nif::atom(env, "true");
    return erlang::nif::atom(env, "false");
}

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, bool& value)
{
    if (erlang::nif::check_nil(env, obj)) {
        return true;
    }

    if (enif_is_atom(env, obj))
    {
        std::string boolean_val;
        if (erlang::nif::get_atom(env, obj, boolean_val)) {
            value = (boolean_val == "true");
            return true;
        }
    }
    return false;
}

template<>
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const size_t& value)
{
    return enif_make_uint64(env, value);
}

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, size_t& value)
{
    if (erlang::nif::check_nil(env, obj)) {
        return true;
    }

    ErlNifUInt64 u64;

    if (enif_get_uint64(env, obj, &u64))
    {
        value = u64;
    }
    else
    {
        return false;
    }
    return true;
}

template<>
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const int& value)
{
    return enif_make_int(env, value);
}

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, int& value)
{
    if (erlang::nif::check_nil(env, obj)) {
        return true;
    }

    int32_t i32;

    if (enif_get_int(env, obj, &i32))
    {
        value = i32;
    }
    else
    {
        return false;
    }
    return !HAS_CONVERSION_ERROR(value);
}

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, char& value)
{
    if (erlang::nif::check_nil(env, obj)) {
        return true;
    }

    int32_t i32;
    if (enif_get_int(env, obj, &i32))
    {
        value = i32;
    } else {
        return false;
    }

    return !HAS_CONVERSION_ERROR(value);
}

template<>
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const double& value)
{
    return enif_make_double(env, value);
}

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, double& value)
{
    if (erlang::nif::check_nil(env, obj)) {
        return true;
    }

    double f64;
    long i64;
    if (enif_get_double(env, obj, &f64))
    {
        value = f64;
    } else if (enif_get_int64(env, obj, (ErlNifSInt64 *)&i64)) {
        value = i64;
    } else {
        return false;
    }

    return true;
}

template<>
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const float& value)
{
    return enif_make_double(env, value);
}

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, float& value)
{
    if (erlang::nif::check_nil(env, obj)) {
        return true;
    }

    ErlNifSInt64 i64;
    double f64;
    if (enif_get_int64(env, obj, (ErlNifSInt64 *)&i64))
    {
        value = static_cast<float>(i64);
    }
    else if (enif_get_double(env, obj, &f64))
    {
        value = static_cast<float>(f64);
    }
    else
    {
        return false;
    }
    return true;
}

template<>
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const int64_t& value)
{
    return enif_make_int64(env, value);
}

template<>
ERL_NIF_TERM erlang_from(ErlNifEnv *env, const std::string& value)
{
    return enif_make_string(env, value.empty() ? "" : value.c_str(), ERL_NIF_LATIN1);
}

template<>
bool erlang_to(ErlNifEnv *env, ERL_NIF_TERM obj, std::string &value)
{
    if (erlang::nif::check_nil(env, obj)) {
        return false;
    }

    std::string str;
    int ret = erlang::nif::get(env, obj, str);
    value = str;
    return (ret > 0);
}

bool erlang_to_binary(ErlNifEnv *env, ERL_NIF_TERM obj, ErlNifBinary* bin) {
    if (enif_term_to_binary(env, obj, bin)) {
        return true;
    }

    return false;
}

// exception-safe erlang_to
template<typename _Tp> static
bool erlang_to_safe(ErlNifEnv *env, ERL_NIF_TERM obj, _Tp& value) {
    try {
        return erlang_to(env, obj, value);
    } catch (...) {
        return false;
    }
}

static ERL_NIF_TERM net_new_net(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace ncnn;
    Net * net = new Net;
    return erlang::nif::ok(env, erlang_from_net(env, net));
}

static ERL_NIF_TERM net_load_param(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace ncnn;
    ERL_NIF_TERM error_term = 0;

    {
        ERL_NIF_TERM erl_term_net = erlang::nif::atom(env, "nil");
        Net * net;
        ERL_NIF_TERM erl_term_param_path = erlang::nif::atom(env, "nil");
        std::string param_path;
        int retval;

        const char *keywords[] = {"net", "param_path", NULL};
        if (0 < argc &&
            erlang::nif::parse_arg(env, 0, argv, (char **) keywords, "OO:", &erl_term_net, &erl_term_param_path) &&
                erlang_to_net(env, erl_term_net, net) &&
            erlang_to_safe(env, erl_term_param_path, param_path)
            ) {
            int error_flag = false;
            ERRWRAP2(retval = net->load_param(param_path.c_str()), env, error_flag, error_term);
            if (!error_flag) {
                return erlang::nif::ok(env, erlang_from_net(env, net));
            }
        }
    }

    if (error_term != 0) return error_term;
    else return erlang::nif::atom(env, "nil");
}

static ERL_NIF_TERM net_load_model(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace ncnn;
    ERL_NIF_TERM error_term = 0;

    {
        ERL_NIF_TERM erl_term_net = erlang::nif::atom(env, "nil");
        Net * net;
        ERL_NIF_TERM erl_term_model_path = erlang::nif::atom(env, "nil");
        std::string model_path;
        int retval;

        const char *keywords[] = {"net", "model_path", NULL};
        if (0 < argc &&
            erlang::nif::parse_arg(env, 0, argv, (char **) keywords, "OO:", &erl_term_net, &erl_term_model_path) &&
                erlang_to_net(env, erl_term_net, net) &&
            erlang_to_safe(env, erl_term_model_path, model_path))
        {
            int error_flag = false;
            ERRWRAP2(retval = net->load_model(model_path.c_str()), env, error_flag, error_term);
            if (!error_flag) {
                return erlang::nif::ok(env, erlang_from_net(env, net));
            }
        }
    }

    if (error_term != 0) return error_term;
    else return erlang::nif::atom(env, "nil");
}

template<class T>
const T& clamp(const T& v, const T& lo, const T& hi)
{
    return v < lo ? lo : hi < v ? hi : v;
}

struct Object
{
    struct rect {
        float x;
        float y;
        float width;
        float height;
    };
    int label;
    float prob;
};

static ERL_NIF_TERM net_forward(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace ncnn;
    ERL_NIF_TERM error_term = 0;

    {
        ERL_NIF_TERM erl_term_net = erlang::nif::atom(env, "nil");
        Net * net;
        ERL_NIF_TERM erl_term_data = erlang::nif::atom(env, "nil");
        ErlNifBinary data;
        ERL_NIF_TERM erl_term_cols = erlang::nif::atom(env, "nil");
        int img_cols = 0;
        ERL_NIF_TERM erl_term_rows = erlang::nif::atom(env, "nil");
        int img_rows = 0;
        int retval;

        const char *keywords[] = {"net", "data", "cols", "rows", NULL};
        if (0 < argc &&
            erlang::nif::parse_arg(env, 0, argv, (char **) keywords, "OOOO:",
                                   &erl_term_net, &erl_term_data, &erl_term_cols, &erl_term_rows) &&
            erlang_to_net(env, erl_term_net, net) &&
                erlang_to_binary(env, erl_term_data, &data) &&
                erlang_to_safe(env, erl_term_cols, img_cols) &&
                erlang_to_safe(env, erl_term_rows, img_rows))
        {
            const int target_size = 300;

            ncnn::Mat in = ncnn::Mat::from_pixels_resize((const unsigned char*)data.data, ncnn::Mat::PIXEL_BGR2RGB, img_cols, img_rows, target_size, target_size);

            const float mean_vals[3] = {123.675f, 116.28f, 103.53f};
            const float norm_vals[3] = {1.0f, 1.0f, 1.0f};
            in.substract_mean_normalize(mean_vals, norm_vals);

            ncnn::Extractor ex = net->create_extractor();

            ex.input("input", in);
            ncnn::Mat out;
            ex.extract("detection_out", out);

            ERL_NIF_TERM * detected = (ERL_NIF_TERM *)enif_alloc(sizeof(ERL_NIF_TERM) * out.h);
            for (int i = 0; i < out.h; i++)
            {
                const float* values = out.row(i);

                // filter out cross-boundary
                float x1 = clamp(values[2] * target_size, 0.f, float(target_size - 1)) / target_size * img_cols;
                float y1 = clamp(values[3] * target_size, 0.f, float(target_size - 1)) / target_size * img_rows;
                float x2 = clamp(values[4] * target_size, 0.f, float(target_size - 1)) / target_size * img_cols;
                float y2 = clamp(values[5] * target_size, 0.f, float(target_size - 1)) / target_size * img_rows;

                ERL_NIF_TERM obj = enif_make_tuple3(env, erlang_from(env, (int)values[0]), erlang_from(env, values[1]),
                    enif_make_tuple2(env,
                        enif_make_tuple2(env, erlang_from(env, x1), erlang_from(env, y1)),
                        enif_make_tuple2(env, erlang_from(env, x2-x1), erlang_from(env, y2-y1))
                    )
                );

                detected[i] = obj;
            }
            enif_release_binary(&data);
            return erlang::nif::ok(env, enif_make_list_from_array(env, detected, out.h));
        }
    }

    if (error_term != 0) return error_term;
    else return erlang::nif::atom(env, "nil");
}

ERL_TYPE_DECLARE(Net, std::shared_ptr<ncnn::Net>, shared_ptr);

static int on_load(ErlNifEnv* env, void**, ERL_NIF_TERM)
{
    ErlNifResourceType *rt;
    rt = enif_open_resource_type(env, "ncnn_nif" , "ncnn_net_type", NULL, ERL_NIF_RT_CREATE, NULL);
    if (!rt) return 1;
    ncnn_net_type = rt;
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

static ErlNifFunc nif_functions[] = {
    F(net_new_net, 0),
    F(net_load_model, 1),
    F(net_load_param, 1),
    F(net_forward, 1)
};

ERL_NIF_INIT(ncnn_nif, nif_functions, on_load, on_reload, on_upgrade, NULL);

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif

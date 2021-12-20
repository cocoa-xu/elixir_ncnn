#ifndef __MOBILE_NET_V3_SSDLITE_HPP__
#define __MOBILE_NET_V3_SSDLITE_HPP__

#pragma once

#include "../helper.hpp"
#include "ncnn/layer.h"

class MobileNetV2SSDLiteNoop : public ncnn::Layer
{
};
DEFINE_LAYER_CREATOR(MobileNetV2SSDLiteNoop)

static ERL_NIF_TERM mobile_net_v2_ssdlite_load(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    using namespace ncnn;
    ERL_NIF_TERM error_term = 0;

    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    if (nif_opts_index < argc) {
        erlang::nif::parse_arg(env, nif_opts_index, argv, erl_terms);
    }

    std::string param_path;
    std::string model_path;

    Net * retval = nullptr;

    {
        if (erlang_to_safe(env, erlang_get_kw(env, erl_terms, "param_path"), param_path) &&
            erlang_to_safe(env, erlang_get_kw(env, erl_terms, "model_path"), model_path))
        {
            retval = new Net;
            retval->register_custom_layer("Silence", MobileNetV2SSDLiteNoop_layer_creator);
            int error_flag = false;
            ERRWRAP2(retval->load_param(param_path.c_str()), env, error_flag, error_term);
            ERRWRAP2(retval->load_model(model_path.c_str()), env, error_flag, error_term);
            if (!error_flag) {
                return erlang::nif::ok(env, erlang_from_net(env, retval));
            }
        }
    }

    if (error_term != 0) return error_term;
    else return erlang::nif::atom(env, "nil");
}

static ERL_NIF_TERM mobile_net_forward(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace ncnn;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    if (nif_opts_index < argc) {
        erlang::nif::parse_arg(env, nif_opts_index, argv, erl_terms);
    }

    Net * net = nullptr;
    ErlNifBinary data;
    std::string version;
    int img_cols = 0;
    int img_rows = 0;

    {
        int retval;

        if (erlang_to_net(env, erlang_get_kw(env, erl_terms, "net"), net) &&
            erlang_to_safe(env, erlang_get_kw(env, erl_terms, "version"), version) &&
            erlang_to_binary(env, erlang_get_kw(env, erl_terms, "data"), &data) &&
            erlang_to_safe(env, erlang_get_kw(env, erl_terms, "cols"), img_cols) &&
            erlang_to_safe(env, erlang_get_kw(env, erl_terms, "rows"), img_rows))
        {
            std::string input_layer_name;
            float mean_vals[3] = {127.5f, 127.5f, 127.5f};
            float norm_vals[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
            if (version == "v1") {
                input_layer_name = "data";
            } else if (version == "v2") {
                input_layer_name = "data";
            } else if (version == "v3") {
                input_layer_name = "input";
                mean_vals[0] = 123.675f;
                mean_vals[1] = 116.28f;
                mean_vals[2] = 103.53f;
                norm_vals[0] = 1.0;
                norm_vals[1] = 1.0;
                norm_vals[2] = 1.0;
            }

            const int target_size = 300;
            ncnn::Mat in = ncnn::Mat::from_pixels_resize((const unsigned char*)data.data, ncnn::Mat::PIXEL_BGR2RGB, img_cols, img_rows, target_size, target_size);
            in.substract_mean_normalize(mean_vals, norm_vals);

            ncnn::Extractor ex = net->create_extractor();
            ex.input(input_layer_name.c_str(), in);

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

#endif  // __MOBILE_NET_V3_SSDLITE_HPP__

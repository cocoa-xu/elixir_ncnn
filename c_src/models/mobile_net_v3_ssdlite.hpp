#ifndef __MOBILE_NET_V3_SSDLITE_HPP__
#define __MOBILE_NET_V3_SSDLITE_HPP__

#pragma once

#include "helper.hpp"

static ERL_NIF_TERM mobile_net_v3_ssdlite_forward(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    using namespace ncnn;
    ERL_NIF_TERM error_term = 0;
    std::map<std::string, ERL_NIF_TERM> erl_terms;
    int nif_opts_index = 0;
    if (nif_opts_index < argc) {
        erlang::nif::parse_arg(env, nif_opts_index, argv, erl_terms);
    }

    Net * net = nullptr;
    ErlNifBinary data;
    int img_cols = 0;
    int img_rows = 0;

    {
        int retval;

        if (erlang_to_net(env, erlang_get_kw(env, erl_terms, "net"), net) &&
            erlang_to_binary(env, erlang_get_kw(env, erl_terms, "data"), &data) &&
            erlang_to_safe(env, erlang_get_kw(env, erl_terms, "cols"), img_cols) &&
            erlang_to_safe(env, erlang_get_kw(env, erl_terms, "rows"), img_rows))
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

#endif  // __MOBILE_NET_V3_SSDLITE_HPP__

#include <erl_nif.h>
#include <ei.h>
#include <stdbool.h>
#include <stdio.h>
#include "nif_utils.hpp"
#include "hnswlib_helper.hpp"
#include "hnswlib_index.hpp"
#include "hnswlib_nif_resource.h"

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

ErlNifResourceType * NifResHNSWLibIndex::type = nullptr;

static ERL_NIF_TERM hnswlib_new(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    if (argc != 7) {
        return erlang::nif::error(env, "expecting 7 arguments");
    }

    std::string space;
    size_t dim;
    size_t max_elements;
    size_t m = 16;
    size_t ef_construction = 200;
    size_t random_seed = 100;
    bool allow_replace_deleted = false;
    NifResHNSWLibIndex * index = nullptr;
    ERL_NIF_TERM ret, error;

    if (!erlang::nif::get_atom(env, argv[0], space)) {
        return erlang::nif::error(env, "expect parameter `space` to be an atom");
    }
    if (!erlang::nif::get(env, argv[1], &dim)) {
        return erlang::nif::error(env, "expect parameter `space` to be a non-negative integer");
    }
    if (!erlang::nif::get(env, argv[2], &max_elements)) {
        return erlang::nif::error(env, "expect parameter `max_elements` to be a non-negative integer");
    }
    if (!erlang::nif::get(env, argv[3], &m)) {
        return erlang::nif::error(env, "expect parameter `m` to be a non-negative integer");
    }
    if (!erlang::nif::get(env, argv[4], &ef_construction)) {
        return erlang::nif::error(env, "expect parameter `ef_construction` to be a non-negative integer");
    }
    if (!erlang::nif::get(env, argv[5], &random_seed)) {
        return erlang::nif::error(env, "expect parameter `random_seed` to be a non-negative integer");
    }
    if (!erlang::nif::get(env, argv[6], &allow_replace_deleted)) {
        return erlang::nif::error(env, "expect parameter `allow_replace_deleted` to be a boolean");
    }

    if ((index = NifResHNSWLibIndex::allocate_resource(env, error)) == nullptr) {
        return error;
    }

    index->val = nullptr;
    try {
        index->val = new Index<float>(space, dim);
        index->val->init_new_index(max_elements, m, ef_construction, random_seed, allow_replace_deleted);
    } catch (std::runtime_error &err) {
        if (index->val) {
            delete index->val;
        }
        enif_release_resource(index);
        return erlang::nif::error(env, err.what());
    }

    ret = enif_make_resource(env, index);
    enif_release_resource(index);
    return erlang::nif::ok(env, ret);
}

static ERL_NIF_TERM hnswlib_knn_query(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    if (argc != 7) {
        return erlang::nif::error(env, "expecting 7 arguments");
    }

    NifResHNSWLibIndex * index = nullptr;
    ErlNifBinary data;
    size_t k;
    ssize_t num_threads; 
    ERL_NIF_TERM filter;
    size_t rows, features;
    ERL_NIF_TERM ret, error;

    if ((index = NifResHNSWLibIndex::get_resource(env, argv[0], error)) == nullptr) {
        return error;
    }
    if (!enif_inspect_binary(env, argv[1], &data)) {
        return erlang::nif::error(env, "expect `data` to be a binary");
    }
    if (data.size % sizeof(float) != 0) {
        return erlang::nif::error(env, (
            std::string{"expect `data`'s size to be a multiple of "} + std::to_string(sizeof(float)) + " (sizeof(float)), got `" + std::to_string(data.size) + "` bytes").c_str());
    }
    if (!erlang::nif::get(env, argv[2], &k) || k == 0) {
        return erlang::nif::error(env, "expect parameter `k` to be a positive integer");
    }
    if (!erlang::nif::get(env, argv[3], &num_threads)) {
        return erlang::nif::error(env, "expect parameter `num_threads` to be an integer");
    }
    if (!enif_is_fun(env, argv[4]) && !erlang::nif::check_nil(env, argv[4])) {
        return erlang::nif::error(env, "expect parameter `filter` to be a function or `nil`");
    }
    if (!erlang::nif::get(env, argv[5], &rows)) {
        return erlang::nif::error(env, "expect parameter `rows` to be a non-negative integer");
    }
    if (!erlang::nif::get(env, argv[6], &features)) {
        return erlang::nif::error(env, "expect parameter `features` to be a non-negative integer");
    }

    ErlNifPid * process = (ErlNifPid *)enif_alloc(sizeof(ErlNifPid));
    if (process == nullptr) {
      return erlang::nif::error(env, "cannot allocate memory for ErlNifPid.");
    }
    process = enif_self(env, process);

    hnswlib::labeltype *data_l;
    float *data_d;

    enif_free(process);
    return erlang::nif::ok(env);
}

static ERL_NIF_TERM hnswlib_float_size(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    return enif_make_uint(env, sizeof(float));
}

static int on_load(ErlNifEnv *env, void **, ERL_NIF_TERM) {
    ErlNifResourceType *rt;

    rt = enif_open_resource_type(env, "Elixir.HNSWLib.Nif", "NifResHNSWLibIndex", NifResHNSWLibIndex::destruct_resource, ERL_NIF_RT_CREATE, NULL);
    if (!rt) return -1;
    NifResHNSWLibIndex::type = rt;

    return 0;
}

static int on_reload(ErlNifEnv *, void **, ERL_NIF_TERM) {
    return 0;
}

static int on_upgrade(ErlNifEnv *, void **, void **, ERL_NIF_TERM) {
    return 0;
}

static ErlNifFunc nif_functions[] = {
    {"new", 7, hnswlib_new, 0},
    {"knn_query", 7, hnswlib_knn_query, ERL_NIF_DIRTY_JOB_CPU_BOUND},
    {"float_size", 0, hnswlib_float_size, 0}
};

ERL_NIF_INIT(Elixir.HNSWLib.Nif, nif_functions, on_load, on_reload, on_upgrade, NULL);

#if defined(__GNUC__)
#pragma GCC visibility push(default)
#endif

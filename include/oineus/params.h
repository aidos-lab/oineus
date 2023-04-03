#ifndef OINEUS_PARAMS_H
#define OINEUS_PARAMS_H

namespace oineus {
    struct Params {
        int n_threads{1};
        int chunk_size{128};
        bool write_dgms{false};
        bool sort_dgms{true};
        bool clearing_opt{true};
        bool acq_rel{false};
        bool print_time{false};
        bool compute_v{true};
        bool compute_u{false};
        bool do_sanity_check{false};
        double elapsed{0.0};
        bool kernel{false};
        bool image{false};
        bool cokernel{false};
        bool verbose{false};
    };

    struct ThreadStats {
        const int thread_id;
        long int n_right_pivots {0};
        long int n_cleared {0};

        ThreadStats()
                :thread_id(-1) { }

        ThreadStats(int _thread_id)
                :thread_id(_thread_id) { }
    };
}
#endif //OINEUS_PARAMS_H

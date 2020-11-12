#pragma once

#include <vector>


// this function is auto-generated by scripts/freudenthal_cube.py
template<typename Int, size_t D>
std::vector<std::vector<std::array<Int, D>>> displacements(size_t dim)
{
    size_t ambient_dim = D;

    switch (ambient_dim) {
        case 1 :
            switch (dim) {
                case 0 : return {
                    { { 0 } } };
                case 1 : return {
                    { { 0 }, { 1 } } };
            }
        case 2 :
            switch (dim) {
                case 0 : return {
                    { { 0, 0 } } };
                case 1 : return {
                    { { 0, 0 }, { 0, 1 } },
                    { { 0, 0 }, { 1, 1 } },
                    { { 0, 0 }, { 1, 0 } } };
                case 2 : return {
                    { { 0, 0 }, { 1, 0 }, { 1, 1 } },
                    { { 0, 0 }, { 0, 1 }, { 1, 1 } } };
            }
        case 3 :
            switch (dim) {
                case 0 : return {
                    { { 0, 0, 0 } } };
                case 1 : return {
                    { { 0, 0, 0 }, { 0, 1, 0 } },
                    { { 0, 0, 0 }, { 1, 1, 0 } },
                    { { 0, 0, 0 }, { 0, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 0, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 } },
                    { { 0, 0, 0 }, { 1, 0, 1 } } };
                case 2 : return {
                    { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 } },
                    { { 0, 0, 0 }, { 1, 0, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 1, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 1, 0 }, { 0, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 0, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 } },
                    { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 } } };
                case 3 : return {
                    { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 } } };
            }
    }

    throw std::runtime_error("Unkown dimensions");
}


template<typename Int_, typename Real_, size_t D>
class Grid {

    using Int = Int_;
    using Real = Real_;
    using GridSimplex = Simplex<Int, Real>;
    using GridPoint = std::array<Int, D>;
    using PointVec = std::vector<GridPoint>;
    using SimplexVec = std::vector<GridSimplex>;

    static constexpr size_t dim { D };

    Grid(const GridPoint& _dims, bool wrap_) : dims_(_dims), strides_(D, 1), wrap_(_wrap), c_order_(true)
    {
        if (dims_.size() != dim)
            throw std::runtime_error("Dims size must be equal D");

        if (c_order_)
            for(size_t d = D - 1; d != 0; ++d)
                strides_[d] = dims_[d] * strides_[d+1];
        else
            throw std::runtime_error("Fortran order not implemented");

        powers_of_two_[0] = 1;
        for(size_t i = 1; i < powers_of_two_.size(); ++i)
        {
            powers_of_two_[i] = 2 * powers_of_two_[i-1];
        }
    }

    PointVec fr_link(const GridPoint& p) const
    {
        PointVec result;
        result.reserve(powers_of_two_[D] - 1);

        for(Int neighbor_idx = 1; neighbor_idx <= powers_of_two_[D]; ++neighbor_idx)
        {
            GridPoint cand = p;
            bool out_of_dims = false;

            for(size_t c = 0; c < D; ++c)
                if (neighbor_idx & powers_of_two_[c + 1]) {
                    ++cand[c];
                    out_of_dims = out_of_dims or cand[c] >= dims_[c];
                }

            if (out_of_dims and not wrap)
                continue;

            if (wrap and out_of_dims)
                for(size_t c = 0; c < D; ++c)
                    cand[c] = cand[c] % dims[c];

            result.push_back(cand);
        }

        return result;
    }

    Int point_id(const GridPoint& v) const
    {
        Int result = 0;
        for(int d = 0; d < dim(); ++d) {
            result += v[i] * strides_[i];
        }
        return result;
    }

    static GridPoint add_points(const GridPoint& x, const GridPoint& y) const
    {
        GridPoint z = y;
        for(size_t i = 0; i < dim(); ++i)
            z[i] += y[i];
        return z;
    }


    SimplexVec fr_simplices(const GridPoint& v, int d) const
    {
        SimplexVec result;

        for(const auto& deltas : displacements<Int, D>(d)) {

            GridSimplex::IdxVector vertices(d + 1, 0);

            for(size_t i = 0; i < d + 1; ++i)
                vertices[i] = point_id(add_points(deltas[i], v));

            result.emplace_back(vertices);
        }

        return result;
    }

private:
    GridPoint dims_;
    GridPoint strides_;
    bool c_order { true };
    bool wrap_;
    std::array<Int, D+1> powers_of_two_;
};


void read_filtration(std::string fname_in, SparseMatrix& r_matrix, Filtration& fil, bool prepare_for_clearing);

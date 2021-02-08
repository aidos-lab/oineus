#pragma once

#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
#include <numeric>
#include <functional>

#include <oineus/filtration.h>

namespace oineus {

// this function is auto-generated by scripts/freudenthal_cube.py
template<typename Int, size_t D>
std::vector<std::vector<std::vector<Int>>> fr_displacements(size_t dim)
{
    size_t cube_dim = D;

    switch (cube_dim) {
        case 1 :
            switch (dim) {
                case 0 : return {
                    { { 0 } } };
                    break;
                case 1 : return {
                    { { 0 }, { 1 } } };
                    break;
            }
            break;
        case 2 :
            switch (dim) {
                case 0 : return {
                    { { 0, 0 } } };
                    break;
                case 1 : return {
                    { { 0, 0 }, { 0, 1 } },
                    { { 0, 0 }, { 1, 1 } },
                    { { 0, 0 }, { 1, 0 } } };
                    break;
                case 2 : return {
                    { { 0, 0 }, { 1, 0 }, { 1, 1 } },
                    { { 0, 0 }, { 0, 1 }, { 1, 1 } } };
                    break;
            }
            break;
        case 3 :
            switch (dim) {
                case 0 : return {
                    { { 0, 0, 0 } } };
                    break;
                case 1 : return {
                    { { 0, 0, 0 }, { 0, 1, 0 } },
                    { { 0, 0, 0 }, { 1, 1, 0 } },
                    { { 0, 0, 0 }, { 0, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 0, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 } },
                    { { 0, 0, 0 }, { 1, 0, 1 } } };
                    break;
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
                    break;
                case 3 : return {
                    { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 } },
                    { { 0, 0, 0 }, { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 } } };
                    break;
            }
            break;
    }

    throw std::runtime_error("Unknown dimensions");
}


template<typename Int_, typename Real_, size_t D>
class Grid {
public:
    using Int = Int_;
    using Real = Real_;
    using GridSimplex = Simplex<Int, Real>;
    using GridPoint = std::array<Int, D>;
    using PointVec = std::vector<GridPoint>;
    using SimplexVec = std::vector<GridSimplex>;
    using IdxVector = typename GridSimplex::IdxVector;

    using GridFiltration = Filtration<Int, Real>;

    static constexpr size_t dim { D };

    Grid() = default;

    Grid(const GridPoint& _dims, bool _wrap, Real* _data) : dims_(_dims), c_order_(true), wrap_(_wrap), data_(_data)
    {
        if (dims_.size() != dim)
            throw std::runtime_error("Dims size must be equal D");

        if (wrap_ and std::any_of(dims_.begin(), dims_.end(), [](Int x) { return x < 3; }))
            throw std::runtime_error("Must have at least 3 cells in each dimension for wrap=true");

        if (not std::all_of(dims_.begin(), dims_.end(), [](Int x) { return x > 0; }))
            throw std::runtime_error("Dims must be positive");

        if (c_order_) {
            strides_[D-1] = 1;

            for(int d = static_cast<int>(D) - 2; d >= 0; --d)
                strides_[d] = dims_[d + 1] * strides_[ d + 1];
        } else
            throw std::runtime_error("Fortran order not implemented");
    }

    Int size() const
    {
        return std::accumulate(dims_.cbegin(), dims_.cend(), Int(1), std::multiplies<Int>());
    }

    Int point_to_id(const GridPoint& v) const
    {
        assert(c_order_);
        return std::inner_product(v.begin(), v.end(), strides_.begin(), 0);
    }

    GridPoint wrap_point(const GridPoint& v) const
    {
        assert(wrap_);
        GridPoint result = v;
        for(size_t i = 0; i < dim; ++i)
            result[i] %= dims_[i];
        return result;
    }

    bool point_in_bounds(const GridPoint& v) const
    {
        for(size_t i = 0; i < dim; ++i)
            if (v[i] >= dims_[i] or v[i] < 0)
                return false;
        return true;
    }

    GridPoint id_to_point(Int i) const
    {
        GridPoint result;

        assert(0 <= i and i < size());

        if (c_order_)
            for(size_t d = 0; d < dim; ++d) {
                result[d] = i / strides_[d];
                i = i % strides_[d];
            }
        else
            throw std::runtime_error("Fortran order not supported");
        return result;
    }


    static GridPoint add_points(const GridPoint& x, const std::vector<Int>& y)
    {
        if (y.size() != x.size())
            throw std::runtime_error("Dimension mismatch");

        GridPoint z = x;
        for(size_t i = 0; i < dim; ++i)
            z[i] += y[i];

        return z;
    }


    SimplexVec freudenthal_simplices(size_t d, bool negate) const
    {
        SimplexVec result;
        result.reserve( size() * fr_displacements<Int, D>(d).size() );

        for(Int i = 0; i < size(); ++i) {
            GridPoint v = id_to_point(i);
            for(const auto& s : freudenthal_simplices_from_vertex(v, d, negate)) {
                result.push_back(s);
            }
        }

        return result;
    }

    GridFiltration freudenthal_filtration(size_t top_d, bool negate) const
    {
        if (top_d > dim)
            throw std::runtime_error("bad dimension");

        std::vector<SimplexVec> dim_to_simplices;

        for(size_t d = 0; d <= top_d; ++d)
            dim_to_simplices.emplace_back(freudenthal_simplices(d, negate));

        return GridFiltration(std::move(dim_to_simplices), negate);
    }


    Real value_at_vertex(Int vertex) const
    {
        assert(vertex >=0 and vertex < size());
        return *(data_ + vertex);
    }


    Real simplex_value(const IdxVector& vertices, bool negate) const
    {
        Real result = negate ? std::numeric_limits<Real>::max() : std::numeric_limits<Real>::lowest();

        for(Int v : vertices) {
            Real value = value_at_vertex(v);
            if (negate)
                result = std::min(result, value);
            else
                result = std::max(result, value);
        }

        return result;
    }


    SimplexVec freudenthal_simplices_from_vertex(const GridPoint& v, size_t d, bool negate) const
    {
        SimplexVec result;

        for(const auto& deltas : fr_displacements<Int, D>(d)) {

            IdxVector v_ids(d + 1, 0);

            bool valid_simplex = true;

            for(size_t i = 0; i < d + 1; ++i) {
                GridPoint u = add_points(v, deltas[i]);
                if (wrap_)
                    v_ids[i] = point_to_id(wrap_point(u));
                else if (not point_in_bounds(u)) {
                    valid_simplex = false;
                    break;
                } else {
                    v_ids[i] = point_to_id(u);
                }
            }

            if (valid_simplex)
                result.emplace_back(v_ids, simplex_value(v_ids, negate));
        }
        return result;
    }

private:
    GridPoint dims_;
    GridPoint strides_;
    bool c_order_ {true};
    bool wrap_ {false};
    Real* data_ {nullptr};
}; // class Grid

} // namespace oineus
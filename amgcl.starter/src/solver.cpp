
#include <iostream>
#include <string>
#include <vector>
#include <random>

#include "cxxopts.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#ifndef REAL
#define REAL double
#endif

#if defined(SOLVER_BACKEND_VEXCL)
#  include <amgcl/value_type/static_matrix.hpp>
#  include <amgcl/adapter/block_matrix.hpp>
#  include <amgcl/backend/vexcl.hpp>
#  include <amgcl/backend/vexcl_static_matrix.hpp>
   typedef amgcl::backend::vexcl<REAL> Backend;
#elif defined(SOLVER_BACKEND_VIENNACL)
#  include <amgcl/backend/viennacl.hpp>
   typedef amgcl::backend::viennacl< viennacl::compressed_matrix<REAL> > Backend;
#elif defined(SOLVER_BACKEND_CUDA)
#  include <amgcl/backend/cuda.hpp>
#  include <amgcl/relaxation/cusparse_ilu0.hpp>
   typedef amgcl::backend::cuda<REAL> Backend;
#elif defined(SOLVER_BACKEND_EIGEN)
#  include <amgcl/backend/eigen.hpp>
   typedef amgcl::backend::eigen<REAL> Backend;
#elif defined(SOLVER_BACKEND_BLAZE)
#  include <amgcl/backend/blaze.hpp>
   typedef amgcl::backend::blaze<REAL> Backend;
#else
#  ifndef SOLVER_BACKEND_BUILTIN
#    define SOLVER_BACKEND_BUILTIN
#  endif
#  include <amgcl/backend/builtin.hpp>
#  include <amgcl/value_type/static_matrix.hpp>
#  include <amgcl/adapter/block_matrix.hpp>
   typedef amgcl::backend::builtin<REAL> Backend;
#endif

#include <amgcl/relaxation/runtime.hpp>
#include <amgcl/coarsening/runtime.hpp>
#include <amgcl/coarsening/rigid_body_modes.hpp>
#include <amgcl/solver/runtime.hpp>
#include <amgcl/preconditioner/runtime.hpp>
#include <amgcl/make_solver.hpp>
#include <amgcl/amg.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <amgcl/adapter/reorder.hpp>
#include <amgcl/io/mm.hpp>

#include <amgcl/profiler.hpp>

#ifndef AMGCL_BLOCK_SIZES
#  define AMGCL_BLOCK_SIZES (3)(4)(6)(10)
#endif

namespace amgcl { profiler<> prof; }
using amgcl::prof;
using amgcl::precondition;

#ifdef SOLVER_BACKEND_BUILTIN
//---------------------------------------------------------------------------
template <int B>
std::tuple<size_t, REAL> block_solve(
        const boost::property_tree::ptree &prm,
        size_t rows,
        std::vector<ptrdiff_t> const &ptr,
        std::vector<ptrdiff_t> const &col,
        std::vector<REAL>    const &val,
        std::vector<REAL>    const &rhs,
        std::vector<REAL>          &x,
        bool reorder
        )
{
    typedef amgcl::static_matrix<REAL, B, B> value_type;
    typedef amgcl::static_matrix<REAL, B, 1> rhs_type;
    typedef amgcl::backend::builtin<value_type> BBackend;

    typedef amgcl::make_solver<
        amgcl::runtime::preconditioner<BBackend>,
        amgcl::runtime::solver::wrapper<BBackend>
        > Solver;

    auto As = std::tie(rows, ptr, col, val);
    auto Ab = amgcl::adapter::block_matrix<value_type>(As);

    std::tuple<size_t, REAL> info;

    if (reorder) {
        prof.tic("reorder");
        amgcl::adapter::reorder<> perm(Ab);
        prof.toc("reorder");

        prof.tic("setup");
        Solver solve(perm(Ab), prm);
        prof.toc("setup");

        std::cout << solve << std::endl;

        rhs_type const * fptr = reinterpret_cast<rhs_type const *>(&rhs[0]);
        rhs_type       * xptr = reinterpret_cast<rhs_type       *>(&x[0]);

        amgcl::backend::numa_vector<rhs_type> F(perm(amgcl::make_iterator_range(fptr, fptr + rows/B)));
        amgcl::backend::numa_vector<rhs_type> X(perm(amgcl::make_iterator_range(xptr, xptr + rows/B)));

        prof.tic("solve");
        info = solve(F, X);
        prof.toc("solve");

        perm.inverse(X, xptr);
    } else {
        prof.tic("setup");
        Solver solve(Ab, prm);
        prof.toc("setup");

        std::cout << solve << std::endl;

        rhs_type const * fptr = reinterpret_cast<rhs_type const *>(&rhs[0]);
        rhs_type       * xptr = reinterpret_cast<rhs_type       *>(&x[0]);

        amgcl::backend::numa_vector<rhs_type> F(fptr, fptr + rows/B);
        amgcl::backend::numa_vector<rhs_type> X(xptr, xptr + rows/B);

        prof.tic("solve");
        info = solve(F, X);
        prof.toc("solve");

        std::copy(X.data(), X.data() + X.size(), xptr);
    }

    return info;
}
#endif

#ifdef SOLVER_BACKEND_VEXCL
//---------------------------------------------------------------------------
template <int B>
std::tuple<size_t, REAL> block_solve(
        const boost::property_tree::ptree &prm,
        size_t rows,
        std::vector<ptrdiff_t> const &ptr,
        std::vector<ptrdiff_t> const &col,
        std::vector<REAL>    const &val,
        std::vector<REAL>    const &rhs,
        std::vector<REAL>          &x,
        bool reorder
        )
{
    typedef amgcl::static_matrix<REAL, B, B> value_type;
    typedef amgcl::static_matrix<REAL, B, 1> rhs_type;
    typedef amgcl::backend::vexcl<value_type> BBackend;

    typedef amgcl::make_solver<
        amgcl::runtime::preconditioner<BBackend>,
        amgcl::runtime::solver::wrapper<BBackend>
        > Solver;

    typename BBackend::params bprm;

    vex::Context ctx(vex::Filter::Env);
    std::cout << ctx << std::endl;
    bprm.q = ctx;
    bprm.fast_matrix_setup = prm.get("fast", true);

    vex::scoped_program_header header(ctx,
            amgcl::backend::vexcl_static_matrix_declaration<REAL,B>());

    auto As = std::tie(rows, ptr, col, val);
    auto Ab = amgcl::adapter::block_matrix<value_type>(As);

    std::tuple<size_t, REAL> info;

    if (reorder) {
        prof.tic("reorder");
        amgcl::adapter::reorder<> perm(Ab);
        prof.toc("reorder");

        prof.tic("setup");
        Solver solve(perm(Ab), prm, bprm);
        prof.toc("setup");

        std::cout << solve << std::endl;

        rhs_type const * fptr = reinterpret_cast<rhs_type const *>(&rhs[0]);
        rhs_type       * xptr = reinterpret_cast<rhs_type       *>(&x[0]);

        std::vector<rhs_type> tmp(rows / B);

        perm.forward(amgcl::make_iterator_range(fptr, fptr + rows/B), tmp);
        vex::vector<rhs_type> f_b(ctx, tmp);

        perm.forward(amgcl::make_iterator_range(xptr, xptr + rows/B), tmp);
        vex::vector<rhs_type> x_b(ctx, tmp);

        prof.tic("solve");
        info = solve(f_b, x_b);
        prof.toc("solve");

        vex::copy(x_b, tmp);
        perm.inverse(tmp, xptr);
    } else {
        prof.tic("setup");
        Solver solve(Ab, prm, bprm);
        prof.toc("setup");

        std::cout << solve << std::endl;

        rhs_type const * fptr = reinterpret_cast<rhs_type const *>(&rhs[0]);
        rhs_type       * xptr = reinterpret_cast<rhs_type       *>(&x[0]);

        vex::vector<rhs_type> f_b(ctx, rows/B, fptr);
        vex::vector<rhs_type> x_b(ctx, rows/B, xptr);

        prof.tic("solve");
        info = solve(f_b, x_b);
        prof.toc("solve");

        vex::copy(x_b.begin(), x_b.end(), xptr);
    }

    return info;
}
#endif

//---------------------------------------------------------------------------
std::tuple<size_t, REAL> scalar_solve(
        const boost::property_tree::ptree &prm,
        size_t rows,
        std::vector<ptrdiff_t> const &ptr,
        std::vector<ptrdiff_t> const &col,
        std::vector<REAL>    const &val,
        std::vector<REAL>    const &rhs,
        std::vector<REAL>          &x,
        bool reorder
        )
{
    Backend::params bprm;

#if defined(SOLVER_BACKEND_VEXCL)
    // https://vexcl.readthedocs.io/en/latest/initialize.html
    //vex::Context ctx(vex::Filter::Env);
    vex::Context ctx(vex::Filter::Position(2));
    std::cout << ctx << std::endl;
    bprm.q = ctx;
    //bprm.q = {ctx.queue(2)}; // RadeonVII
#elif defined(SOLVER_BACKEND_VIENNACL)
    std::cout
        << viennacl::ocl::current_device().name()
        << " (" << viennacl::ocl::current_device().vendor() << ")\n\n";
#elif defined(SOLVER_BACKEND_CUDA)
    cusparseCreate(&bprm.cusparse_handle);
    {
        int dev;
        cudaGetDevice(&dev);

        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, dev);
        std::cout << prop.name << std::endl << std::endl;
    }
#endif

    typedef amgcl::make_solver<
        amgcl::runtime::preconditioner<Backend>,
        amgcl::runtime::solver::wrapper<Backend>
        > Solver;

    std::tuple<size_t, REAL> info;

    if (reorder) {
        prof.tic("reorder");
        amgcl::adapter::reorder<> perm(std::tie(rows, ptr, col, val));
        prof.toc("reorder");

        prof.tic("setup");
        Solver solve(perm(std::tie(rows, ptr, col, val)), prm, bprm);
        prof.toc("setup");

        std::cout << solve << std::endl;

        std::vector<REAL> tmp(rows);

        perm.forward(rhs, tmp);
        auto f_b = Backend::copy_vector(tmp, bprm);

        perm.forward(x, tmp);
        auto x_b = Backend::copy_vector(tmp, bprm);

        prof.tic("solve");
        info = solve(*f_b, *x_b);
        prof.toc("solve");

#if defined(SOLVER_BACKEND_VEXCL)
        vex::copy(*x_b, tmp);
#elif defined(SOLVER_BACKEND_VIENNACL)
        viennacl::fast_copy(*x_b, tmp);
#elif defined(SOLVER_BACKEND_CUDA)
        thrust::copy(x_b->begin(), x_b->end(), tmp.begin());
#else
        std::copy(&(*x_b)[0], &(*x_b)[0] + rows, &tmp[0]);
#endif

        perm.inverse(tmp, x);
    } else {
        prof.tic("setup");
        Solver solve(std::tie(rows, ptr, col, val), prm, bprm);
        prof.toc("setup");

        std::cout << solve << std::endl;

        auto f_b = Backend::copy_vector(rhs, bprm);
        auto x_b = Backend::copy_vector(x,   bprm);

        prof.tic("solve");
        info = solve(*f_b, *x_b);
        prof.toc("solve");

#if defined(SOLVER_BACKEND_VEXCL)
        vex::copy(*x_b, x);
#elif defined(SOLVER_BACKEND_VIENNACL)
        viennacl::fast_copy(*x_b, x);
#elif defined(SOLVER_BACKEND_CUDA)
        thrust::copy(x_b->begin(), x_b->end(), x.begin());
#else
        std::copy(&(*x_b)[0], &(*x_b)[0] + rows, &x[0]);
#endif
    }

    return info;
}

#define AMGCL_CALL_BLOCK_SOLVER(z, data, B)                                    \
  case B:                                                                      \
    return block_solve<B>(prm, rows, ptr, col, val, rhs, x, reorder);

//---------------------------------------------------------------------------
std::tuple<size_t, REAL> solve(
        const boost::property_tree::ptree &prm,
        size_t rows,
        std::vector<ptrdiff_t> const &ptr,
        std::vector<ptrdiff_t> const &col,
        std::vector<REAL>    const &val,
        std::vector<REAL>    const &rhs,
        std::vector<REAL>          &x,
        int block_size,
        bool reorder
        )
{
    switch (block_size) {
        case 1:
            return scalar_solve(prm, rows, ptr, col, val, rhs, x, reorder);
#if defined(SOLVER_BACKEND_BUILTIN) || defined(SOLVER_BACKEND_VEXCL)
        BOOST_PP_SEQ_FOR_EACH(AMGCL_CALL_BLOCK_SOLVER, ~, AMGCL_BLOCK_SIZES)
#endif
        default:
            precondition(false, "Unsupported block size");
            return std::make_tuple(0, 0.0);
    }
}

//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    //namespace po = boost::program_options;
    namespace io = amgcl::io;

    using amgcl::prof;
    using std::vector;
    using std::string;

    //namespace po = boost::program_options;
    namespace io = amgcl::io;

    cxxopts::Options options("amgcl_block_matrices", "check AMG for different block sizes");
    options
        .allow_unrecognised_options()
        .add_options()
        ("A,matrix", "System matrix in the MatrixMarket format.",
         cxxopts::value<std::string>())
        ("b,block-size", "The block size of the system matrix. ",
         cxxopts::value<int>()->default_value("1"))

        (
         "p,prm",
         "Parameters specified as name=value pairs. "
         "May be provided multiple times. Examples:\n"
         "  -p solver.tol=1e-3\n"
         "  -p precond.coarse_enough=300",
         cxxopts::value<vector<string>>())

        ("h,help", "Print help")
        ;

    auto vm = options.parse(argc, argv);

    if (vm.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; ++i) {
        if (i) std::cout << " ";
        std::cout << argv[i];
    }
    std::cout << std::endl;

    boost::property_tree::ptree prm;
    if (vm.count("prm-file")) {
        read_json(vm["prm-file"].as<string>(), prm);
    }

    if (vm.count("prm")) {
        for(const string &v : vm["prm"].as<vector<string>>()) {
            amgcl::put(prm, v);
        }
    }

    // see [Question: How to retrieve the initial residual from the linear solvers?](https://github.com/ddemidov/amgcl/issues/89)
    auto set_prm_defaults = [&prm](string const& param, auto const& default_val) {
        prm.put(param, prm.get(param, default_val));
    };
    set_prm_defaults("solver.type",     "cg"); // "bicgstab"
    set_prm_defaults("solver.tol",      1e-4);
    set_prm_defaults("solver.maxiter",  5000);
    set_prm_defaults("solver.verbose",  false);
    set_prm_defaults("precond.coarsening.type",  "smoothed_aggregation");
    set_prm_defaults("precond.relax.type",       "damped_jacobi"); // ilu0
    set_prm_defaults("precond.allow_rebuild", true);

    size_t block_size = 1;

    if (vm.count("block-size")) {
        block_size = vm["block-size"].as<int>();
    }

    size_t rows, nv = 0;
    vector<ptrdiff_t> ptr, col;
    vector<REAL> val;

    if (vm.count("matrix")) {
        string Afile  = vm["matrix"].as<string>();
        auto [r, cols] = io::mm_reader(Afile)(ptr, col, val);

        rows = r;
        std::cout << "rows: " << rows << std::endl;
        std::cout << "cols: " << cols << std::endl;
    }

    vector<REAL> rhs(rows, 0.0), x(rows, 0.0);

    // Set RHS := Ax where x = 1
    for (size_t i = 0; i < rows; ++i) {
        REAL s = 0;
        for (ptrdiff_t j = ptr[i], e = ptr[i+1]; j < e; ++j) {
            s += val[j];
        }
        rhs[i] = s;
    }

    auto [iters, error] = solve(
            prm, rows, ptr, col, val, rhs, x,
            block_size, false /* reorder */);

    std::cout << "Iterations: " << iters << std::endl
              << "Error:      " << error << std::endl
              << prof << std::endl;
}

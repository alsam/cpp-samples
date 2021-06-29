#include "cxxopts.hpp"

// see https://www.gitmemory.com/issue/ddemidov/amgcl/162/668662041 for inspiration

#include <amgcl/io/mm.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <amgcl/amg.hpp>

#if defined(SOLVER_BACKEND_CUDA)
#  include <amgcl/adapter/eigen.hpp>
#  include <amgcl/backend/cuda.hpp>
#  include <amgcl/relaxation/cusparse_ilu0.hpp>
   using Backend = amgcl::backend::cuda<double>;

#else
   using Backend = amgcl::backend::builtin<double>;
#endif

#include <amgcl/make_solver.hpp>
#include <amgcl/make_block_solver.hpp>
#include <amgcl/solver/bicgstab.hpp>
#include <amgcl/solver/gmres.hpp>
#include <amgcl/solver/runtime.hpp>
#include <amgcl/preconditioner/runtime.hpp>
#include <amgcl/amg.hpp>
#include <amgcl/coarsening/smoothed_aggregation.hpp>
#include <amgcl/coarsening/plain_aggregates.hpp>
#include <amgcl/coarsening/ruge_stuben.hpp>
#include <amgcl/coarsening/aggregation.hpp>
#include <amgcl/relaxation/spai0.hpp>
#include <amgcl/relaxation/ilu0.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// ./amgcl-block-matrices -A ../data/B_test.mtx  -b 3

#include <amgcl/profiler.hpp>


namespace amgcl { profiler<> prof; }
using amgcl::prof;
using amgcl::precondition;

//---------------------------------------------------------------------------
std::tuple<size_t, double> scalar_solve(
        const boost::property_tree::ptree &prm,
        size_t rows,
        std::vector<ptrdiff_t> const &ptr,
        std::vector<ptrdiff_t> const &col,
        std::vector<double>    const &val,
        std::vector<double>    const &rhs,
        std::vector<double>          &x,
        bool reorder
        )
{
    Backend::params bprm;
    // We use the tuple of CRS arrays to represent the system matrix.
    // Note that std::tie creates a tuple of references, so no data is actually
    // copied here:
    auto A = std::tie(rows, ptr, col, val);

#if defined(SOLVER_BACKEND_VEXCL)
    vex::Context ctx(vex::Filter::Env);
    std::cout << ctx << std::endl;
    bprm.q = ctx;
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

    using Solver1 = amgcl::make_solver<
    amgcl::runtime::preconditioner<Backend>,
    amgcl::runtime::solver::wrapper<Backend>>;

    //using Solver2 = amgcl::make_block_solver<
    using Solver2 = amgcl::make_solver<
    // Use AMG as preconditioner:
    amgcl::amg<
        Backend,
        amgcl::coarsening::smoothed_aggregation,
        amgcl::relaxation::spai0
        >,
    // And BiCGStab as iterative solver:
    amgcl::solver::bicgstab<Backend>>;

    using Solver3 = amgcl::make_solver<
    // Use AMG as preconditioner:
    amgcl::amg<
        Backend,
        amgcl::coarsening::ruge_stuben,
        amgcl::relaxation::spai0
        //amgcl::relaxation::ilu0
        >,
    // And GMRES as iterative solver:
    amgcl::solver::gmres<Backend>>;

    try {
        Solver2::params prm;
        Solver2::backend_params bprm;
        // prm.precond.direct_coarse = false;
        prm.precond.direct_coarse = true;
        prm.solver.maxiter = 1200;
        prm.solver.verbose = true;
        //prm.precond.coarsening ...

#if defined(SOLVER_BACKEND_VEXCL)
        vex::Context ctx(vex::Filter::Env);
        std::cout << ctx << std::endl;
        bprm.q = ctx;
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

        Solver2 solve( A, prm, bprm );


        auto f_b = Backend::copy_vector(rhs, bprm);
        auto x_b = Backend::copy_vector(x,   bprm);

        //auto [iters, error] = solve(A, rhs, x);
        auto [iters, error] = solve(A, *f_b, *x_b);

#if defined(SOLVER_BACKEND_VEXCL)
        vex::copy(*x_b, x);
#elif defined(SOLVER_BACKEND_VIENNACL)
        viennacl::fast_copy(*x_b, x);
#elif defined(SOLVER_BACKEND_CUDA)
        thrust::copy(x_b->begin(), x_b->end(), x.begin());
#else
        std::copy(&(*x_b)[0], &(*x_b)[0] + rows, &x[0]);
#endif

        // Output the number of iterations, the relative error:
        std::cout << "Iters: " << iters << std::endl
                  << "Error: " << error << std::endl;
        // std::cout << "x = [ ";
        // for (auto el : x) {
        //     std::cout << el << ", ";
        // }
        // std::cout << "]\n";
        return {iters, error};
    } catch(std::runtime_error &e) {
        std::cout << "caught exception: " << e.what() << std::endl;
    }

    return {0, 0.0};
}

int main(int argc, char *argv[])
{
    //namespace po = boost::program_options;
    namespace io = amgcl::io;

    using std::vector;
    using std::string;

    cxxopts::Options options("amgcl_block_matrices", "check AMG for different block sizes");
    options
        .allow_unrecognised_options()
        .add_options()
        ("A,matrix", "System matrix in the MatrixMarket format.",
         cxxopts::value<std::string>())
        ("b,block-size", "The block size of the system matrix. ",
         cxxopts::value<int>()->default_value("1"))
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

    size_t n, block_size = 1;
    vector<ptrdiff_t> ptr, col;
    vector<double> val;

    if (vm.count("block-size")) {
        block_size = vm["block-size"].as<int>();
    }

    if (vm.count("matrix")) {
        string Afile  = vm["matrix"].as<string>();
        auto [rows, cols] = io::mm_reader(Afile)(ptr, col, val);

        std::cout << "rows: " << rows << std::endl;
        std::cout << "cols: " << cols << std::endl;
        n = rows;
    }

    // We use the tuple of CRS arrays to represent the system matrix.
    // Note that std::tie creates a tuple of references, so no data is actually
    // copied here:
    auto A = std::tie(n, ptr, col, val);
    std::vector<double> rhs (n, 0.0);
    std::vector<double> x   (n, 0.0);

    // Set RHS := Ax where x = 1
    for (size_t i = 0; i < n; ++i) {
        double s = 0;
        for (ptrdiff_t j = ptr[i], e = ptr[i+1]; j < e; ++j) {
            s += val[j];
        }
        rhs[i] = s;
    }

    if (block_size == 1) { // scalar case
        scalar_solve(prm, n, ptr,col, val, rhs, x, false);
    } else {
        // Compose the solver type
        using dmat_type = amgcl::static_matrix<double, 3, 3>;
        using dvec_type = amgcl::static_matrix<double, 3, 1>;
        using SBackend = amgcl::backend::builtin<double>;    // the outer iterative solver backend
        using PBackend = amgcl::backend::builtin<float>;     // the PSolver backend
        using UBackend = amgcl::backend::builtin<dmat_type>; // the USolver backend
        using BlockSolver = amgcl::make_block_solver<
                // preconditioner
                amgcl::amg<
                    UBackend,
                    amgcl::coarsening::aggregation,
                    amgcl::relaxation::ilu0
                    >,
                // solver
                amgcl::solver::gmres<UBackend>>;
        using Solver = amgcl::make_solver<
                // preconditioner
                amgcl::amg<
                    UBackend,
                    amgcl::coarsening::aggregation,
                    amgcl::relaxation::ilu0
                    >,
                // solver
                amgcl::solver::gmres<UBackend>>;
        try {
            // see `tutorial/3.CoupCons3D/coupcons3d.cpp`
#if 0
            BlockSolver::params prm;
            prm.solver.maxiter = 1200;
            prm.solver.verbose = true;
            BlockSolver::backend_params bprm;
            prm.precond.direct_coarse = false;
            auto Ab = amgcl::adapter::block_matrix<dmat_type>(A);
            auto rhsb = reinterpret_cast<dvec_type*>(rhs.data());
            auto xb = reinterpret_cast<dvec_type*>(x.data());
            auto RHS = amgcl::make_iterator_range(rhsb, rhsb + n / 3);
            auto X = amgcl::make_iterator_range(xb, xb + n / block_size);
            Solver solve0( Ab, prm, bprm );
            auto [iters0, error0] = solve0(Ab, RHS, X);
            std::cout << "Iters0: " << iters0 << std::endl
                      << "Error0: " << error0 << std::endl;

            BlockSolver solve( A, prm, bprm );
            auto [iters, error] = solve(A, rhs, x);

            std::cout << "Iters: " << iters << std::endl
                      << "Error: " << error << std::endl;
            // std::cout << "x = [ ";
            // for (auto el : x) {
            //     std::cout << el << ", ";
            // }
            // std::cout << "]\n";
#endif
        } catch(std::runtime_error &e) {
            std::cout << "caught exception: " << e.what() << std::endl;
        }
    }
}

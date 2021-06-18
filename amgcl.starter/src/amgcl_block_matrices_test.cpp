#include <amgcl/io/mm.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <amgcl/amg.hpp>
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
// #include <format> // not yet supported

// ./amgcl-block-matrices -A ../data/B_test.mtx  -b 3

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    namespace io = amgcl::io;

    using std::vector;
    using std::string;

    po::options_description desc("Options");

    desc.add_options()
        ("help,h", "Show this help.")
        ("prm-file,P",
         po::value<string>(),
         "Parameter file in json format. "
        )
        (
         "prm,p",
         po::value< vector<string> >()->multitoken(),
         "Parameters specified as name=value pairs. "
         "May be provided multiple times. Examples:\n"
         "  -p solver.tol=1e-3\n"
         "  -p precond.coarse_enough=300"
        )
        ("matrix,A",
         po::value<string>(),
         "System matrix in the MatrixMarket format. "
         "When not specified, solves a Poisson problem in 3D unit cube. "
        )
        (
         "block-size,b",
         po::value<int>()->default_value(1),
         "The block size of the system matrix. "
         "When specified, the system matrix is assumed to have block-wise structure. "
         "This usually is the case for problems in elasticity, structural mechanics, "
         "for coupled systems of PDE (such as Navier-Stokes equations), etc. "
        )
        ;


    po::positional_options_description p;
    p.add("prm", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
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
        using Backend = amgcl::backend::builtin<double>;

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
            //prm.precond.coarsening ...
            Solver2 solve( A, prm, bprm );
            auto [iters, error] = solve(A, rhs, x);

            // Output the number of iterations, the relative error:
            std::cout << "Iters: " << iters << std::endl
                      << "Error: " << error << std::endl;
            // std::cout << "x = [ ";
            // for (auto el : x) {
            //     std::cout << el << ", ";
            // }
            // std::cout << "]\n";
        } catch(std::runtime_error &e) {
            std::cout << "caught exception: " << e.what() << std::endl;
        }
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
            BlockSolver::params prm;
            BlockSolver::backend_params bprm;
            prm.precond.direct_coarse = false;
            auto Ab = amgcl::adapter::block_matrix<dmat_type>(A);
            auto rhsb = reinterpret_cast<dvec_type*>(rhs.data());
            auto xb = reinterpret_cast<dvec_type*>(x.data());
            auto RHS = amgcl::make_iterator_range(rhsb, rhsb + n / 3);
            auto X = amgcl::make_iterator_range(xb, xb + n / 3);
            Solver solve0( Ab, prm, bprm );
            auto [iters0, error0] = solve0(Ab, RHS, X);
            std::cout << "Iters0: " << iters0 << std::endl
                      << "Error0: " << error0 << std::endl;

            BlockSolver solve( A, prm, bprm );
            auto [iters, error] = solve(A, rhs, x);

            std::cout << "Iters: " << iters << std::endl
                      << "Error: " << error << std::endl;
            std::cout << "x = [ ";
            for (auto el : x) {
                std::cout << el << ", ";
            }
            std::cout << "]\n";

        } catch(std::runtime_error &e) {
            std::cout << "caught exception: " << e.what() << std::endl;
        }
    }
}

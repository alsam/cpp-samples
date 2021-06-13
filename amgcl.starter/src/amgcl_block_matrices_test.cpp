#include <amgcl/io/mm.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <amgcl/amg.hpp>
#include <amgcl/make_solver.hpp>
#include <amgcl/make_block_solver.hpp>
#include <amgcl/solver/bicgstab.hpp>
#include <amgcl/amg.hpp>
#include <amgcl/coarsening/smoothed_aggregation.hpp>
#include <amgcl/coarsening/plain_aggregates.hpp>
#include <amgcl/coarsening/ruge_stuben.hpp>
#include <amgcl/relaxation/spai0.hpp>
#include <amgcl/relaxation/ilu0.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
// #include <format> // not yet supported

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
        for(const string &v : vm["prm"].as<vector<string> >()) {
            amgcl::put(prm, v);
        }
    }

    size_t n;
    vector<ptrdiff_t> ptr, col;
    vector<double> val, rhs, x;

    if (vm.count("matrix")) {
        string Afile  = vm["matrix"].as<string>();
        auto [rows, cols] = io::mm_reader(Afile)(ptr, col, val);

        std::cout << "rows: " << rows << std::endl;
        std::cout << "cols: " << cols << std::endl;
        n = rows;
    }

    using Backend = amgcl::backend::builtin<double>;
    using Solver1 = amgcl::make_solver<
    // Use AMG as preconditioner:
    amgcl::amg<
        Backend,
        amgcl::coarsening::smoothed_aggregation,
        amgcl::relaxation::spai0
        >,
    // And BiCGStab as iterative solver:
    amgcl::solver::bicgstab<Backend>>;

    using Solver2 = amgcl::make_block_solver<
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
        amgcl::relaxation::ilu0
        >,
    // And BiCGStab as iterative solver:
    amgcl::solver::bicgstab<Backend>>;

    try {
        Solver3 solve( std::tie(n, ptr, col, val) );
    } catch(std::runtime_error &e) {
        std::cout << "caught exception: " << e.what() << std::endl;
    }


}

#include <iostream>
#include <fstream>
#include <gtest/gtest.h>

//#define exact(A) (A)
#include <CGAL/Lazy.h> // needed for CGAL::exact(FT)/CGAL::exact(Lazy_exact_nt<T>)
// [](https://github.com/lrineau/cgal/commit/68cf051563d25c7842eda5a96f5f7c796c15ea9f)
// [Compilation errors of <CGAL/Polygon_mesh_processing/measure.h>](https://github.com/CGAL/cgal/issues/2654)
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polygon_mesh_processing/measure.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Surface_mesh = CGAL::Polyhedron_3<Kernel>;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "gimme input name\n";
        return -1;
    }
    Surface_mesh surface_mesh;
    std::ifstream ifs(argv[1]);
    if (!ifs) {
        std::cout << "I/O problem with " << argv[1] << std::endl;
        return -2;
    }
    if (!read_off(ifs, surface_mesh)) {
        std::cout << "problem reading polyhedron " << argv[1] << std::endl;
        return -3;
    }

    bool isTriangleMesh = CGAL::is_triangle_mesh(surface_mesh);
    if (!isTriangleMesh) {
        std::cout << "Input geometry is not triangualted\n";
        return -5;
    }
    if (CGAL::is_closed(surface_mesh)) {
        std::cout << "-I- the surface mesh is closed\n";
    } else {
        std::cout << "-I- the surface mesh is **NOT** closed\n";
    }
    double vol = CGAL::Polygon_mesh_processing::volume(surface_mesh);
    std::cout << "vol: " << vol << std::endl;
    return 0;
}

#pragma once
#include <png++/png.hpp>
#include <tuple>

namespace cl_tutorial
{
    using Colour = std::tuple<int, int, int>;

    namespace colour_map
    {
        Colour rainbow(float x) {
            float r = (0.472-0.567*x+4.05*pow(x, 2))
                      /(1.+8.72*x-19.17*pow(x, 2)+14.1*pow(x, 3)),
                  g = 0.108932-1.22635*x+27.284*pow(x, 2)-98.577*pow(x, 3)
                      +163.3*pow(x, 4)-131.395*pow(x, 5)+40.634*pow(x, 6),
                  b = 1./(1.97+3.54*x-68.5*pow(x, 2)+243*pow(x, 3)
                      -297*pow(x, 4)+125*pow(x, 5));

            return Colour(r*255, g*255, b*255);
        }
    }

    template <typename T, typename ColourMap>
    inline void save_png(
            std::string const &filename,
            unsigned width,
            unsigned height,
            std::vector<T> const &data,
            ColourMap palette)
    {
        std::cerr << "<<< writing image to " << filename << " ... ";
        png::image<png::rgb_pixel> image(width, height);

        for (unsigned j = 0; j < height; ++j)
        {
            for (unsigned i = 0; i < width; ++i)
            {
                // TODO: use std::apply once C++17 is mainstream.
                auto c = palette(data[i + j*width]);
                image[j][i] = png::rgb_pixel(std::get<0>(c), std::get<1>(c), std::get<2>(c));
            }
        }

        image.write(filename);
        std::cerr << ">>> (ok)\n";
    }
}


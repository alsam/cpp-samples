__kernel void mandel(global float *out, float x0, float y0, float res, unsigned max_it)
{
    unsigned i = get_global_id(0),
             j = get_global_id(1);

    size_t address = i + j * get_global_size(0);

    float cx = x0 + res * i,
          cy = y0 + res * j,
          zx = 0, zy = 0, w = 0;

    unsigned k = 0;
    while (k < max_it && w < 4.0)
    {
        float u = zx*zx - zy*zy,
              v = 2*zx*zy;

        zx = u + cx;
        zy = v + cy;

        w = zx*zx + zy*zy;
        ++k;
    }

    out[address] = (k == max_it ? 1.0 : (float)k / max_it);
}

__kernel void julia(
        global float *out, float x0, float y0, float res,
        float cx, float cy, unsigned max_it)
{
    unsigned i = get_global_id(0),
             j = get_global_id(1);

    size_t address = i + j * get_global_size(0);

    float zx = x0 + res * i,
          zy = y0 + res * j,
          w = zx*zx + zy*zy;

    unsigned k = 0;
    while (k < max_it && w < 4.0)
    {
        float u = zx*zx - zy*zy,
              v = 2*zx*zy;

        zx = u + cx;
        zy = v + cy;

        w = zx*zx + zy*zy;
        ++k;
    }

    out[address] = (k == max_it ? 1.0 : (float)k / max_it);
}


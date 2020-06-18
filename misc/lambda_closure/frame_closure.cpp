#include <iostream>
#include <functional>
#include <vector>


namespace abc {


struct State {};

class FrameReplay {
public:

    using init_function_t = std::function<void(const std::vector<uint8_t*>&)>;
    using draw_function_t = std::function<void(const State&, const std::vector<uint8_t*>&)>;

    FrameReplay(uint32_t frameId,
                init_function_t initFunc = [](const std::vector<uint8_t*> &){},
                draw_function_t drawFunc = [](const State&, const std::vector<uint8_t*> &){})
        : frameId_(frameId),
          initFunc_(initFunc),
          drawFunc_(drawFunc) 
    {}

    uint32_t getFrameId() const { return frameId_; }
    void setInitFunc(init_function_t initFunc) { initFunc_ = initFunc; }
    void setDrawFunc(draw_function_t drawFunc) { drawFunc_ = drawFunc; }
    void setHeight(int h) { height_ = h; }
    void setWidth(int w) { width_ = w; }
    void init(const std::vector<uint8_t*> &buffer) const { initFunc_(buffer); }
    void draw(const State &engine, const std::vector<uint8_t*> &buffer) const { drawFunc_(engine, buffer); }

private:
    uint32_t frameId_;
    init_function_t initFunc_;
    draw_function_t drawFunc_;
    int height_;
    int width_;
};

std::vector<FrameReplay*> init()
{
    std::vector<FrameReplay*> frames;
    {
        // frame #0
        // some resources
        static int res77;
        static const char arr_0[] = "frame #0 vs";
        auto frame = new FrameReplay(0, [&](const std::vector<uint8_t*> &) { res77 = 1; },
                                        [&](const State&, const std::vector<uint8_t*> &)
                                        {
                                            std::cout << "res77: " << res77 << " ";
                                            std::cout << "arr_0: " << arr_0 << std::endl;
                                        } );
        frames.push_back(frame);
    }
    {
        // frame #1
        // some resources
        static int res77;
        static const char arr_0[] = "frame #1 vs";
        auto frame = new FrameReplay(1);
        frame->setInitFunc([&](const std::vector<uint8_t*> &) { res77 = 2; });
        frame->setDrawFunc([&](const State&, const std::vector<uint8_t*> &)
                                        {
                                            std::cout << "res77: " << res77 << " ";
                                            std::cout << "arr_0: " << arr_0 << std::endl;
                                        } );
        frames.push_back(frame);
    }

    // NVRO
    return frames;
}

}

int main()
{
    auto frames = abc::init();
    for (const auto& f : frames) {
        std::cout << "frame #" << f->getFrameId() << std::endl;
        f->init({});
        f->draw({},{});
    }

}

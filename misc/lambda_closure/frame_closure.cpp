#include <iostream>
#include <functional>
#include <vector>

struct State {};

class FrameReplay {
public:

    using init_function_t = std::function<void(const std::vector<uint8_t*>&)>;
    using draw_function_t = std::function<void(const State&, const std::vector<uint8_t*>&)>;

    FrameReplay(uint32_t frameId, init_function_t initFunc, draw_function_t drawFunc)
        : frameId_(frameId), initFunc_(initFunc), drawFunc_(drawFunc) {}

    uint32_t getFrameId() const { return frameId_; }
    void init(const std::vector<uint8_t*> &buffer) { initFunc_(buffer); }
    void draw(const State &engine, const std::vector<uint8_t*> &buffer) { drawFunc_(engine, buffer); }

private:
    uint32_t frameId_;
    init_function_t initFunc_;
    draw_function_t drawFunc_;
};

int main()
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
                                        }  );
        frames.push_back(frame);
    }
    {
        // frame #1
        // some resources
        static int res77;
        static const char arr_0[] = "frame #1 vs";
        auto frame = new FrameReplay(1, [&](const std::vector<uint8_t*> &) { res77 = 2; },
                                        [&](const State&, const std::vector<uint8_t*> &)
                                        {
                                            std::cout << "res77: " << res77 << " ";
                                            std::cout << "arr_0: " << arr_0 << std::endl;
                                        }  );
        frames.push_back(frame);
    }

    for (const auto& f : frames) {
        std::cout << "frame #" << f->getFrameId() << std::endl;
        f->init({});
        f->draw({},{});
    }
}


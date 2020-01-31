#include <cxxopts.hpp>
// The MIT License (MIT)
//
// Copyright (c) 2020 Alexander Samoilov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE


#include <thread>
#include <mutex>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct Descriptor
{
    enum class Mode
    {
        Read,
        Write,
    };

    Descriptor(const char* filename, Mode mode)
      : fd_(-1), filename_(strdup(filename)), mode_(mode)
    {
        if (mode == Mode::Read)
            fd_ = open(filename_, O_RDONLY);
        else if (mode == Mode::Write)
        {
            constexpr mode_t wmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            fd_ = open(filename_, O_WRONLY, wmode);
        }
        if (fd_ < 0)
        {
            std::ostringstream oss;
            oss << "failed to open file";
            if (filename_) oss << " '" << filename_ << '\'';
            oss << ": " << strerror(errno);
            free(filename_);
            throw std::runtime_error(oss.str());
        }
    }

    std::string readString()
    {
        std::string retVal;
        if (Mode::Read == mode_)
        {
            ssize_t bytesRead = read(fd_, buf_, bufSize_);
            if (bytesRead > 0)
            {
                retVal = buf_;
            }
        }
        return retVal;
    }

    uint64_t readULong()
    {
        uint64_t retVal = 1U;
        if (Mode::Read == mode_)
        {
            // lock read operation
            std::unique_lock<std::mutex> locker(mu_);
            ssize_t bytesRead = read(fd_, buf_, bufSize_);
            if (bytesRead > 0)
            {
                retVal = strtoumax(buf_, nullptr, 10);
                if (UINTMAX_MAX == retVal)
                {
                    retVal = 1U;
                }
            }
        }
        return retVal;
    }

    bool writeULong(uint64_t val)
    {
        bool status = false;
        if (Mode::Write == mode_)
        {
            // lock write operation
            std::unique_lock<std::mutex> locker(mu_);
            snprintf(buf_, bufSize_, "%ld", val);
            ssize_t bytesWrote = write(fd_, buf_, bufSize_);
            status = (bytesWrote > 0);
        }
        return status;
    }

    ~Descriptor()
    {
        if (fd_ != -1 && close(fd_) == -1)
        {
            // don't throw from destructors
            std::cerr << "failed to close file";
            if (filename_) std::cerr << " '" << filename_ << '\'';
            std::cerr << ": " << strerror(errno) << std::endl;
        }
        free(filename_);
    }
    operator int() const { return fd_; }

private:
    int fd_;
    char* filename_;
    Mode mode_;
    std::mutex mu_;
    static const size_t bufSize_ = 256;
    char buf_[bufSize_];
};

uint64_t getGpuFrequency()
{
    Descriptor d("/sys/class/devfreq/gpufreq/cur_freq", Descriptor::Mode::Read);
    uint64_t retVal = d.readULong();
    return retVal;
}

bool lockGpuFrequency(uint64_t freq)
{
    bool status = true;
    uint64_t curGpuFreq = getGpuFrequency();
    Descriptor maxFreq("/sys/class/devfreq/gpufreq/max_freq", Descriptor::Mode::Write);
    Descriptor minFreq("/sys/class/devfreq/gpufreq/min_freq", Descriptor::Mode::Write);
    if (freq < curGpuFreq)
    {
        status = status && minFreq.writeULong(freq);
        status = status && maxFreq.writeULong(freq);
    }
    else
    {
        status = status && maxFreq.writeULong(freq);
        status = status && minFreq.writeULong(freq);
    }
    return status;
}

int main(int argc, char** argv)
{
    cxxopts::Options options("freqlock", "locks and displays device CPU/GPU/DDR frequencies");
    options
        .allow_unrecognised_options()
        .add_options()
        ("d,display", "display current frequencies")
        ("a,available", "display available frequencies")
        ("g,gpu", "set GPU frequency", cxxopts::value<uint64_t>())
        ("h,help", "Print help")
        ;
    auto opts = options.parse(argc, argv);

    if (opts.arguments().empty() || opts.count("help"))
    {
      std::cout << options.help() << std::endl;
      std::exit(0);
    }

    if (opts.count("d"))
    {
      std::cout << "display device frequencies" << std::endl;
      std::cout << "  GPU: " << getGpuFrequency() << std::endl;
      std::exit(0);
    }

    if (opts.count("g"))
    {
      uint64_t gpu_freq = opts["g"].as<uint64_t>();
      std::cout << "set GPU frequency: " << gpu_freq << std::endl;
      bool status = lockGpuFrequency(gpu_freq);
      std::cout << std::boolalpha << "op status: " << status
                << "\ncur GPU frequency: " << getGpuFrequency() << std::endl;
    }

}

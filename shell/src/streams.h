#pragma once

#include <iostream>
#include <sstream>

namespace shell {

// Class-interface for all types of streams (system and buffered)
class IStreams {
public:
    // returns input stream
    virtual std::istream& get_in_stream() = 0;
    // returns output stream
    virtual std::ostream& get_out_stream() = 0;
    // returns error stream
    virtual std::ostream& get_err_stream() = 0;

    virtual ~IStreams() = default;
};

// This class is responsible for global streams
class StreamsGlobal : public IStreams {
public:
    std::istream& get_in_stream() override { return std::cin; }
    std::ostream& get_out_stream() override { return std::cout; }
    std::ostream& get_err_stream() override { return std::cerr; }
};

// This stream is responsible for buffered streams^ it is required by pipes.
class StreamsBuffered : public IStreams {
public:
    explicit StreamsBuffered(const std::string& input = "") : is_{input} {}

    // Move previous output to new input, delete previous input
    void exchange_in_and_out() {
        is_.swap(os_);
        std::stringstream().swap(os_);
    }

    std::istream& get_in_stream() override {return is_; }
    std::ostream& get_out_stream() override {return os_; }
    std::ostream& get_err_stream() override {return es_; }

    // Get underlying buffers:
    std::string get_in_buffer() { return is_.str(); }
    std::string get_out_buffer() { return os_.str(); }
    std::string get_err_buffer() { return es_.str(); }


private:
    std::stringstream is_;
    std::stringstream os_;
    std::stringstream es_;
};

} // namespace shell

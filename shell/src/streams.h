#pragma once

#include <iostream>
#include <sstream>

namespace shell {

class IStreams {
public:
    virtual std::istream& get_in_stream() = 0;
    virtual std::ostream& get_out_stream() = 0;
    virtual std::ostream& get_err_stream() = 0;

    virtual ~IStreams() = default;
};

class StreamsGlobal : public IStreams {
public:
    std::istream& get_in_stream() override { return std::cin; }
    std::ostream& get_out_stream() override { return std::cout; }
    std::ostream& get_err_stream() override { return std::cerr; }
};

class StreamsBuffered : public IStreams {
public:
    explicit StreamsBuffered(const std::string& input = "") : is_{input} {}

    std::istream& get_in_stream() override {return is_; }
    std::ostream& get_out_stream() override {return os_; }
    std::ostream& get_err_stream() override {return es_; }

    std::string get_out_buffer() { return os_.str(); }
    std::string get_err_buffer() { return es_.str(); }


private:
    std::istringstream is_;
    std::ostringstream os_;
    std::ostringstream es_;
};

} // namespace shell

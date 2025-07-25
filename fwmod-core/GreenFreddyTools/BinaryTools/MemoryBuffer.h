#pragma once
//Somehow the windows min and max macros are being leaked into here regardless of where I define NOMINMAX. This works as a fix for the moment
#undef min
#undef max
#include <streambuf>
#include <algorithm>
#include <numeric>
#include <limits>

//Simple wrapper around std::streambuf representing a memory buffer.
//Used by BinaryReader and BinaryWriter
struct MemoryBuffer : public std::streambuf {
    MemoryBuffer(char* begin, uint32_t sizeInBytes) {
        this->setg(begin, begin, begin + sizeInBytes); // Set get area for reading
        this->setp(begin, begin + sizeInBytes); // Set put area for writing
    }
};

//Used by BinaryReader for reading from in memory buffers.
//Source: https://gist.github.com/polyvertex/ce86fddfa28edcfb19a77f9024a5461c

// A memory stream buffer class compliant with `std::basic_streambuf`.
//
// Usage example:
//
//   std::vector<char> data;
//   // ... fill-in *data* here ...
//   xx::basic_memstreambuf<data.value_type> sbuf(&data[0], data.size());
//   std::istream in(&sbuf);
//   // ... read data from *in* ...
//
// Useful references:
// * Deriving from std::streambuf
//   https://artofcode.wordpress.com/2010/12/12/deriving-from-stdstreambuf/
// * A beginner's guide to writing a custom stream buffer (std::streambuf)
//   http://www.voidcn.com/article/p-vjnlygmc-gy.html
// * membuf.cpp
//   https://gist.github.com/mlfarrell/28ea0e7b10756042956b579781ac0dd8
// * Memory streambuf and stream
//   https://codereview.stackexchange.com/questions/138479/memory-streambuf-and-stream

class basic_memstreambuf : public std::streambuf
{
public:
    using BaseT = std::streambuf;

    using BaseT::int_type;
    using BaseT::traits_type;

public:
    basic_memstreambuf()
        : BaseT()
    { }

    explicit basic_memstreambuf(const basic_memstreambuf& rhs)
        : BaseT(rhs)
    { }

    // non-standard
    explicit basic_memstreambuf(const std::basic_string<char_type>& s)
        : BaseT()
    {
        //assert(!s.empty());
        setg(
            const_cast<char_type*>(&s.front()),
            const_cast<char_type*>(&s.front()),
            const_cast<char_type*>(&s.back()));
    }

    // non-standard
    basic_memstreambuf(const char_type* s, std::streamsize n) : BaseT()
    {
       // assert(s);
        //assert(n > 0);
        setg(
            const_cast<char_type*>(s),
            const_cast<char_type*>(s),
            const_cast<char_type*>(s + n));
    }

    // non-standard
    basic_memstreambuf(const char_type* begin, const char_type* end)
        : BaseT()
    {
        //assert(begin);
        //assert(end);
        //assert(begin < end);

        // check size
        const std::uintmax_t count = end - begin;
        constexpr std::uintmax_t maxValue = static_cast<std::uintmax_t>(std::numeric_limits<std::streamsize>::max());
        if (count > maxValue)
        {
            throw std::invalid_argument("basic_memstreambuf too big");
        }

        setg(
            const_cast<char_type*>(begin),
            const_cast<char_type*>(begin),
            const_cast<char_type*>(end));
    }

    basic_memstreambuf& operator=(const basic_memstreambuf&) = delete;


protected:
    virtual std::streamsize showmanyc() override
    {
        const auto* ptr = gptr();
        const auto* end = egptr();

        //assert(ptr <= end);

        return (ptr <= end) ? (end - ptr) : 0;
    }

    virtual int_type underflow() override
    {
        const auto* ptr = gptr();

        if (ptr >= egptr())
            return traits_type::eof();

        return traits_type::to_int_type(*ptr);
    }

    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override {
		// TODO: make it handle huge buffer size, std::streamsize is 64 bits regardless of architecture while size_t is based on architecture
        // for now it just ignores data (to_read <= 0, static_cast<std::size_t> to prevent C4244)
        if (count == 0) {
            return 0;
        }

        const char* ptr = gptr();
        const std::streamsize available = static_cast<std::streamsize>(egptr() - ptr);
        const std::streamsize to_read = std::min(count, available);

        if (to_read <= 0) {
            return traits_type::eof();
        }
        else {
            // fix C4244
            std::memcpy(s, ptr, static_cast<std::size_t>(to_read)); // Safe conversion
            gbump(static_cast<int>(to_read));
            return to_read;
        }
    }

    virtual pos_type seekoff(
        off_type off,
        std::ios_base::seekdir dir,
        std::ios_base::openmode which = std::ios_base::in) override
    {
        if (which != std::ios_base::in)
        {
            //assert(0);
            throw std::invalid_argument("basic_memstreambuf::seekoff[which]");
        }

        if (dir == std::ios_base::beg)
        {
            if (off >= 0 && off < egptr() - eback())
            {
                setg(eback(), eback() + off, egptr());
            }
            else
            {
                //assert(0);
                throw std::out_of_range("basic_memstreambuf::seekoff[beg]");
            }
        }
        else if (dir == std::ios_base::cur)
        {
            if ((off >= 0 && off <= egptr() - gptr()) ||
                (off < 0 && std::abs(off) < gptr() - eback()))
            {
                gbump((int)off);
            }
            else
            {
                //assert(0);
                throw std::out_of_range("basic_memstreambuf::seekoff[cur]");
            }
        }
        else if (dir == std::ios_base::end)
        {
            if (off <= 0 && std::abs(off) < egptr() - eback())
            {
                setg(eback(), egptr() + (int)off, egptr());
            }
            else
            {
                //assert(0);
                throw std::out_of_range("basic_memstreambuf::seekoff[end]");
            }
        }
        else
        {
            //assert(0);
            throw std::invalid_argument("basic_memstreambuf::seekoff[dir]");
        }

        return gptr() - eback();
    }

    virtual pos_type seekpos(
        pos_type pos,
        std::ios_base::openmode which = std::ios_base::in) override
    {
        if (which != std::ios_base::in)
        {
            //assert(0);
            throw std::invalid_argument("basic_memstreambuf::seekpos[which]");
        }

        if (pos < egptr() - eback())
        {
            // fix C4244
            setg(eback(), eback() + static_cast<size_t>(pos), egptr());
        }
        else
        {
            ////assert(0);
            throw std::out_of_range("memstreambuf::seekpos");
        }

        return pos;
    }

#if 0
    virtual int_type pbackfail(int_type c = traits_type::eof()) override
    {
        const auto* begin = eback();
        const auto* ptr = gptr();
        const auto gc = *(ptr - 1);

        if (ptr == begin || (c != traits_type::eof() && c != gc))
            return traits_type::eof();

        gbump(-1);

        return traits_type::to_int_type(gc);
    }
#endif
};
//
// Copyright (c) 2022 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: JSON body
//
//------------------------------------------------------------------------------

// 2024-05-15 John Wason use nlohmann/json for json parsing

#ifndef BOOST_BEAST_EXAMPLE_JSON_BODY
#define BOOST_BEAST_EXAMPLE_JSON_BODY

#include <nlohmann/json.hpp>
// #include <boost/json.hpp>
// #include <boost/json/stream_parser.hpp>
// #include <boost/json/monotonic_resource.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/buffer.hpp>

using json = nlohmann::json;

struct json_body
{
    using value_type = json;

    struct writer
    {
        using const_buffers_type = boost::asio::const_buffer;
        template <bool isRequest, class Fields>
        writer(boost::beast::http::header<isRequest, Fields> const& h, value_type const& body)
        {
            serialized_body = body.dump();
        }

        void init(boost::system::error_code& ec)
        {
            // The serializer always works, so no error can occur here.
            ec = {};
        }

        boost::optional<std::pair<const_buffers_type, bool> > get(boost::system::error_code& ec)
        {
            ec = {};
            const_buffers_type output_buffer(serialized_body.data(), serialized_body.size());
            return std::make_pair(output_buffer, false);
        }

      private:
        std::string serialized_body;
    };

    struct reader
    {
        template <bool isRequest, class Fields>
        reader(boost::beast::http::header<isRequest, Fields>& h, value_type& body) : body(body)
        {}
        void init(boost::optional<std::uint64_t> const& content_length, boost::system::error_code& ec) { ec = {}; }

        template <class ConstBufferSequence>
        std::size_t put(ConstBufferSequence const& buffers, boost::system::error_code& ec)
        {
            std::string buffer(static_cast<const char*>(buffers.data()), buffers.size());
            body = json::parse(buffer, nullptr, false, true);
            return buffers.size();
        }

        void finish(boost::system::error_code& ec)
        {
            ec = {};
            // We check manually if the json is complete.
            if (!body.is_discarded())
                body = body;
            else
                ec = boost::beast::http::error::partial_message;
        }

      private:
        value_type& body;
    };
};

#endif

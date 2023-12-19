#include <boost/asio/ssl.hpp>

namespace RobotRaconteur
{
namespace detail
{
    template <typename Stream>
    class asio_ssl_stream_threadsafe
    {
    
    public:

        typedef typename boost::remove_reference<boost::asio::ssl::stream<Stream> >::type next_layer_type;
        typedef typename next_layer_type::lowest_layer_type lowest_layer_type;
        typedef typename lowest_layer_type::executor_type executor_type;
        typedef typename next_layer_type::native_handle_type native_handle_type;
    
    protected:    
        boost::asio::ssl::stream<Stream> ssl_stream_;
        boost::asio::ssl::context& context_;
        boost::asio::strand<executor_type> strand_;

    public:

        asio_ssl_stream_threadsafe(Stream& stream, boost::asio::ssl::context& context)
            : ssl_stream_(stream, context), context_(context), strand_(boost::asio::make_strand(stream.get_executor()))
        {
        }

        // async_read_some using strand for callback
        template <typename MutableBufferSequence, typename ReadHandler>
        void async_read_some(const MutableBufferSequence& buffers, ReadHandler&& handler)
        {
            ssl_stream_.async_read_some(buffers, boost::asio::bind_executor(strand_,std::forward<ReadHandler>(handler)));
        }

        // async_write_some using strand for callback
        template <typename ConstBufferSequence, typename WriteHandler>
        void async_write_some(const ConstBufferSequence& buffers, WriteHandler&& handler)
        {
            ssl_stream_.async_write_some(buffers, boost::asio::bind_executor(strand_, std::forward<WriteHandler>(handler)));
        }

        // async_handshake using strand for callback
        template <typename HandshakeHandler>
        void async_handshake(boost::asio::ssl::stream_base::handshake_type type, HandshakeHandler&& handler)
        {
            ssl_stream_.async_handshake(type, boost::asio::bind_executor(strand_, std::forward<HandshakeHandler>(handler)));
        }

        // async_shutdown using strand for callback
        template <typename ShutdownHandler>
        void async_shutdown(ShutdownHandler&& handler)
        {
            ssl_stream_.async_shutdown(boost::asio::bind_executor(strand_, std::forward<ShutdownHandler>(handler)));
        }

        native_handle_type native_handle()
        {
            return ssl_stream_.native_handle();
        }

        lowest_layer_type& lowest_layer()
        {
            return ssl_stream_.lowest_layer();
        }

        next_layer_type& next_layer()
        {
            return ssl_stream_;
        }

        void set_verify_mode(boost::asio::ssl::verify_mode v)
        {
            ssl_stream_.set_verify_mode(v);
        }

        executor_type get_executor() noexcept
        {
            return ssl_stream_.lowest_layer().get_executor();
        }           

    };
}
}
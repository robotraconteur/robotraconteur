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
    typedef typename next_layer_type::native_handle_type native_handle_type;

#if BOOST_ASIO_VERSION >= 101200
    typedef typename lowest_layer_type::executor_type executor_type;
#else
    typedef boost::asio::io_service executor_type;
#endif
  protected:
    boost::asio::ssl::stream<Stream> ssl_stream_;
    boost::asio::ssl::context& context_;
    RR_BOOST_ASIO_STRAND2(executor_type) strand_;

    template <typename Handler, typename Executor>
    class handler_wrapper // NOLINT(bugprone-exception-escape)
    {
      public:
        typedef typename boost::remove_const<typename boost::remove_reference<Handler>::type>::type HandlerValueType;

        handler_wrapper(const Handler& handler, RR_BOOST_ASIO_NEW_API_CONST Executor& executor)
            : handler_(handler), executor_(executor)
        {}

        void operator()(const boost::system::error_code& ec, const std::size_t& bytes_transferred)
        {
            boost::asio::detail::binder2<HandlerValueType, boost::system::error_code, std::size_t> handler(
                handler_, ec, bytes_transferred);
#if BOOST_ASIO_VERSION >= 101200
            boost::asio::post(boost::asio::get_associated_executor(handler, executor_), handler);
#else
            executor_.post(handler);
#endif
        }

      private:
        HandlerValueType handler_;
#if BOOST_ASIO_VERSION >= 101200
        Executor executor_;
#else
        boost::asio::io_service& executor_;
#endif
    };

  public:
    asio_ssl_stream_threadsafe(Stream& stream, boost::asio::ssl::context& context)
        : ssl_stream_(stream, context), context_(context),
          strand_(RR_BOOST_ASIO_MAKE_STRAND(executor_type, RR_BOOST_ASIO_GET_IO_SERVICE(stream)))
    {}

    // async_read_some using strand for callback
    template <typename MutableBufferSequence, typename ReadHandler>
    void async_read_some(const MutableBufferSequence& buffers, RR_MOVE_ARG(ReadHandler) handler)
    {
        handler_wrapper<ReadHandler, executor_type> handler2(boost::ref(handler),
                                                             RR_BOOST_ASIO_GET_IO_SERVICE((*this)));
        ssl_stream_.async_read_some(buffers, RR_BOOST_ASIO_STRAND_WRAP(strand_, RR_MOVE(handler2)));
    }

    // async_write_some using strand for callback
    template <typename ConstBufferSequence, typename WriteHandler>
    void async_write_some(const ConstBufferSequence& buffers, RR_MOVE_ARG(WriteHandler) handler)
    {
        handler_wrapper<WriteHandler, executor_type> handler2(boost::ref(handler),
                                                              RR_BOOST_ASIO_GET_IO_SERVICE((*this)));
        ssl_stream_.async_write_some(buffers, RR_BOOST_ASIO_STRAND_WRAP(strand_, RR_MOVE(handler2)));
    }

    // async_handshake using strand for callback
    template <typename HandshakeHandler>
    void async_handshake(boost::asio::ssl::stream_base::handshake_type type, RR_MOVE_ARG(HandshakeHandler) handler)
    {
        ssl_stream_.async_handshake(type, RR_BOOST_ASIO_STRAND_WRAP(strand_, RR_FORWARD(HandshakeHandler, handler)));
    }

    // async_shutdown using strand for callback
    template <typename ShutdownHandler>
    void async_shutdown(RR_MOVE_ARG(ShutdownHandler) handler)
    {
        ssl_stream_.async_shutdown(RR_BOOST_ASIO_STRAND_WRAP(strand_, RR_FORWARD(ShutdownHandler, handler)));
    }

    native_handle_type native_handle() { return ssl_stream_.native_handle(); }

    lowest_layer_type& lowest_layer() { return ssl_stream_.lowest_layer(); }

    next_layer_type& next_layer() { return ssl_stream_; }

    void set_verify_mode(boost::asio::ssl::verify_mode v) { ssl_stream_.set_verify_mode(v); }

#if BOOST_ASIO_VERSION >= 101200
    executor_type get_executor() BOOST_ASIO_NOEXCEPT { return ssl_stream_.lowest_layer().get_executor(); }
#else
    boost::asio::io_service& get_io_service() { return ssl_stream_.lowest_layer().get_io_service(); }
#endif
};
} // namespace detail
} // namespace RobotRaconteur
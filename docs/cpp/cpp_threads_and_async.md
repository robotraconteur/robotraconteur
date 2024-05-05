# C++ Multithreading and Asynchronous Functions {#cpp_threading_and_async}

## Threading {#cpp_threading}

The Robot Raconteur Core library is a multithreading library, using Boost.Asio to dispatch threads to handle events. A `boost::asio::io_context` is the core of the thread pool. The thread pool starts several threads, and each of these threads calls `boost::asio::io_context::run()`. The `run()` function will block until an event needs to be handled. The event could be a network operation, a timer, a user input, etc. The `io_context` will use one of the threads to execute the handler. Reading the [Boost.Asio Overview](https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/overview.html) is highly recommended, since the event handling and threading concepts are used by the Robot Raconteur library. Robot Raconteur by default uses the ThreadPool class with 20 threads.

Because Robot Raconteur is a multithreaded library, members and handler functions may be called concurrently by different threads. This can easily lead to a number of concurrency errors, that can be hard to debug, and cause fatal program errors. An external block post [Top 20 C++ multithreading mistakes and how to avoid them](https://www.acodersjourney.com/top-20-cplusplus-multithreading-mistakes/) gives a good overview of these pitfalls. (Robot Raconteur uses `boost::thread` instead of `std::thread`, however they are nearly identical so the pitfals still apply!)

The most common problem with Robot Raconteur theading is not protecting shared data or an operation with a mutex. C++ multithreaded programming is a very complex topics, with [entire books](https://www.oreilly.com/library/view/mastering-c-multithreading/9781787121706/) written on just multithreading, so a full discussion of is beyond the scope of this documentation. In general, it is best to assume that all data and operations require a mutex lock, unless overlapping operations require separate mutex locks. Service objects can use the `_default_impl::this_lock` mutex. See the examples for suggestions on using C++ mutexes.

## Asynchronous operations {#cpp_async}

Robot Raconteur operations can be "synchronous" or "asynchronous". For the synchronous case, an operation is started, and the calling thread is blocked until the result is ready, or an error occurs. During the time the operation is executed, the calling thread is either waiting for completion or is used to complete some task. For networking libraries like Robot Raconteur, this can be very inefficient since a large amount of time is spent waiting for data communication over the network or for remote nodes to complete operations. For robotics libraries that interact with hardware, the thread may be waiting for a physical action to complete. During this time, the thread is essentially wasting resources. Since every operation requires its own thread, scalability is limited. With "asynchronous" programming, an operation is initiated, and a "handler" function is specified. Control of the thread is returned immediately, and the thread can continue doing other tasks, or be returned to the thread pool. When the operation is complete, the thread pool will dispatch a thread to execute the handler function.

Robot Raconteur provides both synchronous and asynchronous versions of most functions. The asynchronous versions typically begin with `Async` for API functions or `async_` for service objects. Asynchronous functions have four common signatures:

    void AsyncSomeFunction1(<ARGS>, std::function<void(<RETURN_TYPE>,RobotRaconteurExceptionPtr)> handler) // Function call with return or exception
    void AsyncSomeFunction2(<ARGS>, std::function<void(RobotRaconteurExceptionPtr)> handler) // Function call with exception
    void AsyncSomeFunction3(<ARGS>, std::function<void(<RETURN_TYPE>)> handler) // Function call with return but no exception
    void AsyncSomeFunction4(<ARGS>, std::function<void()> handler) // Function call with no return nor exception

The `<ARGS>` are one or more arguments passed to the function along with the handler. The asyncronous function returns a return value and/or/neither an exception. The existence or type of the return value depends on the function being called, and generally corresponds to the return type of the synchronous function. The exception is returned as a shared pointer of RobotRaconteurException or its subclasses. If no exception has occurred, the exception argument to the handler will be `nullptr`. If an exception has occurred, it will be an instance of RobotRaconteurException. If the exception is not `nullptr`, the return value is undefined.

The `handler` function can be created using `boost::bind` or using a C++11 lambda function. Using C++ lambda functions is highly recommended when C++11 is available.
A pseudocode example of using a lambda function with `AsyncSomeFunction1`, assuming there are two args and the return type is `std::string`:

    class MyClass : public boost::enable_shared_from_this()
    {
        boost::mutex this_lock;

        public:
        void call_some_function()
        {
            boost::mutex::scoped_lock lock(this_lock);
            int my_arg1 = 10;
            std::string my_arg2 = 20;
            boost::weak_ptr<MyClass> weak_this = shared_from_this();
            AsyncSomeFunction1(my_arg1, my_arg2, [weak_this](std::string ret, RobotRaconteurExceptionPtr exp)
            {
                boost::shared_ptr<MyClass> this_ = weak_this.lock();
                if (!this_)
                {
                    // The class has been destroyed, most likely due to shutdown.
                    return;
                }
                if (exp)
                {
                    // Handle the exception here
                    std::cout << "An exception occurred: " << exp.what()  << std::endl;
                    return;
                }

                // Do some work
                boost::mutex::scoped_lock lock(this_lock);
                this_->do_some_work(ret);

            });
        }

        void do_some_work(const std::string& ret)
        {
            // Do some work with the return value
        }
    };

The above is a very simple example of how asynchronous programming can be used. Note the use of locking. Since the lambda function is called later, after the function has exited by a separate thread it will not encounter a double-locking scenario. Also note the use of `weak_this`. A weak pointer is created, and "captured" by the lambda. The weak pointer is used to prevent "cyclic references", meaning that two objects have references to each other. In this situation, the total reference count will never go to zero, and the objects will never be destroyed. This is common problem with asynchronous programming, since a reference to the object is stored in the event handler. Using a `boost::weak_ptr` to store the object reference in the event handler prevents the creation of cyclical references. If the object is destroyed before the handler is called, the `boost::weak_ptr::lock()` function will result in `nullptr`, and the event handler can simply return since there is no work to do.

## Single Thread Operation {#cpp_single_thread}

### Motivation

The Robot Raconteur Core library uses Boost.Asio for networking and event handling with a thread pool. By default this thread pool uses 20 threads. Threads run handlers in parallel, meaning that handlers can be called from multiple threads, often concurrently. While this is normally the optimal behavior, there are two scenarios where this multithreaded behavior can be a problem:

1. Integrating with single threaded programs such as simulation packages
2. Soft-real time operation where the thread switching can introduce undesirable latency

Single-threaded operation has lower latency because it requires fewer context switches. When a handler is dispatched, Boost.Asio will select the next thread from the pool to run. If there are 20 threads, this results in a lot of switching between these threads. Using a single thread results in fewer context switches because the same thread will execute all the handlers. The disadvantage is that it will not use multiple cores, and only non-blocking (asynchronous) operations can be used.

Robot Raconteur Core is an asynchronous library, meaning that all internal operations are non-blocking. The user API exposes both asynchronous (non-blocking) and synchronous (blocking) options for most functions. The synchronous operations require multithreading because a handler thread needs to be available to run in the background to process incoming data, and notify the synchronous function when the result is ready. In single thread mode, these functions are not available. Synchronous functions that require multithreading are marked in the documentation, and when in a debug build will throw an assertion failure if they are called.

### boost::asio::io_context

Robot Raconteur Core provides the IOContextThreadPool to allow the use of an external `boost::asio::io_context`. This allows the user complete control over how handlers are executed. See the [boost::asio::iocontext reference] (https://www.boost.org/doc/libs/1_72_0/doc/html/boost_asio/reference/io_context.html) for more details. The `boost::asio::io_context` provides core I/O functionality, and manages dispatching handlers. Handlers are run when one of the following functions are used: `poll()`, `poll_one()`, `run()`, and `run_one()`. They have the following behavior:
| Function | Description |
|--- |--- |
| poll() | Run all available handlers, return immediately if none ready |
| poll_one() | Run only one handler, return immediately if none ready |
| run() | Run and wait for handlers. Return if no active objects (see `io_context::work` to keep alive) |
| run_one() | Wait and run one handler. Return if no active objects (see `io_context::work` to keep alive) |


### Robot Raconteur Core single thread operation

Only available in C++

To use single thread operation, a `boost::asio::io_context` is required. This can be either stored as a class variable or on the local stack in a main function. See the Boost.Asio documentation for more examples.

For this example, assume that `my_io_context` is declared on the stack.

    boost::asio::io_context my_io_context; // Could also be a class variable!

    // Create thread pool
    IOContextThreadPoolPtr io_context_thread_pool = boost::make_shared<IOContextThreadPool>(RobotRaconteurNode::sp(), boost::ref(asio_io_context), false);
    // Thread pool must be set before all other operations!
    RobotRaconteurNode::s()->SetThreadPool(io_context_thread_pool);

At this point the node is configured to use single threading. Because there are no background threads, `poll_one()`, `run_one()`, etc must be called to "pump" the handlers. As long as these functions are called, the node can be used normally, except that synchronous functions cannot be called.

For programs that have an update loop like simulation packages or soft real-time controllers, `poll()` should be called every time step. If there is a limit on the amount of time available, `poll_one()` should be called as many times as possible within the available time.

### Asynchronous Result Handlers

For clients, the operation is often a series of functions called in order rather than a constant loop handling events. Implementing sequences of function calls with asynchronous callbacks can be cumbersome. To simplify this design, the IOContextThreadPool_AsyncResultAdapter template can be used. This template calls `io_context::run_one()` in during IOContextThreadPool_AsyncResultAdapter::GetResult() to "pump" handlers until the result is available. An example from the test programs:

    IOContextThreadPool_AsyncResultAdapter<RRObjectPtr> connect_res(io_context);
    RobotRaconteurNode::s()->AsyncConnectService(url, "", RRMapPtr<std::string,RRValue>(), NULL, "", connect_res, 5000);
    RRObjectPtr c1 = connect_res.GetResult();
    async_testrootPtr c = rr_cast<async_testroot>(c1);

    IOContextThreadPool_AsyncResultAdapter<RRArrayPtr<double> > get_d2(io_context);
    c->async_get_d2(get_d2);
    double d2_b[]={ 1.374233e+19, 2.424327e-04, -1.615609e-02, ... };
    ca(AttachRRArray(d2_b,392,false),get_d2.GetResult());

    double d2_a[]={ 8.805544e-12, 3.735066e+12, ... };
    IOContextThreadPool_AsyncResultAdapter<void> set_d2(io_context);
    c->async_set_d2(AttachRRArrayCopy(d2_a,647),set_d2);
    set_d2.GetResult();

    IOContextThreadPool_AsyncResultAdapter<void> disconnect_res(io_context);
    RobotRaconteurNode::s()->AsyncDisconnectService(c1,disconnect_res);
    disconnect_res.GetResult();

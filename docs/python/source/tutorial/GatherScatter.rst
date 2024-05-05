Gather/Scatter operations
=========================

In many situations it is necessary to query data from dozens or hundreds of sensors. Thanks to the asynchronous
functionality discussed in :ref:`Async` this is possible and is limited only by memory and
network bandwidth. An example of a practical application of gather/scatter operations is that of an advanced lighting
system that needs to rapidly query large numbers of sensors, make control decisions, and then distribute updated
commands to a large number of lighting fixtures. Consider a list of connections that have already been connected and are
stored in a variable ``c_list``. Each connection is to a service that has the function ``ReadSensor()`` that returns
some important data. The following example will query each sensor concurrently and call the
handler when all the sensors have been queried.

.. code:: python


   global_err=[]
   global_data=[]
   ev=threading.Event()


   def read_finished(data, err):
       # "data" now contains a list of data
       # "err" contains a list with each element containing "None" or the exception that occurred for that read

       # Store data in global variables
       global global_err, global_data
       global_err=err
       global_data=data

       # Notify "main()" that the read is complete
       ev.set()


   # c_list contains a list of connections created with RobotRaconteur.s.ConnectService
   def start_read(c_list, handler, timeout):
       N=len(c_list}
       keys=[]
       keys_lock=threading.Lock()
       ret=[None]*N
       err=[None]*N

       def h(key, d, erri):
           done=False
           with keys_lock:
               if (erri is not None):
                   err[key]=erri
               else:
                   ret[key]=d
               keys.remove(key)

               if (len(keys)==0): done=True

           if (done):
               handler(ret,err)

       with keys_lock:
           for i in xrange(N):
               try:
                   c_list[i].async_ReadSensor(functools.partial(h,i),timeout)
                   keys.append(i)
               except Exception as erri:
                   err[i]=erri
           if (len(keys)==0):
               raise Exception("Could not read any sensors")

   def main()

       # Create all the c_list connections here

       # Start the read with a 100 ms timeout
       start_read(c_list,read_finished,0.1)

       # Wait for completion
       ev.wait()

       # Do something with the results
       print global_data
       print global_err

from pytest_pyodide import run_in_pyodide

@run_in_pyodide(packages=["RobotRaconteur", "numpy"])
async def test_timer(selenium):
    import asyncio
    import RobotRaconteur as RR
    RRN = RR.RobotRaconteurNode.s

    evt = asyncio.Event()

    i=[0]

    def timer_cb(ev):
        
        if ev.stopped:
            return
        print("Got timer callback!")
        i[0]+=1
        if i[0] > 10:            
            timer.TryStop()
            evt.set()
            print("Stopped")

    timer = RRN.CreateTimer(0.1,timer_cb,False)
    timer.Start()

@run_in_pyodide(packages=["RobotRaconteur", "numpy"])
async def test_post(selenium):
    import asyncio
    import RobotRaconteur as RR
    RRN = RR.RobotRaconteurNode.s

    evt = asyncio.Event()

    def post_cb():
        
        print("Got post callback")
        evt.set()

    RRN.PostToThreadPool(post_cb)
    
    assert await asyncio.wait_for(evt.wait(), 5)
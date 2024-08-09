# reynard_the_robot_client_pyodide.py - Reynard the Robot client using Pyodide

from RobotRaconteur.Client import *
import js
import asyncio
import traceback


async def main():
    status_div = js.document.getElementById("status")
    status_div.innerHTML = "Connecting..."
    error_log = js.document.getElementById("error_log")
    try:

        RRN.SetLogLevel(RR.LogLevel_Debug)

        # All Robot Raconteur calls must use asynchronous variants
        # in Pyodide because browsers are single threaded

        # Connect to Reynard the Robot service using websocket
        url = 'rr+tcp://localhost:29200?service=reynard'
        c = await RRN.AsyncConnectService(url, None, None, None, None)

        received_messages_div = js.document.getElementById("received_messages")

        def new_message(msg):
            received_messages_div.innerHTML += f"{msg}<br>"

        # Connect a callback function to listen for new messages
        c.new_message += new_message

        # Handle when the send_message button is clicked
        send_button = js.document.getElementById("send_message")

        async def do_send_message():
            try:
                message = js.document.getElementById("message").value
                await c.async_say(message, None)
            except Exception as e:
                error_log.innerHTML += f"Error: {traceback.format_exc()}" + "<br>"
        send_button.onclick = lambda _: asyncio.create_task(do_send_message())

        teleport_button = js.document.getElementById("teleport")

        async def do_teleport():
            try:
                x = float(js.document.getElementById("teleport_x").value) * 1e-3
                y = float(js.document.getElementById("teleport_y").value) * 1e-3
                await c.async_teleport(x, y, None)
            except Exception as e:
                error_log.innerHTML += f"Error: {traceback.format_exc()}" + "<br>"
        teleport_button.onclick = lambda _: asyncio.create_task(do_teleport())

        status_div.innerHTML = "Connected"

        # Run loop to update Reynard position
        while True:
            try:
                # Read the current state using a wire "peek". Can also "connect" to receive streaming updates.
                state, _ = await c.state.AsyncPeekInValue(None)
                js.document.getElementById(
                    "reynard_position").innerHTML = f"x: {state.robot_position[0]}, y: {state.robot_position[1]}"
            except:
                status_div.innerHTML = "Error"
                error_log.innerHTML += f"Error: {traceback.format_exc()}" + "<br>"
            await asyncio.sleep(0.1)

    except:
        status_div.innerHTML = "Error"
        error_log.innerHTML += f"Error: {traceback.format_exc()}" + "<br>"

asyncio.create_task(main())

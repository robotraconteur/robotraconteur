"""End to end test of the Emscripten Reynard the Robot client example.

Starts the Reynard the Robot service and a static HTTP server for the example build
directory, then drives the client page and the Reynard GUI in headless Chromium using
Playwright. Requires the reynard-the-robot and playwright Python packages, and
"playwright install chromium".

Usage: python test_reynard_client_emscripten.py <example build directory>
"""

import socket
import subprocess
import sys
import time

from playwright.sync_api import expect, sync_playwright

HTTP_PORT = 8000
CLIENT_URL = f"http://localhost:{HTTP_PORT}/reynard_the_robot_client_emscripten.html"
REYNARD_GUI_URL = "http://localhost:29201/"


def wait_port(port, timeout=60):
    t0 = time.time()
    while time.time() - t0 < timeout:
        try:
            socket.create_connection(("127.0.0.1", port), 1).close()
            return
        except OSError:
            time.sleep(0.2)
    raise RuntimeError(f"Port {port} did not open")


def run_test(page, gui):
    expect(page.locator("#status")).to_have_text("Connected", timeout=30000)
    expect(page.locator("#reynard_position")).to_contain_text("x: ", timeout=10000)

    # Teleport uses millimeters in the page, meters in the service
    page.fill("#teleport_x", "100")
    page.fill("#teleport_y", "-200")
    page.click("#teleport")
    expect(page.locator("#reynard_position")).to_have_text("x: 0.1, y: -0.2", timeout=10000)

    # say() shows the message in the Reynard GUI
    page.fill("#message", "Hello from Emscripten")
    page.click("#send_message")
    expect(gui.locator("#reynard-output")).to_contain_text("Hello from Emscripten", timeout=10000)

    # A message typed in the GUI is received by the client as a new_message event,
    # and must be shown as text, not HTML
    gui.fill("#reynard-talk-input", "Hello <i>from</i> the GUI")
    gui.click("#reynard-send-submit")
    expect(page.locator("#received_messages")).to_contain_text("Hello <i>from</i> the GUI", timeout=10000)
    assert page.locator("#received_messages i").count() == 0, "Message text was interpreted as HTML"

    expect(page.locator("#error_log")).to_have_text("")
    page.fill("#teleport_x", "abc")
    page.click("#teleport")
    expect(page.locator("#error_log")).to_contain_text("Invalid teleport position", timeout=10000)

    expect(page.locator("#status")).to_have_text("Connected")


def main():
    if len(sys.argv) != 2:
        print(__doc__)
        return 2
    build_dir = sys.argv[1]

    reynard = subprocess.Popen([sys.executable, "-m", "reynard_the_robot",
                                f"--robotraconteur-tcp-ws-add-origins=http://localhost:{HTTP_PORT}"])
    http = subprocess.Popen([sys.executable, "-m", "http.server", str(HTTP_PORT), "--bind", "127.0.0.1",
                             "-d", build_dir], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        wait_port(29200)
        wait_port(29201)
        wait_port(HTTP_PORT)
        with sync_playwright() as p:
            browser = p.chromium.launch()
            try:
                context = browser.new_context()
                # Open the GUI first, it only shows say() messages received while it is open
                gui = context.new_page()
                gui.goto(REYNARD_GUI_URL)
                page = context.new_page()
                page.on("console", lambda m: print("[client console]", m.text))
                page.on("pageerror", lambda e: print("[client page error]", e))
                page.goto(CLIENT_URL)
                run_test(page, gui)
            finally:
                browser.close()
    finally:
        http.terminate()
        reynard.terminate()
        try:
            reynard.wait(10)
        except subprocess.TimeoutExpired:
            reynard.kill()

    print("Emscripten Reynard client test passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())

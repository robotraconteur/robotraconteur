# Copyright 2011-2020 Wason Technology, LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s

node_setup = RR.ClientNodeSetup(argv=__import__("sys").argv)
node_setup.ReleaseNode()

local_transport = node_setup.local_transport
tcp_transport = node_setup.tcp_transport
hardware_transport = node_setup.hardware_transport
intra_transport = node_setup.intra_transport

__all__ = [
    'RR',
    'RRN',
    'node_setup',
    'local_transport',
    'tcp_transport',
    'hardware_transport',
    'intra_transport',
]

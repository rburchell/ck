#!/usr/bin/env python
##
## This file is part of ContextKit.
##
## Copyright (C) 2008, 2009 Nokia. All rights reserved.
##
## Contact: Marius Vollmer <marius.vollmer@nokia.com>
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public License
## version 2.1 as published by the Free Software Foundation.
##
## This library is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
## 02110-1301 USA
##
##
## Requires python2.5-gobject and python2.5-dbus
##

import sys
import unittest
import os
from ContextKit.cltool import CLTool
from time import sleep

class MultiProvider(unittest.TestCase):

        def testPingPong(self):
                """
                Description
                This test verifies that valueChanged signals are emitted
                when two providers continuously set new values to the same
                property.

                Pre-conditions

                Steps
                1. starts two providers (A and B) providing the same property
                   the property is respectively initialised to V1 and V2
                2. starts a client
                3. provider A sets the property to V2, a signal is emitted
                   since V2 is different than previous value V1
                4. provider B sets the property to V1, a signal is emitted
                   since V1 is different than previous value V2
                5. provider A sets the property to V1, a signal is emitted
                   since V1 is different than previous value V2
                6. provider B sets the property to V2, a signal is emitted
                   since V2 is different than previous value V1

                Post-conditions

                References
                        None
                """
                provider_a = CLTool("context-provide", "--v2", "--session", "com.nokia.test.a",
                                  "int","test.int","1")
                provider_a.expect("Added key") # wait for it

                provider_b = CLTool("context-provide", "--v2", "--session", "com.nokia.test.b",
                                  "int","test.int","2")
                provider_b.expect("Added key") # wait for it

                client = CLTool("client")

                client.send("assign session com.nokia.test.a providerA")
                client.expect("Assigned providerA")

                client.send("assign session com.nokia.test.b providerB")
                client.expect("Assigned providerB")

                sleep(2)

                client.send("subscribe providerA test.int")
                self.assert_(client.expect("Subscribe returned: int:1"))

                client.send("subscribe providerB test.int")
                self.assert_(client.expect("Subscribe returned: int:2"))

                provider_a.send("test.int = 2")
                client.send("waitforchanged 1000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.a /org/maemo/contextkit/test/int int:2"))

                provider_b.send("test.int = 1")
                client.send("waitforchanged 1000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.b /org/maemo/contextkit/test/int int:1"))

                provider_a.send("test.int = 1")
                client.send("waitforchanged 1000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.a /org/maemo/contextkit/test/int int:1"))

                provider_b.send("test.int = 2")
                client.send("waitforchanged 1000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.b /org/maemo/contextkit/test/int int:2"))

                client.close()
                provider_a.close()
                provider_b.close()

        def testOverheardValues(self):
                """
                Description
                This test verifies that valueChanged signals are emitted
                when a provider overhears the value from another provider.

                Pre-conditions

                Steps
                1. starts two providers (A and B) providing the same property
                   the property is respectively initialised to V1 and V2
                2. starts a client
                3. provider A sets the property to V3, a signal is emitted
                   since V3 is different than previous value V1
                4. provider B sets the property to V4, a signal is emitted
                   since V4 is different than previous value V2
                5. provider A sets the property to V3, a signal is emitted
                   because A has overheard V4 from provider B

                Post-conditions

                References
                        None
                """
                provider_a = CLTool("context-provide", "--v2", "--session", "com.nokia.test.a",
                                  "double","test.double","1.5")
                provider_a.expect("Added key") # wait for it

                provider_b = CLTool("context-provide", "--v2", "--session", "com.nokia.test.b",
                                  "double","test.double","2.5")
                provider_b.expect("Added key") # wait for it

                client = CLTool("client")

                client.send("assign session com.nokia.test.a providerA")
                self.assert_(client.expect("Assigned providerA"))

                client.send("assign session com.nokia.test.b providerB")
                self.assert_(client.expect("Assigned providerB"))

                sleep(2)

                client.send("subscribe providerA test.double")
                self.assert_(client.expect("Subscribe returned: double:1.5"))

                client.send("subscribe providerB test.double")
                self.assert_(client.expect("Subscribe returned: double:2.5"))

                provider_a.send("test.double = 3.5")
                client.send("waitforchanged 3000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.a /org/maemo/contextkit/test/double double:3.5"))

                provider_b.send("test.double = 4.5")
                client.send("waitforchanged 3000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.b /org/maemo/contextkit/test/double double:4.5"))

                provider_a.send("test.double = 3.5")
                client.send("waitforchanged 3000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.a /org/maemo/contextkit/test/double double:3.5"))

                client.close()
                provider_a.close()
                provider_b.close()


        def testBothProvidersSameValue(self):
                """
                Description
                This test verifies that no signals are emitted when
                both providers are setting the same value to the same property.

                Pre-conditions

                Steps
                1. starts two providers (A and B) providing the same property
                   the property is respectively initialised to V1 and V2
                2. starts a client
                3. provider A sets the property to V2, a signal is emitted
                   since V2 is different than previous value V1
                4. provider B sets the property to V2, no signal is emitted
                   since V2 is identical to previous value V2 and V2 from Provider A
                5. provider A sets the property to V2, no signal is emitted
                   since V2 is identical to previous value V2 and V2 from Provider B

                Post-conditions

                References
                        None
                """
                provider_a = CLTool("context-provide", "--v2", "--session", "com.nokia.test.a",
                                  "truth","test.truth","true")
                provider_a.expect("Added key") # wait for it

                provider_b = CLTool("context-provide", "--v2", "--session", "com.nokia.test.b",
                                    "truth","test.truth","false")
                provider_b.expect("Added key") # wait for it

                client = CLTool("client")

                client.send("assign session com.nokia.test.a providerA")
                self.assert_(client.expect("Assigned providerA"))

                client.send("assign session com.nokia.test.b providerB")
                self.assert_(client.expect("Assigned providerB"))

                sleep(2)

                client.send("subscribe providerA test.truth")
                self.assert_(client.expect("Subscribe returned: bool:true"))

                client.send("subscribe providerB test.truth")
                self.assert_(client.expect("Subscribe returned: bool:false"))

                provider_a.send("test.truth = false")
                client.send("waitforchanged 3000")
                self.assert_(client.expect("ValueChanged: com.nokia.test.a /org/maemo/contextkit/test/truth bool:false"))

                provider_b.send("test.truth = false")
                client.send("waitforchanged 5000")
                self.assert_(client.expect("Timeout", timeout = 6))

                provider_a.send("test.truth = false")
                client.send("waitforchanged 5000")
                self.assert_(client.expect("Timeout", timeout = 6))

                client.close()
                provider_a.close()
                provider_b.close()


if __name__ == "__main__":
        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
        unittest.main()

#! /usr/bin/env python
# ==========================================================================
# This scripts performs unit tests for the cscaldb script.
#
# Copyright (C) 2016 Juergen Knoedlseder
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# ==========================================================================
import gammalib
import cscripts
import test_cscripts_class


# ============================= #
# Test class for cscaldb script #
# ============================= #
class Test(test_cscripts_class.cscripts_test):
    """
    Test class for cscaldb script

    This test class makes unit tests for the cslightcrv script by using it
    from the command line and from Python.
    """

    # Constructor
    def __init__(self):
        """
        Constructor.
        """
        # Call base class constructor
        test_cscripts_class.cscripts_test.__init__(self)

        # Return
        return

    # Set test functions
    def set(self):
        """
        Set all test functions.
        """
        # Set test name
        self.name('cscaldb')

        # Append tests
        self.append(self._test_cmd, 'Test cscaldb on command line')
        self.append(self._test_python, 'Test cscaldb from Python')

        # Return
        return

    # Test cscaldb on command line
    def _test_cmd(self):
        """
        Test cscaldb on the command line.
        """
        # Set script name
        cscaldb = self._script('cscaldb')

        # Setup cscaldb command
        cmd = cscaldb+' logfile="cscaldb_cmd1.log" chatter=1'

        # Check if execution of wrong command fails
        self.test_assert(self._execute('command that does not exist') != 0,
             'Self test of test script')

        # Check if execution was successful
        self.test_assert(self._execute(cmd) == 0,
             'Check successful execution from command line')

        # Return
        return

    # Test cscaldb from Python
    def _test_python(self):
        """
        Test cscaldb from Python.
        """
        # Set-up cscaldb
        caldb = cscripts.cscaldb()
        caldb['logfile'] = 'cscaldb_py1.log'
        caldb['chatter'] = 2

        # Run script
        caldb.logFileOpen()   # Make sure we get a log file
        caldb.run()

        # Return
        return

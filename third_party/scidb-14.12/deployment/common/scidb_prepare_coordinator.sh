#!/bin/bash
#
# BEGIN_COPYRIGHT
#
# This file is part of SciDB.
# Copyright (C) 2008-2014 SciDB, Inc.
#
# SciDB is free software: you can redistribute it and/or modify
# it under the terms of the AFFERO GNU General Public License as published by
# the Free Software Foundation.
#
# SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
# INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
# NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
# the AFFERO GNU General Public License for the complete license terms.
#
# You should have received a copy of the AFFERO GNU General Public License
# along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
#
# END_COPYRIGHT
#

set -eu

username="${1}"
database="${2}"
SCIDB_VER="${3}"

expect <<EOF
set timeout -1
spawn sudo -u postgres /opt/scidb/${SCIDB_VER}/bin/scidb.py init_syscat ${database}
expect eof
catch wait result
if {[lindex \$result 3]!=0} { exit [lindex \$result 3] }
EOF
su ${username} -c "/opt/scidb/${SCIDB_VER}/bin/scidb.py initall-force ${database}"

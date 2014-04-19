# chjdkver

## What is this?

It is difficult to switch jdk version on OS X environment which
contains several jdk versions.  Because, the relation among symbolic
links is quite complex.

This program simply switches jdk versions.

## Usage

### Preparation

Oracle JDKs are installed into `/Library/Java/JavaVirtualMachines`.
This program shows only files whose name is contained numbers, dot(.),
hyphen, and underbar.  Therefore, use symbolic link.

    $ cd /Library/Java/JavaVirtualMachines
    $ ls -F
    jdk1.7.0_45.jdk/    jdk1.8.0_20.jdk/
    $ chjdkver
    $ sudo ln -s jdk1.7.0_45.jdk ./1.7
    $ sudo ln -s jdk1.8.0_20.jdk ./1.8
    $ ls -F
    1.7@             1.8@             jdk1.7.0_45.jdk/ jdk1.8.0_20.jdk/
    $ chjdkver
    1.7 (*)
    1.8

Note that,
`/System/Library/Frameworks/JavaVM.framework/Versions/A/Commands` must
be linked to `../CurrentJDK/Contents/Home/bin`.  Because
`/usr/bin/java` and etc. are symblic linked to
`/System/Library/Frameworks/JavaVM.framework/Versions/A/Commands/java`,
and the `A` directory is not symbolic link.

    $ sudo chjdkver --init_env

Above command shows the following commands.

    $ cd /System/Library/Frameworks/JavaVM.framework/Versions/A
    $ sudo mv Commands Commands.back
    $ sudo ln -s ../CurrentJDK/Contents/Home/bin ./Commands

### Switch JDK

    $ java -version
    java version "1.7.0_45"
    Java(TM) SE Runtime Environment (build 1.7.0_45-b18)
    Java HotSpot(TM) 64-Bit Server VM (build 24.45-b08, mixed mode)
    $ chjdkver
    1.7 (*)
    1.8
    $ sudo chjdkver 1.8
    $ java -version
    java version "1.8.0_20-ea"
    Java(TM) SE Runtime Environment (build 1.8.0_20-ea-b05)
    Java HotSpot(TM) 64-Bit Server VM (build 25.20-b05, mixed mode)
    $ chjdkver
    1.7
    1.8 (*)

## Future work

* Do initial settings.

## Authors

* Author: Haruaki Tamada
* Web page: http://github.com/tamada/chjdkver

## License

Apache License Version 2.0

    Copyright 2013- Haruaki Tamada

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

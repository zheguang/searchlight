# or-tools 1.0.0
make third_party
```
svn co http://gflags.googlecode.com/svn/trunk/ -r 76 dependencies/sources/gflags-76
svn: E160013: Unable to connect to a repository at URL 'http://gflags.googlecode.com/svn/trunk'
svn: E160013: '/svn/trunk' path not found
```

# scidb
CMakefile, set to false as:
```
set(Boost_NO_SYSTEM_PATHS FALSE)
```

error: 'treat_direct' was not declared in this scope
This means libpqxxx4 was installed instead of libpqxx3.  Install libpqxx3 instead.


# scidb
After build everything, prepare scidb cluster:
```
bash scidb-prepare-db.sho
```
With postgres admin: default (postgres)
Admin password: default (empty)
System catelog owner: sl
System catelog owner password: sl
System catelog db name: test


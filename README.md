php-cassandra - PHP Driver/Extension For Apache Cassandra (alpha)
=============

A PHP driver for Apache Cassandra. This driver works exclusively with the Cassandra Query Language version 3 (CQL3) and Cassandra's binary protocol.

## Building

#### CentOS 6 - Requirements

Git version >= 1.7.10

Check your git version:
```
git --version
```

if git version < 1.7.10 install newer version for example from rpmforge (http://wiki.centos.org/AdditionalResources/Repositories/RPMForge#head-f0c3ecee3dbb407e4eed79a56ec0ae92d1398e01)
```
wget http://pkgs.repoforge.org/rpmforge-release/rpmforge-release-0.5.3-1.el6.rf.x86_64.rpm
sudo rpm --import http://apt.sw.be/RPM-GPG-KEY.dag.txt
sudo rpm -K rpmforge-release-0.5.3-1.el6.rf.*.rpm
rpm -i rpmforge-release-0.5.3-1.el6.rf.*.rpm
sudo yum erase git
### Enable RPM extras repo => set enabled=1
sudo nano /etc/yum.repos.d/rpmforge.repo
sudo yum install git
## Disable RPM extras repo => set enabled=0
sudo nano /etc/yum.repos.d/rpmforge.repo

## check new git version
git --version
```

Install required packages
```
yum install cmake gcc-c++ openssl-devel libssh2-devel
```

Install boost >= 1.55
```
sudo wget http://repo.enetres.net/enetres.repo -O /etc/yum.repos.d/enetres.repo
sudo yum install boost-devel
```

Install PHP >= 5.3.10
```
wget http://dl.fedoraproject.org/pub/epel/6/i386/epel-release-6-8.noarch.rpm
wget http://rpms.famillecollet.com/enterprise/remi-release-6.rpm
wget http://rpms.famillecollet.com/RPM-GPG-KEY-remi
sudo rpm --import RPM-GPG-KEY-remi
sudo rpm -Uvh remi-release-6*.rpm epel-release-6*.rpm
## Enable Remi repo => set enabled=1
sudo nano /etc/yum.repos.d/remi.repo
sudo yum install php-devel
## Disable Remi repo => set enabled=0
sudo nano /etc/yum.repos.d/remi.repo
```

The library use official c++ driver by DataStax https://github.com/datastax/cpp-driver
Before build php-cassandra you should download and install DataStax C++ Driver for Apache Cassandra.

```
git clone https://github.com/datastax/cpp-driver/ --branch deprecated --single-branch cpp-driver
cd cpp-driver
cmake . && make && make install
```

```
git clone https://github.com/michaltrmac/php-cassandra.git --branch get_row --single-branch php-cassandra
cd php-cassandra
phpize && ./configure && make
```

After ```make``` you can check extension and see cassandra module

```
php -d="extension=modules/cassandra.so" -m
```

##Troubleshooting Common Problems

- If you'll see the message
```
PHP Warning: PHP Startup: Unable to load dynamic library 'modules/cassandra.so' - libcql.so.0: cannot open shared object file: No such file or directory in Unknown on line 0
```

Try to add path to cql library. On Debian system cql library installed to /usr/local/lib
```
LD_LIBRARY_PATH=/usr/local/lib php -d="extension=modules/cassandra.so" -m
```

Or you can create symlink

```
sudo ln -s /usr/local/lib/libcql.so.0 /usr/lib/libcql.so.0
```

Don't forget to add extension to your php.ini if you'll try next example

## Examples
### Minimal Working Example - simple query against system.schema_keyspaces.
```php
// Suppose you have the Cassandra cluster at 127.0.0.1, 
// listening at default port (9042).
$builder  = new CqlBuilder();
$builder->addContactPoint("127.0.0.1");

// Now build a model of cluster and connect it to DB.
$cluster  = $builder->build();
$session  = $cluster->connect();

// Write a query, switch keyspaces.
$query    = new CqlQuery('SELECT * FROM system.schema_keyspaces');

// Send the query.
$future   = $session->query($query);

// Wait for the query to execute; retrieve the result.
$future->wait();
$result   = $future->getResult();

if (null === $future->getError()) {

	echo "rowCount: {$result->getRowCount()}\n";
	
	while ($result->next()) {
		echo "strategy_options: " . $result->get("strategy_options") . "\n";
		// or you can use $result->get() to get complete row as associative array
		// var_dump($result->get());
	}
	
}

// Boilerplate: close the connection session and perform the cleanup.
$session->close();
$cluster->shutdown();
```

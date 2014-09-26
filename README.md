php-cassandra - PHP Driver/Extension For Apache Cassandra (alpha)
=============

A PHP driver for Apache Cassandra. This driver works exclusively with the Cassandra Query Language version 3 (CQL3) and Cassandra's binary protocol.

## Building

The library use official c++ driver by DataStax https://github.com/datastax/cpp-driver
Before build php-cassandra you should download and install DataStax C++ Driver for Apache Cassandra.

```
git clone https://github.com/datastax/cpp-driver/ --branch deprecated --single-branch cpp-driver
cd cpp-driver
cmake . && make && make install
```

```
git clone https://github.com/aparkhomenko/php-cassandra.git
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
	}
	
}

// Boilerplate: close the connection session and perform the cleanup.
$session->close();
$cluster->shutdown();
```

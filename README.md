php-cassandra - PHP Driver/Extension For Apache Cassandra (alpha)
=============

A PHP driver for Apache Cassandra. This driver works exclusively with the Cassandra Query Language version 3 (CQL3) and Cassandra's binary protocol.

## Building

The library use official c++ driver by DataStax https://github.com/datastax/cpp-driver
Before build php-cassandra you should download and install DataStax C++ Driver for Apache Cassandra.

```
git clone https://github.com/datastax/cpp-driver.git
cd cpp-driver
cmake . && make && make cql_demo && make cql_test && make test && make install
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

Don't forget to add extension to your php.ini if you'll try next example

## Examples
### Minimal Working Example - simple query against system.schema_keyspaces.
```php
$builder  = new CqlBuilder();
$builder->addContactPoint("127.0.0.1");

$cluster  = $builder->build();
$session  = $cluster->connect();

$query    = new CqlQuery('SELECT * FROM system.schema_keyspaces;');
$future   = $session->query($query);

$future->wait();
$result   = $future->getResult();

echo "rowCount: {$result->getRowCount()}\n";

while ($result->next()) {
	echo "strategy_options: " . $result->get("strategy_options") . "\n";
}

$session->close();
$cluster->shutdown();
```
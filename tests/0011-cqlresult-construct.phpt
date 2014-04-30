--TEST--
Test cassandra CqlCluster connect
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/config.inc');

$builder  = new CqlBuilder();
$builder->addContactPoint($host, $port);

$cluster  = $builder->build();
$session  = $cluster->connect();

$query    = new CqlQuery('SELECT * FROM system.schema_keyspaces');

// Send the query.
$future   = $session->query($query);
$future->wait();

$result   = $future->getResult();
echo $result instanceof CqlResult ? "OK" : "FAILURE";

$session->close();
$cluster->shutdown();

?>
--EXPECT--
OK
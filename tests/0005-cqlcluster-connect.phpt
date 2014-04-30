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
echo $session instanceof CqlSession ? "OK" : "FAILURE";

$session->close();
$cluster->shutdown();

?>
--EXPECT--
OK
--TEST--
Test cassandra CqlCluster construction
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/config.inc');

$builder  = new CqlBuilder();
$builder->addContactPoint($host, $port);

$cluster  = $builder->build();
echo $cluster instanceof CqlCluster ? "OK" : "FAILURE";

$cluster->shutdown();

?>
--EXPECT--
OK
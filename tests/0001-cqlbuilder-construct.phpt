--TEST--
Test cassandra CqlBuilder construction
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/config.inc');

$builder = new CqlBuilder();
echo "OK";

?>
--EXPECT--
OK
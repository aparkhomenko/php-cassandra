--TEST--
Test cassandra CqlBuilder addContactPoint
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/config.inc');

$builder  = new CqlBuilder();
$builder->addContactPoint($host);
echo "OK";

?>
--EXPECT--
OK
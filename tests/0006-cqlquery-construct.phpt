--TEST--
Test cassandra CqlQuery construction
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/config.inc');

$query    = new CqlQuery('');
echo 'OK';

?>
--EXPECT--
OK